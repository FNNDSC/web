//
//
//  Description:
//      Implementation of the main application class for the pipeline GUI.  It has the
//      startup logic and has the container widgets that constitute the entirety
//      of the application.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "SubjectPage.h"
#include "MonitorPage.h"
#include "ResultsPage.h"
#include "ResultsPage.h"
#include "ProjectPage.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include "LoginPage.h"
#include "MRIBrowser.h"
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
#include <QCoreApplication>
#include <boost/filesystem.hpp>

///
//  Namespaces
//
using namespace Wt;
using namespace boost;
using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Functions
//
//


///
//  Return the ConfigXML pointer from the application instance.  Each
//  session has its own WApplication instance.
//
ConfigXML* getConfigXMLPtr()
{
    PipelineApp* instance = dynamic_cast<PipelineApp*>(WApplication::instance());

    if (instance != NULL)
    {
        return instance->getConfigXML();
    }
}



///
/// Return the ConfigOptions pointer from the application instance.  Each
/// session has its own WApplication instance.
///
///     THREAD-SAFE version of function
///
ConfigOptions* getConfigOptionsPtrTS(WApplication *appInstance)
{
    PipelineApp* instance = dynamic_cast<PipelineApp*>(appInstance);

    if (instance != NULL)
    {
        return instance->getConfigOptions();
    }
}

///
//  Return the ConfigOptions pointer from the application instance.  Each
//  session has its own WApplication instance.
//
ConfigOptions* getConfigOptionsPtr()
{
    return getConfigOptionsPtrTS(WApplication::instance());
}

///
//  Return the current logged in user name
//
const std::string getCurrentUserName()
{
    PipelineApp* instance = dynamic_cast<PipelineApp*>(WApplication::instance());

    if (instance != NULL)
    {
        return instance->getCurrentUser();
    }

    return std::string();
}

