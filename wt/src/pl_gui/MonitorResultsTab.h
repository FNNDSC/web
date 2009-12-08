//
//
//  Description:
//      This is the widget that contains the results browser
//      chosen cluster job.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef MONITORRESULTSTAB_H
#define MONITORRESULTSTAB_H

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <string>

class MRIBrowser;
class ResultsBrowser;
class FilePreviewBox;

namespace Wt
{
    class WLabel;
}
using namespace Wt;

///
/// \class MonitorResultsTab
/// \brief Container widget for the contents of the "Results" tab of the
///        "Monitor Cluster" page
///
class MonitorResultsTab : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    MonitorResultsTab(const MRIBrowser* mriBrowser, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MonitorResultsTab();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Handle job selection changes
    ///
    void jobSelectedChanged(std::string jobSelectedFile);

private:

    ///
    ///  Handle result file selection changes [slot]
    ///
    void resultFileChanged(std::string resultFilePath);

private:

    /// MRI Browser pointer
    const MRIBrowser* mMRIBrowser;

    /// Pipeline Label
    WLabel *mPipelineLabel;

    /// Results browser
    ResultsBrowser *mResultsBrowser;

    /// File info box
    FilePreviewBox *mFilePreviewBox;
};

#endif // MONITORRESULTSTAB_H

