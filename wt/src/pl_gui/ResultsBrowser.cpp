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
#include "ResultsBrowser.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
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


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ResultsBrowser::ResultsBrowser(WContainerWidget *parent) :
    FileBrowser(parent)
{
    mTreeView->selectionChanged().connect(SLOT(this, ResultsBrowser::resultChanged));
   //TreeView->resize(600, WLength::Auto);

    mRefreshButton = new WPushButton("Refresh Available Results");
    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mTreeView, 0, 0);
    layout->addWidget(mRefreshButton, 1, 0);
    layout->setRowStretch(0, 1);
    setLayout(layout);

    mRefreshButton->clicked().connect(SLOT(this, ResultsBrowser::refreshResults));

    resetAll();
}

///
//  Destructor
//
ResultsBrowser::~ResultsBrowser()
{
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

    //mTreeView->expandToDepth(4); // Causing crash on reload

    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO RESULTS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
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
    WStandardItemModel *model = ConfigXML::getPtr()->getResultsPipelineTree(mPipelineName);
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
    if (!data.empty())
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
    //mTreeView->expandToDepth(4);


    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO Results FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
    }
}