///
/// Return the current E-mail address
///
const std::string getCurrentUserEmail()
{
    PipelineApp* instance = dynamic_cast<PipelineApp*>(WApplication::instance());

    if (instance != NULL)
    {
        return instance->getCurrentEmail();
    }

    return std::string();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
PipelineApp::PipelineApp(const WEnvironment &env) :
    WQApplication(env, true)
{
    /*
     * Note: do not create any Qt objects from here. Initialize your
     * application from within the virtual create() method.
     */
    mConfigOptions = new ConfigOptions();
    mConfigXML = new ConfigXML();

    // Start by loading configuration options
    if(!mConfigOptions->LoadFromFile("conf/pl_gui.conf"))
    {
       this->log("error") << "Loading configuration file 'conf/pl_gui.conf'";
    }

    if(!mConfigXML->loadFromFile(mConfigOptions->GetConfigXML()))
    {
       this->log("error") << "Loading results configuration XML file" << getConfigOptionsPtr()->GetConfigXML();
    }

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
    delete mConfigOptions;
    delete mConfigXML;
}

///
//  Callback for creating Qt objects (see libwtwithqt)
//
void PipelineApp::create()
{
    // Create all Qt Objects
    mSubjectPage->createQt();
    mMonitorPage->createQt();
    mResultsPage->createQt();
}

///
//  Callback for destroying Qt objects (see libwtwithqt)
//
void PipelineApp::destroy()
{
    // Destroy Qt Objects
    mSubjectPage->destroyQt();
    mMonitorPage->destroyQt();
    mResultsPage->destroyQt();

}

///
// Override finalize to handle destruction of threads
//
void PipelineApp::finalize()
{
    mSubjectPage->finalize();
    mMonitorPage->finalize();
    mResultsPage->finalize();
}


///
//  Create the user interface widgets
//
void PipelineApp::createUI()
{
    WContainerWidget *w = root();
    w->setStyleClass("maindiv");

    WGridLayout *layout = new WGridLayout();

    /// Create top container holding logo and title
    WContainerWidget *topContainer = new WContainerWidget();
    WGridLayout *topLayout = new WGridLayout();
    topContainer->setStyleClass("topdiv");
    WImage *chbLogo = new WImage(w->tr("logo-image").toUTF8());
    WImage *brainImage = new WImage("icons/mri.gif");
    chbLogo->setStyleClass("titlediv");
    topLayout->addWidget(chbLogo, 0, 0, Wt::AlignLeft);
    WLabel *titleLabel = new WLabel(w->tr("page-top-text"));
    titleLabel->setStyleClass("titlediv");

    WGridLayout *projectAndBrainLayout = new WGridLayout();
    mCurrentProjectLabel = new WLabel("Current Project: []");
    mCurrentProjectLabel->setStyleClass("projectdiv");
    mCurrentProjectLabel->setWordWrap(false);
    mCurrentUserLabel = new WLabel("");
    mCurrentUserLabel->setStyleClass("projectdiv");
    mCurrentUserLabel->setWordWrap(false);

    WPushButton *changeProjectButton = new WPushButton("Change");
    WPushButton *logoutButton = new WPushButton("Logout");
    projectAndBrainLayout->addWidget(mCurrentProjectLabel, 0, 0, AlignRight | AlignMiddle);
    projectAndBrainLayout->addWidget(changeProjectButton, 0, 1, AlignLeft | AlignMiddle);
    projectAndBrainLayout->addWidget(mCurrentUserLabel, 1, 0, AlignRight | AlignMiddle);
    projectAndBrainLayout->addWidget(logoutButton, 1, 1, AlignRight | AlignMiddle);
    projectAndBrainLayout->setVerticalSpacing(2);

    topLayout->addWidget(titleLabel, 0, 1, Wt::AlignCenter | Wt::AlignMiddle);
    topLayout->addLayout(projectAndBrainLayout, 0, 2, Wt::AlignRight | AlignMiddle);
    topContainer->setLayout(topLayout);

    // Create the top tab
    WTabWidget *topTab = new WTabWidget();
    topTab->setStyleClass("toptabdiv");
    mSubjectPage = new SubjectPage();
    mResultsPage = new ResultsPage(mSubjectPage->getMRIBrowser());
    mMonitorPage = new MonitorPage(mSubjectPage->getMRIBrowser());
    topTab->addTab(mSubjectPage, "Subjects");
    topTab->addTab(mResultsPage, "Results");
    topTab->addTab(mMonitorPage, "Monitor Cluster");
    topTab->addTab(new WText(w->tr("cluster-load-page"), XHTMLUnsafeText), "Cluster Load");
    topTab->currentChanged().connect(this, &PipelineApp::mainTabChanged);

    layout->addWidget(topContainer, 0, 0);
    layout->addWidget(topTab, 1, 0);
    layout->addWidget(new WLabel(w->tr("file-bug")), 2, 0, Wt::AlignCenter);
    layout->setRowStretch(1, 1);

    mMainSiteWidget = new WContainerWidget();
    mProjectPage = new ProjectPage();


    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    mSubjectPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));
    mResultsPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));
    mMonitorPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));

    mMainSiteWidget->setLayout(layout);

    mLoginPage = new LoginPage();
    mLoginPage->userLoggedIn().connect(this, &PipelineApp::userLoggedIn);
    mProjectPage->projectChosen().connect(this, &PipelineApp::projectChosen);
    mProjectPage->logoutUser().connect(this, &PipelineApp::logout);
    changeProjectButton->clicked().connect(this, &PipelineApp::goHome);
    logoutButton->clicked().connect(this, &PipelineApp::logout);

    mStackedWidget = new WStackedWidget();
    mStackedWidget->addWidget(mLoginPage);
    //mStackedWidget->addWidget(mainSite);
    mStackedWidget->setCurrentIndex(0);

    WVBoxLayout *primaryLayout = new WVBoxLayout();
    primaryLayout->addWidget(mStackedWidget);

    w->setLayout(primaryLayout);

    requestTooLarge().connect(this, &PipelineApp::largeRequest);

    setTitle(w->tr("page-title"));

    // See if the user is already logged in via a cookie
    if (mLoginPage->getLoggedIn())
    {
        userLoggedIn(mLoginPage->getCurrentUserName(), mLoginPage->getCurrentEmail());
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Slot for handling when the main tab changes
//
void PipelineApp::mainTabChanged(int currentIndex)
{
    switch (currentIndex)
    {
    case SUBJECTS_TAB:
        mMonitorPage->stopUpdate();
        mResultsPage->stopUpdate();
        break;

    case RESULTS_TAB:
        mResultsPage->startUpdate();
        mMonitorPage->stopUpdate();
        break;

    case MONITOR_CLUSTER_TAB:
        mMonitorPage->resetAll();
        mResultsPage->stopUpdate();
        break;
    case CLUSTER_LOAD_TAB:
        mMonitorPage->stopUpdate();
        mResultsPage->stopUpdate();
        break;
    }
}

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

///
//	Private slots
//

///
//	User logged in succesfully [slot]
//
void PipelineApp::userLoggedIn(std::string userName, std::string email)
{
    mCurrentUser = userName;
    mCurrentEmail = email;
    mProjectPage->resetAll();
    mCurrentUserLabel->setText(userName);

    bool firstTime = (mStackedWidget->count() == 1);
    if (firstTime)
    {
        mStackedWidget->addWidget(mProjectPage);
    }

    mStackedWidget->setCurrentIndex(1);


}


///
//  Project chosen [slot]
//
void PipelineApp::projectChosen(std::string projectPath)
{
    bool firstTime = (mStackedWidget->count() == 2);

    if (firstTime)
    {
        mStackedWidget->addWidget(mMainSiteWidget);
    }
    mStackedWidget->setCurrentIndex(2);
    mSubjectPage->resetAll();

    if (firstTime)
    {
        mResultsPage->resetAll();
    }
    else
    {
        // Resetting the entire page causes problems, so just reset the
        // user
        mResultsPage->resetUser();
    }

    mCurrentProjectLabel->setText(WString("Current Project: [{1}]").arg(path(projectPath).stem()));

    // Not really a path, don't try to load it
    if (projectPath.find("/") == std::string::npos)
    {
        projectPath = "";
    }
    mSubjectPage->getMRIBrowser()->setFilterFile(projectPath);
}

///
//  Go home [slot]
//
void PipelineApp::goHome()
{
    mProjectPage->resetAll();
    mStackedWidget->setCurrentIndex(1);
}

///
//  Go home [slot]
//
void PipelineApp::logout()
{
    mLoginPage->logout();
    mStackedWidget->setCurrentIndex(0);
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
    // This must be created to use Qt exec() event loop in libwtwithqt.a
    QCoreApplication app(argc, argv);

    return WRun(argc, argv, &createApplication);
}

