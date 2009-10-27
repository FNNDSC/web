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
#include "LogFileBrowser.h"
#include "ConfigOptions.h"
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
#include <fstream>
#include <iostream>
#include <string>

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
LogFileBrowser::LogFileBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    mLogTreeView = new WTreeView();
    mLogModel = new WStandardItemModel();

    mLogTreeView->setAttributeValue
            ("oncontextmenu",
             "event.cancelBubble = true; event.returnValue = false; return false;");
    mLogTreeView->setModel(mLogModel);
    mLogTreeView->resize(350, WLength::Auto);
    mLogTreeView->setSelectionMode(SingleSelection);
    mLogTreeView->expandToDepth(1);
    mLogTreeView->selectionChanged().connect(SLOT(this, LogFileBrowser::logChanged));

    mRefreshButton = new WPushButton("Refresh Available Logs");
    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mLogTreeView, 0, 0);
    layout->addWidget(mRefreshButton, 1, 0);
    layout->setRowStretch(0, 1);
    setLayout(layout);

    mRefreshButton->clicked().connect(SLOT(this, LogFileBrowser::refreshLogs));

    resetAll();
}

///
//  Destructor
//
LogFileBrowser::~LogFileBrowser()
{
    delete mLogModel;
}

///
//  Reset to default state
//
void LogFileBrowser::resetAll()
{
    WModelIndexSet noSelection;
    mLogTreeView->setSelectedIndexes(noSelection);
    mLogModel->clear();

    populateLogs(mBaseLogDir, mPostProcDir);

    if (mLogModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO LOGS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mLogModel->appendRow(newItem);
    }
}

///
//  Set the log base directory
//
void LogFileBrowser::setLogBaseDir(const std::string& baseDir)
{
    mBaseLogDir = baseDir;
}

