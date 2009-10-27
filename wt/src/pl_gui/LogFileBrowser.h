//
//
//  Description:
//      Definition of Log File browser widget.  This widget provides
//      a browser for all of the log files for a cluster run.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef LOGFILEBROWSER_H
#define LOGFILEBROWSER_H

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>
#include <Wt/WStandardItemModel>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>

using namespace Wt;

namespace Wt
{
    class WPushButton;
}

///
/// \class LogFileBrowser
/// \brief Provides a browser for all the log files of a cluster job
///
class LogFileBrowser : public WContainerWidget
{
public:

    /// Log file entry structure
    typedef struct
    {
        /// Located in root log directory
        bool mRootDir;

        /// Has standard out file
        bool mHasStdOut;

        /// Has standard err file
        bool mHasStdErr;

        /// Basename for log
        std::string mBaseLogName;

        /// Path to log directory
        std::string mBaseLogDir;

        /// Root director of log (as opposed to base, which points
        /// to the actual base directory).  This is the location of
        /// the root folder given sub-folders are present
        std::string mRootLogDir;

        /// Depth of folder from mRootLogDir -> mBaseLogDir
        int mDepth;

    } LogFileEntry;

    ///
    /// Constructor
    ///
    LogFileBrowser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~LogFileBrowser();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    ///  Set the log base directory
    ///
    void setLogBaseDir(const std::string& baseDir);

    ///
    ///  Set the log base directory
    ///
    void setPostProcDir(const std::string& postProcDir);

    ///
    /// Signal accessor for log file selection
    ///
    Wt::Signal<LogFileEntry>& logFileSelected() { return mLogFileSelected; }

private:

    ///
    ///  Log file selection changed by user
    ///
    void logChanged();

    ///
    ///  Populate the log job model by parsing the directories for log files
    ///
    bool populateLogs(const std::string& baseLogDir, const std::string& postProcDir);

    ///
    ///  Add log files from a base directory
    ///
    void addLogEntriesFromDir(const boost::filesystem::path& logDir, bool rootDir, int depth = -1);

    ///
    ///  Create a log entry
    ///
    WStandardItem* createLogEntry(const LogFileEntry* logEntry);


    ///
    ///  Handle refresh log button clicked [slot]
    ///
    void refreshLogs();

private:

    /// Log file vector
    std::vector<LogFileEntry> mLogFileEntries;

    /// Signal for when a log file is selected
    Wt::Signal<LogFileEntry> mLogFileSelected;

    /// MRID Tree view
    WTreeView *mLogTreeView;

    /// MRID Model
    WStandardItemModel *mLogModel;

    /// Refersh button
    WPushButton *mRefreshButton;

    /// Log base directory
    std::string mBaseLogDir;

    /// Post proc directory
    std::string mPostProcDir;

};

#endif // LOGFILEBROWSER_H

