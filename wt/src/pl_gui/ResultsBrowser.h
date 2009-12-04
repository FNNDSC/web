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

class ArchiveFileResource;

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
    /// Signal accessor for result file selection
    ///
    Wt::Signal<std::string>& resultFileSelected() { return mResultFileSelected; }

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
    std::vector<std::string> mResultFileEntries;

    /// Signal for when a result file is selected
    Wt::Signal<std::string> mResultFileSelected;

    /// Refersh button
    WPushButton *mRefreshButton;

    /// Results base directory
    std::string mResultsBaseDir;

    /// Pipeline name
    std::string mPipelineName;

    /// Archive file resource
    ArchiveFileResource *mArchiveFileResource;

};

#endif // LOGFILEBROWSER_H

