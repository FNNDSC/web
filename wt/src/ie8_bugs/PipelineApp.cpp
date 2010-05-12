//
//
//  Description:
//      Reproducer app for IE8 bugs in Wt
//
//  Author:
//      Dan Ginsburg
//
//
#include "PipelineApp.h"
#include "SubjectPage.h"
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WLogger>
#include <Wt/WPushButton>

///
//  Namespaces
//
using namespace Wt;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
PipelineApp::PipelineApp(const WEnvironment &env) :
    WApplication(env)
{
    // Use XML resource bundle containing text
    messageResourceBundle().use("text");

    createUI();

    enableUpdates(true);
}

///
//  Destructor
//
PipelineApp::~PipelineApp()
{
}

///
//  Create the user interface widgets
//
void PipelineApp::createUI()
{
    WApplication::instance()->log("debug") << "createUI: START";
    WContainerWidget *w = root();
    w->setStyleClass("maindiv");

    // Create the top tab
    WTabWidget *topTab = new WTabWidget();
    topTab->setStyleClass("toptabdiv");
    mSubjectPage = new SubjectPage();
    //mResultsPage = new ResultsPage(mSubjectPage->getMRIBrowser());
    //mMonitorPage = new MonitorPage(mSubjectPage->getMRIBrowser());
    topTab->addTab(mSubjectPage, "Subjects");
    //topTab->addTab(mResultsPage, "Results");
    //topTab->addTab(mMonitorPage, "Monitor Cluster");
    //topTab->addTab(new WText(w->tr("cluster-load-page"), XHTMLUnsafeText), "Cluster Load");
    //topTab->currentChanged().connect(this, &PipelineApp::mainTabChanged);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(topTab, 0, 0);
    layout->addWidget(new WLabel(w->tr("file-bug")), 2, 0, Wt::AlignCenter);
    layout->setRowStretch(0, 1);

    mMainSiteWidget = new WContainerWidget();
    //mProjectPage = new ProjectPage();


    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    mSubjectPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));
    //mResultsPage->resize(WLength(100.0, WLength::Percentage),
   //                      WLength(100.0, WLength::Percentage));
    //mMonitorPage->resize(WLength(100.0, WLength::Percentage),
      //                   WLength(100.0, WLength::Percentage));

    mMainSiteWidget->setLayout(layout);

    //mLoginPage = new LoginPage();
    //mLoginPage->userLoggedIn().connect(this, &PipelineApp::userLoggedIn);
    //mProjectPage->projectChosen().connect(this, &PipelineApp::projectChosen);
    //mProjectPage->logoutUser().connect(this, &PipelineApp::logout);
    //changeProjectButton->clicked().connect(this, &PipelineApp::goHome);
    //logoutButton->clicked().connect(this, &PipelineApp::logout);

    //mStackedWidget = new WStackedWidget();
    //mStackedWidget->addWidget(mLoginPage);
    //mStackedWidget->addWidget(mainSite);
    //mStackedWidget->setCurrentIndex(0);

    WVBoxLayout *primaryLayout = new WVBoxLayout();
    primaryLayout->addWidget(mMainSiteWidget);

    w->setLayout(primaryLayout);

    requestTooLarge().connect(this, &PipelineApp::largeRequest);

    setTitle(w->tr("page-title"));

    WApplication::instance()->log("debug") << "createUI: FINISH";

    // See if the user is already logged in via a cookie
//    if (mLoginPage->getLoggedIn())
   // {
     //   userLoggedIn(mLoginPage->getCurrentUserName(), mLoginPage->getCurrentEmail());
   // }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
// Handle too large a request being sent
//
void PipelineApp::largeRequest(int size)
{
    this->log("notice") << "Request too large: " << size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functions
//
//

///
//  Create the application
//
WApplication *createApplication(const WEnvironment& env)
{
    PipelineApp *app = new PipelineApp(env);
    app->setTwoPhaseRenderingThreshold(0);
    app->useStyleSheet("styles.css");
    app->refresh();

    // Set the loading indicator to be an overlay box
    app->setLoadingIndicator(new WOverlayLoadingIndicator());

    return app;
}

///
//  Main entrypoint
//
int main(int argc, char **argv)
{
    return WRun(argc, argv, &createApplication);
}

