//
//
//  Description:
//      This is the widget that controls and lays out the entire "Monitor Cluster" page.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef MONITORPAGE_H
#define MONITORPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <string>

namespace Wt
{
    class WPushButton;
    class WStackedWidget;
}
class ClusterJobBrowser;
class MRIBrowser;
class PatientInfoBox;
class MonitorLogTab;
class MonitorResultsTab;

using namespace Wt;


///
/// \class MonitorPage
/// \brief Container widget for the contents of the "Monitor Cluster" tab
///
class MonitorPage : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    MonitorPage(const MRIBrowser* mriBrowser, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MonitorPage();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    ///  Start updating logs
    ///
    void startUpdate();

    ///
    ///  Stop updating logs
    ///
    void stopUpdate();

private:

    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

    ///
    /// Handle job selection changes [slot]
    ///
    void jobSelectedChanged(std::string jobSelectedFile);


private:

    /// MRI Browser pointer
    const MRIBrowser* mMRIBrowser;

    /// Cluster job browser
    ClusterJobBrowser *mClusterJobBrowser;

    /// Monitor log tab
    MonitorLogTab *mMonitorLogTab;

    /// Monitor results tab
    MonitorResultsTab *mMonitorResultsTab;

    /// Patient info box
    PatientInfoBox *mPatientInfoBox;

};

#endif // MONITORPAGE_H

