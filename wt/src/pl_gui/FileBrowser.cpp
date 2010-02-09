//
//
//  Description:
//      Implementation of base class for File browser widget.  This widget provides
//      common functionality that can be inherited to browse a tree of files.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "FileBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WLogger>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "QtFileSystemWatcher.h"
#include <QStringList>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
FileBrowser::FileBrowser(WContainerWidget *parent) :
    WContainerWidget(parent),
    mQtFileSystemWatcher(NULL),
    mStopUpdateThread(false),
    mUpdateBrowser(false),
    mApp(WApplication::instance())
{
    mTreeView = new WTreeView();
    mModel = new WStandardItemModel(this);

    mTreeView->setAttributeValue
            ("oncontextmenu",
             "event.cancelBubble = true; event.returnValue = false; return false;");
    mTreeView->setModel(mModel);
    mTreeView->setSelectionMode(SingleSelection);
    mTreeView->setHeaderHeight(0);

    mThread = new boost::thread(boost::bind(&FileBrowser::updateBrowser, this));
}

///
//  Destructor
//
FileBrowser::~FileBrowser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
/// Reset to default state
///
void FileBrowser::resetAll()
{
    if (mQtFileSystemWatcher != NULL)
    {
        QStringList dirList = mQtFileSystemWatcher->directories();

        if (dirList.size() > 0)
        {
            mQtFileSystemWatcher->removePaths(dirList);
        }
    }
}

///
//  Do an update on the log
//
void FileBrowser::doUpdate()
{
    boost::mutex::scoped_lock lock(mDoUpdateMutex);
    mUpdateBrowser = true;
    mDoUpdateCondition.notify_one();
}

///
// Finalize the widget (pre-destruction)
//
void FileBrowser::finalize()
{
    if (mThread != NULL)
    {
        mStopUpdateThread = true;
        mUpdateBrowser = false;
        mDoUpdateCondition.notify_one();
        mThread->join();
        delete mThread;
        mThread = NULL;
    }
}

///
//  Create Qt objects
//
void FileBrowser::createQt()
{
    mQtFileSystemWatcher = new QtFileSystemWatcher(this);
}

