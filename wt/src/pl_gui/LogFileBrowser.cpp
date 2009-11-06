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
    FileBrowser(parent)
{
    mTreeView->selectionChanged().connect(SLOT(this, LogFileBrowser::logChanged));
    mTreeView->resize(350, WLength::Auto);

    mRefreshButton = new WPushButton("Refresh Available Logs");
    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mTreeView, 0, 0);
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
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default state
//
void LogFileBrowser::resetAll()
{
    WModelIndexSet noSelection;
    mTreeView->setSelectedIndexes(noSelection);
    mModel->clear();

    populateBrowser();
    mTreeView->expandToDepth(3);

    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO FILES FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
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
//  Protected Members
//
//

///
//  Populate the log job model by parsing the directories for log files
//
void LogFileBrowser::populateBrowser()
{
    mLogFileEntries.clear();
    addLogEntriesFromDir(path(mBaseLogDir), true);
    addLogEntriesFromDir(path(mPostProcDir), false);

    // Add the entries to the browser
    for(int i = 0; i < mLogFileEntries.size(); i++)
    {
        const LogFileEntry *logFileEntry = &mLogFileEntries[i];

        addEntry(logFileEntry->mRootDir, logFileEntry->mDepth,
                 logFileEntry->mBaseLogDir, logFileEntry->mBaseLogName,
                 i);
    }
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
//  Log selection changed by user
//
void LogFileBrowser::logChanged()
{
    if (mTreeView->selectedIndexes().empty())
        return;

    WModelIndex selected = *mTreeView->selectedIndexes().begin();
    boost::any logEntryDataIndex = selected.data(UserRole);

    if (!logEntryDataIndex.empty())
    {
        int logFileEntryIndex = boost::any_cast<int>(logEntryDataIndex);

        mLogFileSelected.emit(mLogFileEntries[logFileEntryIndex]);
    }
}

///
//  Handle refresh log button clicked [slot]
//
void LogFileBrowser::refreshLogs()
{
    mModel->clear();
    populateBrowser();
    mTreeView->expandToDepth(4);


    if (mModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO LOGS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mModel->appendRow(newItem);
    }
}

