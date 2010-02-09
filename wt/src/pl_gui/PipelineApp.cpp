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
#include "ClusterLoadPage.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include "LoginPage.h"
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
#include <QApplication>

///
//  Namespaces
//
using namespace Wt;

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
    mMonitorPage->createQt();
}

///
//  Callback for destroying Qt objects (see libwtwithqt)
//
void PipelineApp::destroy()
{
    // Destroy Qt Objects
    mMonitorPage->destroyQt();

}

///
// Override finalize to handle destruction of threads
//
void PipelineApp::finalize()
{
    mSubjectPage->finalize();
    mMonitorPage->finalize();
    mClusterLoadPage->finalize();
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
    WImage *chbLogo = new WImage("icons/chbIntranet.gif");
    WImage *brainImage = new WImage("icons/mri.gif");
    chbLogo->setStyleClass("titlediv");
    topLayout->addWidget(chbLogo, 0, 0, Wt::AlignLeft);
    WLabel *titleLabel = new WLabel(w->tr("page-top-text"));
    titleLabel->setStyleClass("titlediv");
    topLayout->addWidget(titleLabel, 0, 1, Wt::AlignCenter | Wt::AlignMiddle);
    topLayout->addWidget(brainImage, 0, 2, Wt::AlignRight);
    topContainer->setLayout(topLayout);

    // Create the top tab
    WTabWidget *topTab = new WTabWidget();
    topTab->setStyleClass("toptabdiv");
    mSubjectPage = new SubjectPage();
    mMonitorPage = new MonitorPage(mSubjectPage->getMRIBrowser());
    mClusterLoadPage = new ClusterLoadPage();
    topTab->addTab(mSubjectPage, "Subjects");
    topTab->addTab(mMonitorPage, "Monitor Cluster");
    topTab->addTab(mClusterLoadPage, "Cluster Load");
    topTab->currentChanged().connect(this, &PipelineApp::mainTabChanged);

    layout->addWidget(topContainer, 0, 0);
    layout->addWidget(topTab, 1, 0);
    layout->addWidget(new WLabel(w->tr("file-bug")), 2, 0, Wt::AlignCenter);
    layout->setRowStretch(1, 1);

    mMainSiteWidget = new WContainerWidget();

    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    mSubjectPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));
    mMonitorPage->resize(WLength(100.0, WLength::Percentage),
                         WLength(100.0, WLength::Percentage));
    mClusterLoadPage->resize(WLength(100.0, WLength::Percentage),
                             WLength(100.0, WLength::Percentage));

    mMainSiteWidget->setLayout(layout);

    LoginPage *loginPage = new LoginPage();
    loginPage->userLoggedIn().connect(this, &PipelineApp::userLoggedIn);

    mStackedWidget = new WStackedWidget();
    mStackedWidget->addWidget(loginPage);
    //mStackedWidget->addWidget(mainSite);
    mStackedWidget->setCurrentIndex(0);

    WVBoxLayout *primaryLayout = new WVBoxLayout();
    primaryLayout->addWidget(mStackedWidget);

    w->setLayout(primaryLayout);

    requestTooLarge().connect(this, &PipelineApp::largeRequest);

    setTitle(w->tr("page-title"));
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
    if (currentIndex == 1)
    {
        mMonitorPage->resetAll();
    }
    else
    {
        mMonitorPage->stopUpdate();
    }

    if (currentIndex == 2)
    {
        mClusterLoadPage->startUpdate();
    }
    else
    {
        mClusterLoadPage->stopUpdate();
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
void PipelineApp::userLoggedIn(std::string userName)
{
    this->log("debug") << "PipelineApp::userLoggedIn";

    mStackedWidget->addWidget(mMainSiteWidget);
    mStackedWidget->setCurrentIndex(1);
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
    QApplication app(argc, argv);

    return WRun(argc, argv, &createApplication);
}