///
//  Destroy Qt objects()
//
void FileBrowser::destroyQt()
{
    delete mQtFileSystemWatcher;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Add directory to be watched
//
void FileBrowser::addWatchPath(const std::string& path)
{
    if (mQtFileSystemWatcher != NULL)
    {
        QStringList dirList = mQtFileSystemWatcher->directories();
        if (!dirList.contains(path.c_str()))
        {
            WApplication::instance()->log("debug") << "Watching: " << path;
            mQtFileSystemWatcher->addPath(path.c_str());
        }
    }
}

///
//  Add an entry to the browser
//
bool FileBrowser::addEntry(bool rootDir, int entryDepth,
                           const std::string &baseDir,
                           const std::string &baseName,
                           int index)
{
    //
    //  The code below adds an entry into the tree such as follows:
    //
    //  + tract_meta.bash
    //  + dicom_seriesCollect.bash
    //  + tract_meta-stage-2-dcm2trk.bash
    //    |
    //    + stage-1-mri_convert
    //      |
    //      + diff_unpack
    //
    //   ...
    //
    //  The relatively convoluted logic below is responsible for
    //  adding a new item to this tree structure.

    // If it's not a root directory, add the unique folder names as
    // root folder entries
    if (!rootDir)
    {
        bool addEntry = true;

        int modelRow;

        // Iterate over all the rows in the root
        for (modelRow = 0; modelRow < mModel->rowCount(); modelRow++)
        {
            WStandardItem *item = mModel->item(modelRow);
            bool match = true;

            // For the depth of the folder, attempt to match as many folders
            // as possible.
            for (int depth = 0; depth <= entryDepth && item != NULL; depth++)
            {
                path dirPath = path(baseDir);
                for (int d = 0; d < (entryDepth - depth); d++)
                {
                    dirPath = dirPath.branch_path();
                }
                std::string folderLeafName = path(dirPath).leaf();

                boost::any displayData = item->data(DisplayRole);
                if (!displayData.empty())
                {
                    WString folderName = boost::any_cast<WString>(displayData);

                    // Folder did not match, this means we need to add it
                    // to the tree
                    if (folderName.toUTF8() != folderLeafName)
                    {
                        match = false;
                        break;
                    }
                }
            }

            // All folders matched, we do not need a new folder
            if (match)
            {
                addEntry = false;
            }
        }

        // Add all of the necessary folder entries to the tree
        if (addEntry)
        {
            WStandardItem *lastItem = mModel->invisibleRootItem();

            for (int depth = 0; depth <= entryDepth; depth++)
            {
                path dirPath = path(baseDir);
                for (int d = 0; d < (entryDepth - depth); d++)
                {
                    dirPath = dirPath.branch_path();
                }
                std::string folderLeafName = path(dirPath).leaf();

                bool addFolder = true;
                for (int row = 0; row < lastItem->rowCount(); row++)
                {
                    WStandardItem *item = lastItem->child(row);
                    std::string folderLeafName = path(dirPath).leaf();

                    boost::any displayData = item->data(DisplayRole);
                    if (!displayData.empty())
                    {
                        WString folderName = boost::any_cast<WString>(displayData);

                        if (folderName.toUTF8() == folderLeafName)
                        {
                           addFolder = false;
                           lastItem = item;
                           break;
                        }
                    }
                }

                if (addFolder)
                {
                    WStandardItem *newItem = new WStandardItem(folderLeafName);
                    newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
                    newItem->setIcon("icons/folder.gif");
                    lastItem->appendRow(newItem);
                    lastItem = newItem;
                }
            }
        }
    }
    // For root entries, add the file logs
    else
    {
        mModel->appendRow(createEntry(baseName, index));
    }

    // Now add the items under the folders
    if (!rootDir)
    {
        WStandardItem *lastItem = mModel->invisibleRootItem();

        for (int depth = 0; depth <= entryDepth; depth++)
        {
            path dirPath = path(baseDir);
            for (int d = 0; d < (entryDepth - depth); d++)
            {
                dirPath = dirPath.branch_path();
            }
            std::string folderLeafName = path(dirPath).leaf();

            for (int row = 0; row < lastItem->rowCount(); row++)
            {
                WStandardItem *item = lastItem->child(row);
                std::string folderLeafName = path(dirPath).leaf();

                boost::any displayData = item->data(DisplayRole);
                if (!displayData.empty())
                {
                    WString folderName = boost::any_cast<WString>(displayData);

                    if (folderName.toUTF8() == folderLeafName)
                    {
                        lastItem = item;
                        break;
                    }
                }
            }
        }

        if (lastItem != NULL)
        {
            lastItem->appendRow(createEntry(baseName, index));
        }
    }

    // Add the directory to the watch list
    addWatchPath(baseDir);

    return true;
}


///
//  Create a file entry
//
WStandardItem* FileBrowser::createEntry(const std::string& baseName, int index)
{
    WStandardItem *result = new WStandardItem(baseName);

    result->setIcon("icons/file.gif");

    // Set the data for the entry index in the user roles
    result->setData(index, UserRole);

    return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Update thread for checking directory changes
//
void FileBrowser::updateBrowser()
{
    boost::mutex::scoped_lock lock(mDoUpdateMutex);

    while (!mStopUpdateThread)
    {
        mDoUpdateCondition.wait(lock);

        if (mUpdateBrowser)
        {

            // First, take the lock to safely manipulate the UI outside of the
            // normal event loop, by having exclusive access to the session.
            WApplication::UpdateLock lock = mApp->getUpdateLock();

            directoryChanged();

            mApp->triggerUpdate();
        }
    }
}
