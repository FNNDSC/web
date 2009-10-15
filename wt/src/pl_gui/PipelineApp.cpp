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
#include "MRIBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>

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

    /// Create primary layout
    WContainerWidget *topContainer = new WContainerWidget(w);
    WGridLayout *topLayout = new WGridLayout(topContainer);
    topContainer->setStyleClass("topdiv");
    topLayout->addWidget(new WImage("icons/chbIntranet.gif", topContainer), 0, 0, Wt::AlignLeft);
    WLabel *titleLabel = new WLabel("Neuroimaging Pipeline", topContainer);
    titleLabel->setStyleClass("titlediv");
    topLayout->addWidget(titleLabel, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);


    // Create the top tab
    WContainerWidget *bottomContainer = new WContainerWidget(w);
    WTabWidget *topTab = new WTabWidget(bottomContainer);
    topTab->addTab(new MRIBrowser(), "Subjects");
    topTab->addTab(new WText("Monitor cluster"), "Monitor");
    topTab->addTab(new WText("Results browser"), "Results");

  //  layout->addWidget(topContainer, 0, 0);
    //layout->addWidget(bottomContainer, 1, 0);
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
