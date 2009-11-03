//
//
//  Description:
//      This is the widget that contains the log file browser for a
//      chosen cluster job.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef MONITORLOGTAB_H
#define MONITORLOGTAB_H

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
class PatientInfoBox;

using namespace Wt;


///
/// \class MonitorLogTab
/// \brief Container widget for the contents of the "Logs" tab of the
///        "Monitor Cluster" page
///
class MonitorLogTab : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    MonitorLogTab(const MRIBrowser* mriBrowser, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MonitorLogTab();

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

    ///
    /// Handle job selection changes
    ///
    void jobSelectedChanged(std::string jobSelectedFile);

private:

    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);



    ///
    ///  Handle log selection changes [slot]
    ///
    void logSelectedChanged(LogFileBrowser::LogFileEntry logFileEntry);


private:

    /// MRI Browser pointer
    const MRIBrowser* mMRIBrowser;

    /// Stdout log
    LogFileTailer *mLogStdOut;

    /// Stderr log
    LogFileTailer *mLogStdErr;

    /// Log file browser
    LogFileBrowser *mLogFileBrowser;

};

#endif // MONITORLOGTAB_H

