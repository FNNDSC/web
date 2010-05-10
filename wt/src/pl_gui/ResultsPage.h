//
//
//  Description:
//      This is the widget that controls and lays out the entire "Results" page.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef RESULTSPAGE_H
#define RESULTSPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <string>

namespace Wt
{
    class WPushButton;
    class WStackedWidget;
    class WComboBox;
    class WLineEdit;
    class WPushButton;
    class WStackedWidget;
}
class ResultsTable;
class MonitorLogTab;
class MonitorResultsTab;
class MRIBrowser;
class PipelineArgTable;
class JobStatus;

using namespace Wt;


///
/// \class ResultsPage
/// \brief Container widget for the contents of the "Results" tab
///
class ResultsPage : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ResultsPage(const MRIBrowser* mriBrowser, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ResultsPage();

    ///
    ///  Create Qt objects
    ///
    void createQt();

    ///
    ///  Destroy Qt objects
    ///
    void destroyQt();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Reset to the current user name
    ///
    void resetUser(const std::string &userName);

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
    /// Called when user changed [slot]
    ///
    void userChanged(int);

    ///
    /// Called when search button pushed [slot]
    ///
    void searchPushed();

    ///
    /// Called when clear button pushed [slot]
    ///
    void clearPushed();

    ///
    /// Called when back button pushed [slot]
    ///
    void backPushed();


    ///
    /// New result selected (double click) [slot]
    ///
    void resultSelected(std::string clusterShFile, std::string metaScript, std::string arguments);

    ///
    /// New result clicked (single click) [slot]
    ///
    void resultClicked(std::string clusterShFile, std::string metaScript, std::string arguments,
                       std::string jobID, std::string userName);

    ///
    /// Refresh clicked [slot]
    ///
    void refreshClicked();

    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

private:

    /// MRI Browser
    const MRIBrowser* mMRIBrowser;

    /// Results table
    ResultsTable *mResultsTable;

    /// Filter results by user combo box
    WComboBox *mUserComboBox;

    /// Search line edit
    WLineEdit *mSearchLineEdit;

    /// Search push button
    WPushButton *mSearchPushButton;

    /// Back button
    WPushButton *mBackButton;

    /// Monitor log tab
    MonitorLogTab *mMonitorLogTab;

    /// Monitor results tab
    MonitorResultsTab *mMonitorResultsTab;

    /// Stacked widget
    WStackedWidget *mStackedWidget;

    /// Pipeline arguments table
    PipelineArgTable *mPipelineArgTable;

    /// Job Status
    JobStatus *mJobStatus;
};

#endif // RESULTSPAGE_H

