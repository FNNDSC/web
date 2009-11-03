//
//
//  Description:
//      Definition of results browser widget.  This widget provides
//      a browser for all of the results files for a cluster run.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef RESULTSBROWSER_H
#define RESULTSBROWSER_H

#include "FileBrowser.h"
#include <vector>

using namespace Wt;

namespace Wt
{
    class WPushButton;
    class WStandardItem;
}

///
/// \class ResultsBrowser
/// \brief Provides a browser for filtered results of a cluster job
///
class ResultsBrowser : public FileBrowser
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

    } ResultFileEntry;

    ///
    /// Constructor
    ///
    ResultsBrowser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ResultsBrowser();

    ///
    /// Reset to the default state
    ///
    void resetAll();

    ///
    ///  Set the base directory for the results
    ///
    void setResultsBaseDir(const std::string& baseDir);

    ///
    /// Set the name of the pipeline
    ///
    void setPipelineName(const std::string& pipelineName);

    ///
    /// Signal accessor for log file selection
    ///
    Wt::Signal<ResultFileEntry>& resultFileSelected() { return mResultFileSelected; }

protected:

    ///
    ///  Populate the browser model by parsing the directories for log files
    ///
    void populateBrowser();

    ///
    ///  Result file selection changed by user
    ///
    void resultChanged();

    ///
    ///  Add files from tree
    ///
    void addFilesFromTree(WStandardItem *item, const std::string& baseDir,
                          int depth, int &index);

    ///
    ///  Handle refresh results button clicked [slot]
    ///
    void refreshResults();

private:

    /// Log file vector
    std::vector<ResultFileEntry> mResultFileEntries;

    /// Signal for when a result file is selected
    Wt::Signal<ResultFileEntry> mResultFileSelected;

    /// Refersh button
    WPushButton *mRefreshButton;

    /// Results base directory
    std::string mResultsBaseDir;

    /// Pipeline name
    std::string mPipelineName;

};

#endif // LOGFILEBROWSER_H

