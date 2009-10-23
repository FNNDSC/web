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
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WOverlayLoadingIndicator>

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
    createUI();
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
    WContainerWidget *w = root();
    w->setStyleClass("maindiv");

    WGridLayout *layout = new WGridLayout();

    /// Create top container holding logo and title
    WContainerWidget *topContainer = new WContainerWidget();
    WGridLayout *topLayout = new WGridLayout();
    topContainer->setStyleClass("topdiv");
    WImage *chbLogo = new WImage("icons/chbIntranet.gif");
    chbLogo->setStyleClass("titlediv");
    topLayout->addWidget(chbLogo, 0, 0, Wt::AlignLeft);
    WLabel *titleLabel = new WLabel("Neuroimaging Pipeline");
    titleLabel->setStyleClass("titlediv");
    topLayout->addWidget(titleLabel, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    topContainer->setLayout(topLayout);

    // Create the top tab
    WTabWidget *topTab = new WTabWidget();
    SubjectPage *subjectPage = new SubjectPage();
    topTab->addTab(subjectPage, "Subjects");
    topTab->addTab(new WText("Monitor cluster"), "Monitor");
    topTab->addTab(new WText("Results browser"), "Results");

    layout->addWidget(topContainer, 0, 0);
    layout->addWidget(topTab, 1, 0);
    layout->setRowStretch(1, 1);

    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    subjectPage->resize(WLength(100.0, WLength::Percentage),
                        WLength(100.0, WLength::Percentage));

    w->setLayout(layout);
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
    WApplication *app = new PipelineApp(env);
    app->setTwoPhaseRenderingThreshold(0);
    app->setTitle("CHB Neuroimaging Pipeline");
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
    // Start by loading configuration options
    if(!ConfigOptions::GetPtr()->LoadFromFile("conf/pl_gui.conf"))
    {
        return 1;
    }

    return WRun(argc, argv, &createApplication);
}
