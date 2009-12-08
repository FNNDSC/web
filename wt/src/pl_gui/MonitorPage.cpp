//
//
//  Description:
//      This is the widget that controls and lays out the entire "Subject" page.
//      This includes the MRID browser, Scan browser, and widgets to kick off
//      a pipeline analysis.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "MonitorPage.h"
#include "MonitorLogTab.h"
#include "MonitorResultsTab.h"
#include "ClusterJobBrowser.h"
#include "ConfigOptions.h"
#include "MRIBrowser.h"
#include "PatientInfoBox.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
#include <Wt/WTabWidget>
#include <signal.h>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;
using namespace boost::processes;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
MonitorPage::MonitorPage(const MRIBrowser *mriBrowser,
                         WContainerWidget *parent) :
    WContainerWidget(parent),
    mMRIBrowser(mriBrowser)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(createTitle("Cluster Jobs"), 0, 0);

    WTabWidget *tabWidget = new WTabWidget();
    tabWidget->setStyleClass("toptabdiv");
    mMonitorLogTab = new MonitorLogTab(mriBrowser);
    mMonitorResultsTab = new MonitorResultsTab(mriBrowser);
    tabWidget->addTab(mMonitorResultsTab, "Results");
    tabWidget->addTab(mMonitorLogTab, "Logs");
    layout->addWidget(tabWidget, 1, 1);

    WVBoxLayout *clusterJobsLayout = new WVBoxLayout();
    mClusterJobBrowser = new ClusterJobBrowser();
    mPatientInfoBox = new PatientInfoBox();
    mPatientInfoBox->setStyleClass("verysmallgroupdiv");

    clusterJobsLayout->addWidget(mClusterJobBrowser);
    clusterJobsLayout->addWidget(mPatientInfoBox);

    layout->addLayout(clusterJobsLayout, 1, 0);


    // Let row 1 and column 2 take the excess space.
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);

    // Make connections
    mClusterJobBrowser->clusterJobSelected().connect(SLOT(this, MonitorPage::jobSelectedChanged));

    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    mMonitorLogTab->resize(WLength(100, WLength::Percentage),
                           WLength(100, WLength::Percentage));
    mMonitorResultsTab->resize(WLength(100, WLength::Percentage),
                               WLength(100, WLength::Percentage));

}

///
//  Destructor
//
MonitorPage::~MonitorPage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
// Reset all widgets to the default state
//
void MonitorPage::resetAll()
{
    mMonitorLogTab->resetAll();
    mMonitorResultsTab->resetAll();
    mPatientInfoBox->resetAll();
    mClusterJobBrowser->resetAll();
}

///
//  Finalize the widget (pre-destruction)
//
void MonitorPage::finalize()
{
    mMonitorLogTab->finalize();
    mMonitorResultsTab->finalize();
}

///
//  Start updating logs
//
void MonitorPage::startUpdate()
{
    mMonitorLogTab->startUpdate();
}

///
//  Stop updating logs
//
void MonitorPage::stopUpdate()
{
    mMonitorLogTab->stopUpdate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Handle job selection changes [slot]
//
void MonitorPage::jobSelectedChanged(std::string jobSelectedFile)
{

    path logBaseDir = path(jobSelectedFile).branch_path();
    std::string scanDir = logBaseDir.branch_path().string();

    mMonitorLogTab->jobSelectedChanged(jobSelectedFile);
    mMonitorResultsTab->jobSelectedChanged(jobSelectedFile);

    mPatientInfoBox->resetAll();
    mPatientInfoBox->setScanDir(scanDir);
}


///
//  Creates a title widget.
//
WText* MonitorPage::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

