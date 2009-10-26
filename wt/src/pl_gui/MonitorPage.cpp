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
#include "ClusterJobBrowser.h"
#include "LogFileTailer.h"
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
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
MonitorPage::MonitorPage(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(createTitle("Cluster Jobs"), 0, 0);
    layout->addWidget(createTitle("Logs"), 0, 1);

    mClusterJobBrowser = new ClusterJobBrowser();
    layout->addWidget(mClusterJobBrowser, 1, 0);


    mLogStdOut = new LogFileTailer("");
    mLogStdErr = new LogFileTailer("");
    WVBoxLayout *vbox = new WVBoxLayout();
    vbox->addWidget(mLogStdOut);
    vbox->addWidget(mLogStdErr);
    layout->addLayout(vbox, 1, 1);

    // Let row 1 and column 2 take the excess space.
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);

    // Make connections
    mClusterJobBrowser->clusterJobSelected().connect(SLOT(this, MonitorPage::jobSelectedChanged));

    mLogStdOut->hide();
    mLogStdErr->hide();
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
    mLogStdOut->hide();
    mLogStdErr->hide();
    mClusterJobBrowser->resetAll();
}

///
//  Start updating logs
//
void MonitorPage::startUpdate()
{
    if (!mLogStdOut->isHidden())
    {
        mLogStdErr->startUpdate();
        mLogStdOut->startUpdate();
    }
}

///
//  Stop updating logs
//
void MonitorPage::stopUpdate()
{
    mLogStdErr->stopUpdate();
    mLogStdOut->stopUpdate();
    mLogStdErr->hide();
    mLogStdOut->hide();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
/// Handle job selection changes [slot]
///
void MonitorPage::jobSelectedChanged(std::string jobSelectedFile)
{
    mLogStdOut->setLogFile(jobSelectedFile + ".std");
    mLogStdErr->setLogFile(jobSelectedFile + ".err");

    mLogStdOut->show();
    mLogStdErr->show();
    mLogStdOut->startUpdate();
    mLogStdErr->startUpdate();
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

