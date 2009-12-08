//
//
//  Description:
//      Implementation of Log File browser widget.  This widget provides
//      a browser for all of the log files for a cluster run.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ResultsBrowser.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include "ArchiveFileResource.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WMessageBox>
#include <Wt/WDialog>
#include <Wt/WStackedWidget>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <fstream>
#include <iostream>
#include <string>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::processes;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ResultsBrowser::ResultsBrowser(WContainerWidget *parent) :
    FileBrowser(parent),
    mApp(WApplication::instance()),
    mTarBuffer(NULL)
{
    mTreeView->selectionChanged().connect(SLOT(this, ResultsBrowser::resultChanged));
    mTreeView->setMinimumSize(400, WLength::Auto);


    mDownloadButton = new WPushButton("Download All Results");
    WHBoxLayout *loadingLayout = new WHBoxLayout();
    WContainerWidget *loadingContainer = new WContainerWidget;
    WImage *loading = new WImage("icons/ajax-loader-trans.gif");
    loadingLayout->addWidget(new WText("Creating tar file, please wait..."));
    loadingLayout->addWidget(loading);
    loadingContainer->setLayout(loadingLayout);

    mTarMemResource = new ArchiveFileResource();
    WAnchor *downloadAnchor = new WAnchor(mTarMemResource);
    downloadAnchor->setTarget(TargetThisWindow);
    WPushButton *downloadTarButton = new WPushButton("Tar File Ready -- Click to Download", downloadAnchor);

    mDownloadStack = new WStackedWidget();
    mDownloadStack->addWidget(mDownloadButton);
    mDownloadStack->addWidget(loadingContainer);
    mDownloadStack->addWidget(downloadAnchor);

    mRefreshButton = new WPushButton("Refresh Available Results");
    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mTreeView, 0, 0);
    layout->addWidget(mRefreshButton, 1, 0, AlignCenter);
    layout->addWidget(mDownloadStack, 2, 0, AlignCenter);
    layout->setRowStretch(0, 1);
    setLayout(layout);

    mRefreshButton->clicked().connect(SLOT(this, ResultsBrowser::refreshResults));
    mDownloadButton->clicked().connect(SLOT(this, ResultsBrowser::downloadResults));

    resetAll();
}