///
//  Set the log base directory
//
void LogFileBrowser::setPostProcDir(const std::string& postProcDir)
{
    mPostProcDir = postProcDir;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Populate the log job model by parsing the directories for log files
//
bool LogFileBrowser::populateLogs(const std::string& baseLogDir, const std::string& postProcDir)
{
    mLogFileEntries.clear();
    addLogEntriesFromDir(path(baseLogDir), true);
    addLogEntriesFromDir(path(postProcDir), false);

    //
    //  The code below creates a tree such as follows:
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
    //  creating this tree structure from the folders that were
    //  found.

    // Create folder entries
    for (int i = 0; i < mLogFileEntries.size(); i++)
    {
        const LogFileEntry *logFileEntry = &mLogFileEntries[i];

        // If it's not a root directory, add the unique folder names as
        // root folder entries
        if (!logFileEntry->mRootDir)
        {
            bool addEntry = true;

            int modelRow;

            // Iterate over all the rows in the root
            for (modelRow = 0; modelRow < mLogModel->rowCount(); modelRow++)
            {
                WStandardItem *item = mLogModel->item(modelRow);
                bool match = true;

                // For the depth of the folder, attempt to match as many folders
                // as possible.
                for (int depth = 0; depth <= logFileEntry->mDepth && item != NULL; depth++)
                {
                    path logDirPath = path(logFileEntry->mBaseLogDir);
                    for (int d = 0; d < (logFileEntry->mDepth - depth); d++)
                    {
                        logDirPath = logDirPath.branch_path();
                    }
                    std::string folderLeafName = path(logDirPath).leaf();

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
                WStandardItem *lastItem = mLogModel->invisibleRootItem();

                for (int depth = 0; depth <= logFileEntry->mDepth; depth++)
                {
                    path logDirPath = path(logFileEntry->mBaseLogDir);
                    for (int d = 0; d < (logFileEntry->mDepth - depth); d++)
                    {
                        logDirPath = logDirPath.branch_path();
                    }
                    std::string folderLeafName = path(logDirPath).leaf();

                    bool addFolder = true;
                    for (int row = 0; row < lastItem->rowCount(); row++)
                    {
                        WStandardItem *item = lastItem->child(row);
                        std::string folderLeafName = path(logDirPath).leaf();

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
            mLogModel->appendRow(createLogEntry(logFileEntry));
        }
    }


    // Now add each of the items under the folders
    for (int i = 0; i < mLogFileEntries.size(); i++)
    {
        const LogFileEntry *logFileEntry = &mLogFileEntries[i];

        if (!logFileEntry->mRootDir)
        {
            WStandardItem *lastItem = mLogModel->invisibleRootItem();

            for (int depth = 0; depth <= logFileEntry->mDepth; depth++)
            {
                path logDirPath = path(logFileEntry->mBaseLogDir);
                for (int d = 0; d < (logFileEntry->mDepth - depth); d++)
                {
                    logDirPath = logDirPath.branch_path();
                }
                std::string folderLeafName = path(logDirPath).leaf();

                for (int row = 0; row < lastItem->rowCount(); row++)
                {
                    WStandardItem *item = lastItem->child(row);
                    std::string folderLeafName = path(logDirPath).leaf();

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
                lastItem->appendRow(createLogEntry(logFileEntry));
            }
        }
    }

    return true;
}

///
//  Add log files from a base directory
//
void LogFileBrowser::addLogEntriesFromDir(const path& logDir, bool rootDir, int depth)
{
    if (!exists(logDir))
    {
        return;
    }

    directory_iterator endItr;
    for ( directory_iterator itr(logDir);
          itr != endItr;
          ++itr)
    {
        if (is_directory(itr->status()))
        {
            addLogEntriesFromDir(itr->path(), rootDir, depth + 1);
        }
        else
        {
            bool stdOutFile = false,
                 stdErrFile = false;
            std::string baseLogName;

            std::string fileExt = extension(itr->path());

            if (fileExt == ".std")
            {
                stdOutFile = true;
            }
            else if (fileExt == ".err")
            {
                stdErrFile = true;
            }

            if (stdOutFile || stdErrFile)
            {
                std::string baseLogName = itr->path().leaf();
                std::string baseLogDir = itr->path().branch_path().string();

                baseLogName.erase(baseLogName.length() - 4); // Strip the last extension

                // See if it is already in the list
                bool addNewEntry = true;
                for (int i = 0; i < mLogFileEntries.size(); i++)
                {
                    LogFileEntry* logEntry = &mLogFileEntries[i];

                    if (logEntry->mBaseLogName == baseLogName &&
                        logEntry->mBaseLogDir == baseLogDir)
                    {
                        addNewEntry = false;

                        if (stdOutFile)
                        {
                            logEntry->mHasStdOut = true;
                        }
                        else
                        {
                            logEntry->mHasStdErr = true;
                        }
                        break;
                    }
                }

                if (addNewEntry)
                {
                    LogFileEntry newEntry;

                    newEntry.mHasStdOut = stdOutFile;
                    newEntry.mHasStdErr = stdErrFile;
                    newEntry.mBaseLogName = baseLogName;
                    newEntry.mBaseLogDir = baseLogDir;
                    newEntry.mRootDir = rootDir;
                    newEntry.mDepth = depth;

                    // Determine the root directory
                    path rootLogPath = path(baseLogDir);
                    while (depth > 0)
                    {
                        rootLogPath = rootLogPath.branch_path();
                        depth--;
                    }
                    newEntry.mRootLogDir = rootLogPath.string();

                    mLogFileEntries.push_back(newEntry);
                }
            }
        }
    }
}

///
//  Create a log entry
//
WStandardItem *LogFileBrowser::createLogEntry(const LogFileEntry* logEntry)
{
    WStandardItem *result = new WStandardItem(logEntry->mBaseLogName);

    result->setIcon("icons/file.gif");

    // Set the data for the log entry in the user roles
    result->setData(*logEntry, UserRole);

    return result;
}

///
//  Log selection changed by user
//
void LogFileBrowser::logChanged()
{
    if (mLogTreeView->selectedIndexes().empty())
        return;

    WModelIndex selected = *mLogTreeView->selectedIndexes().begin();
    boost::any logEntryData = selected.data(UserRole);

    if (!logEntryData.empty())
    {
        LogFileEntry logFileEntry = boost::any_cast<LogFileEntry>(logEntryData);

        mLogFileSelected.emit(logFileEntry);
    }
}

///
//  Handle refresh log button clicked [slot]
//
void LogFileBrowser::refreshLogs()
{
    mLogModel->clear();
    populateLogs(mBaseLogDir, mPostProcDir);

    if (mLogModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO LOGS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mLogModel->appendRow(newItem);
    }
}

