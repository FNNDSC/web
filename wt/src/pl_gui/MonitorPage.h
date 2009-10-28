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
#include "LogFileBrowser.h"

namespace Wt
{
    class WPushButton;
    class WStackedWidget;
}
class ClusterJobBrowser;
class LogFileTailer;
class MRIBrowser;

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

    ///
    ///  Handle log selection changes [slot]
    ///
    void logSelectedChanged(LogFileBrowser::LogFileEntry logFileEntry);


private:

    /// MRI Browser pointer
    const MRIBrowser* mMRIBrowser;

    /// Cluster job browser
    ClusterJobBrowser *mClusterJobBrowser;

    /// Stdout log
    LogFileTailer *mLogStdOut;

    /// Stderr log
    LogFileTailer *mLogStdErr;

    /// Log file browser
    LogFileBrowser *mLogFileBrowser;


};

#endif // MONITORPAGE_H

