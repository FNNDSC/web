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

#include "FileBrowser.h"
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
class LogFileBrowser : public FileBrowser
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
    /// Reset to the default state
    ///
    virtual void resetAll();

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

    ///
    /// Handle async changes to directory
    ///
    virtual void directoryChanged();

protected:

    ///
    ///  Populate the browser model by parsing the directories for log files
    ///
    void populateBrowser();

    ///
    ///  Log file selection changed by user
    ///
    void logChanged();

    ///
    ///  Add log files from a base directory
    ///
    void addLogEntriesFromDir(const boost::filesystem::path& logDir, bool rootDir, int depth = -1);

    ///
    ///  Handle refresh log button clicked [slot]
    ///
    void refreshLogs();

private:

    /// Log file vector
    std::vector<LogFileEntry> mLogFileEntries;

    /// Signal for when a log file is selected
    Wt::Signal<LogFileEntry> mLogFileSelected;

    /// Refersh button
    WPushButton *mRefreshButton;

    /// Log base directory
    std::string mBaseLogDir;

    /// Post proc directory
    std::string mPostProcDir;

};

#endif // LOGFILEBROWSER_H

