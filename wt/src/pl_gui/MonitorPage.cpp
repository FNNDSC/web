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
#include "LogFileBrowser.h"
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
    layout->addWidget(createTitle("Logs"), 0, 1);

    WVBoxLayout *clusterJobsLayout = new WVBoxLayout();
    mClusterJobBrowser = new ClusterJobBrowser();
    mPatientInfoBox = new PatientInfoBox();
    mPatientInfoBox->setStyleClass("smallgroupdiv");

    clusterJobsLayout->addWidget(mClusterJobBrowser);
    clusterJobsLayout->addWidget(mPatientInfoBox);

    layout->addLayout(clusterJobsLayout, 1, 0);

    mLogFileBrowser = new LogFileBrowser();

    WVBoxLayout *logInfoLayout = new WVBoxLayout();
    logInfoLayout->addWidget(mLogFileBrowser);
    layout->addLayout(logInfoLayout, 1, 1);

    mLogStdOut = new LogFileTailer("", false);
    mLogStdErr = new LogFileTailer("", true);
    WVBoxLayout *vbox = new WVBoxLayout();
    vbox->addWidget(mLogStdOut);
    vbox->addWidget(mLogStdErr);
    layout->addLayout(vbox, 1, 2);

    // Let row 1 and column 2 take the excess space.
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(2, 1);

    setLayout(layout);

    // Make connections
    mClusterJobBrowser->clusterJobSelected().connect(SLOT(this, MonitorPage::jobSelectedChanged));
    mLogFileBrowser->logFileSelected().connect(SLOT(this, MonitorPage::logSelectedChanged));

    mLogFileBrowser->hide();
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
    mLogFileBrowser->hide();
    mPatientInfoBox->resetAll();
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
    mLogFileBrowser->hide();
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

    std::string logDirName = logBaseDir.leaf();
    mLogFileBrowser->setLogBaseDir(logBaseDir.string());

    if (mMRIBrowser != NULL)
    {
        // The log directory will be
        // <outputDir>/MRID-<logDirNameStripped>  (where <logDirNameStripped> is stripped of
        //                                         leading "log")

        std::string mrid = mMRIBrowser->getMRIDFromScanDir(scanDir);

        // Strip leading "log"
        logDirName.erase(logDirName.begin(), logDirName.begin() + 3);

        std::string postProcDir = ConfigOptions::GetPtr()->GetOutDir() + "/" +
                                  mrid + logDirName;

        mLogFileBrowser->setPostProcDir(postProcDir);


    }
    mLogFileBrowser->resetAll();
    mLogFileBrowser->show();

    mPatientInfoBox->resetAll();
    mPatientInfoBox->setScanDir(scanDir);
}

///
//  Handle log selection changes [slot]
//
void MonitorPage::logSelectedChanged(LogFileBrowser::LogFileEntry logFileEntry)
{
    std::string baseLogName;

    baseLogName = logFileEntry.mBaseLogDir + "/" +
                  logFileEntry.mBaseLogName;

    if (logFileEntry.mHasStdOut)
    {
        mLogStdOut->setLogFile(baseLogName + ".std");
        mLogStdOut->show();
        mLogStdOut->startUpdate();
    }
    else
    {
        mLogStdOut->hide();
        mLogStdOut->stopUpdate();
    }

    if (logFileEntry.mHasStdErr)
    {
        mLogStdErr->setLogFile(baseLogName + ".err");
        mLogStdErr->show();
        mLogStdErr->startUpdate();
    }
    else
    {
        mLogStdErr->hide();
        mLogStdErr->stopUpdate();
    }
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