///
//  Destructor
//
ResultsBrowser::~ResultsBrowser()
{
    if (mTarBuffer != NULL)
    {
        delete [] mTarBuffer;
        mTarBuffer = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default state
//
void ResultsBrowser::resetAll()
{
    WModelIndexSet noSelection;
    mTreeView->setSelectedIndexes(noSelection);
    mModel->clear();

    populateBrowser();

    mTreeView->expandToDepth(4);

    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO RESULTS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
    }

    mDownloadStack->setCurrentIndex(0);
    if (mTarBuffer != NULL)
    {
        delete [] mTarBuffer;
        mTarBuffer = NULL;
    }
}


///
//  Set the base directory for the results
//
void ResultsBrowser::setResultsBaseDir(const std::string& baseDir)
{
    mResultsBaseDir = baseDir;
}

///
// Set the name of the pipeline
//
void ResultsBrowser::setPipelineName(const std::string& pipelineName)
{
    mPipelineName = pipelineName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Populate the results job model by parsing patterns stored in the configuration
//  file
//
void ResultsBrowser::populateBrowser()
{
    WStandardItemModel *model = getConfigXMLPtr()->getResultsPipelineTree(mPipelineName);
    mResultFileEntries.clear();

    if (model != NULL)
    {
        int index = 0;
        for(int row = 0; row < model->rowCount(); row++)
        {
            WStandardItem *item = model->item(row);

            addFilesFromTree(item, mResultsBaseDir, -1, index);
        }
    }
}

///
///  Add files from tree
///
void ResultsBrowser::addFilesFromTree(WStandardItem *item, const std::string& baseDir, int depth, int &index)
{
    boost::any data = item->data(UserRole);
    if (!data.empty() && exists(baseDir))
    {
        ConfigXML::FilePatternNode node = boost::any_cast<ConfigXML::FilePatternNode>(data);


        path basePath(baseDir);
        regex regEx;

        try
        {
            regEx = regex(node.mExpression);
        }
        catch(...)
        {
            WApplication::instance()->log("error") << "Invalid regular expression '" << node.mExpression << "' in config XML file";
            regEx = regex();
        }

        for(directory_iterator dirIter(basePath); dirIter != directory_iterator(); ++dirIter)
        {
            const string& fileName = dirIter->path().filename();

            boost::smatch what;

            // Skip if no match
            if( !boost::regex_match( fileName, what, regEx ) )
                continue;

            if (is_directory(dirIter->path()) && node.mDirectory)
            {
                // Now do this for all the children of this directory
                for(int row = 0; row < item->rowCount(); row++)
                {
                    addFilesFromTree(item->child(row), dirIter->path().string(), depth + 1, index);
                }
            }
            else if(!is_directory(dirIter->path()) && !node.mDirectory)
            {
                addEntry(false, depth,
                         dirIter->path().branch_path().string(), dirIter->path().leaf(),
                         index);

                mResultFileEntries.push_back(dirIter->path().string());
                index++;
            }
        }
    }
}


///
//  Results selection changed by user
//
void ResultsBrowser::resultChanged()
{
    if (mTreeView->selectedIndexes().empty())
        return;


    WModelIndex selected = *mTreeView->selectedIndexes().begin();
    boost::any fileEntryDataIndex = selected.data(UserRole);

    if (!fileEntryDataIndex.empty())
    {
        int index = boost::any_cast<int>(fileEntryDataIndex);

        mResultFileSelected.emit(mResultFileEntries[index]);
    }
}

///
//  Handle refresh results button clicked [slot]
//
void ResultsBrowser::refreshResults()
{
    mModel->clear();

    populateBrowser();
    mTreeView->expandToDepth(4);


    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO RESULTS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
    }
}

///
//  Download results button clicked [slot]
//
void ResultsBrowser::downloadResults()
{
    mApp->log("info") << "Starting thread";

    mDownloadStack->setCurrentIndex(1);
    mArchiveThread = new boost::thread(boost::bind(&ResultsBrowser::createTarArchive, this));
}

///
//  Create tar file for directory in a thread
//
void ResultsBrowser::createTarArchive()
{
    char *tmpName = strdup("/tmp/archive_XXXXXX");

    if (mkstemp(tmpName) == -1)
    {
        WApplication::instance()->log("error") << "Error creating file on server: " << tmpName;
        return;
    }

    path dirPath = path(mResultsBaseDir);
    string cmdToExecute;
    cmdToExecute = string("tar cvzf ") + tmpName + " -C " + dirPath.branch_path().string() + " " + dirPath.leaf();

    context ctx;
    child c = launch_shell(cmdToExecute.c_str(), ctx);
    boost::processes::status s = c.wait();

    // Now read the temp file and delete it
    FILE *fp = fopen(tmpName, "rb");
    if (fp != NULL)
    {
        long curPos = ftell(fp);
        long size;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, curPos, SEEK_SET);

        if (mTarBuffer != NULL)
        {
            delete [] mTarBuffer;
        }


        mTarBuffer = new unsigned char [size];

        if (fread(mTarBuffer, 1, size, fp) != size)
        {
            mApp->log("error") << "Error reading bytes of tar file " << tmpName;
        }
        fclose(fp);

        mTarMemResource->setData(mTarBuffer, size);
        mTarMemResource->suggestFileName(path(mResultsBaseDir).leaf() + ".tar.gz");

        // Remove the temp file
        cmdToExecute = string("rm -rf ") + tmpName;
        context ctx;
        child c = launch_shell(cmdToExecute.c_str(), ctx);
        boost::processes::status s = c.wait();

        // First, take the lock to safely manipulate the UI outside of the
        // normal event loop, by having exclusive access to the session.
        WApplication::UpdateLock lock = mApp->getUpdateLock();

        mTarMemResource->setChanged();

        // Set to download tarball button
        mDownloadStack->setCurrentIndex(2);

        mApp->triggerUpdate();
    }
    else
    {
        // First, take the lock to safely manipulate the UI outside of the
        // normal event loop, by having exclusive access to the session.
        WApplication::UpdateLock lock = mApp->getUpdateLock();
        mApp->log("error") << "Error opening tar file " << tmpName;
        mDownloadStack->setCurrentIndex(0);
        mApp->triggerUpdate();
    }
}
