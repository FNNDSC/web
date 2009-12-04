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
#include "PipelineApp.h"
#include "MonitorLogTab.h"
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
MonitorLogTab::MonitorLogTab(const MRIBrowser *mriBrowser,
                             WContainerWidget *parent) :
    WContainerWidget(parent),
    mMRIBrowser(mriBrowser)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();


    mLogFileBrowser = new LogFileBrowser();

    WVBoxLayout *logInfoLayout = new WVBoxLayout();
    logInfoLayout->addWidget(mLogFileBrowser);
    layout->addLayout(logInfoLayout, 0, 0);

    mLogStdOut = new LogFileTailer("", false);
    mLogStdErr = new LogFileTailer("", true);
    WVBoxLayout *vbox = new WVBoxLayout();
    vbox->addWidget(mLogStdOut);
    vbox->addWidget(mLogStdErr);
    layout->addLayout(vbox, 0, 1);

    layout->setRowStretch(0, -1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);

    // Make connections
    mLogFileBrowser->logFileSelected().connect(SLOT(this, MonitorLogTab::logSelectedChanged));

    mLogFileBrowser->hide();
    mLogStdOut->hide();
    mLogStdErr->hide();
}

///
//  Destructor
//
MonitorLogTab::~MonitorLogTab()
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
void MonitorLogTab::resetAll()
{
    mLogStdOut->hide();
    mLogStdErr->hide();
    mLogFileBrowser->hide();
}

///
//  Finalize the widget (pre-destruction)
//
void MonitorLogTab::finalize()
{
    mLogStdOut->finalize();
    mLogStdErr->finalize();
}

///
//  Start updating logs
//
void MonitorLogTab::startUpdate()
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
void MonitorLogTab::stopUpdate()
{
    mLogStdErr->stopUpdate();
    mLogStdOut->stopUpdate();
    mLogStdErr->hide();
    mLogStdOut->hide();
    mLogFileBrowser->hide();
}

///
//  Handle job selection changes
//
void MonitorLogTab::jobSelectedChanged(std::string jobSelectedFile)
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

        std::string postProcDir = getConfigOptionsPtr()->GetOutDir() + "/" +
                                  mrid + logDirName;

        WApplication::instance()->log("debug") << "ScanDir: " << scanDir;
        WApplication::instance()->log("debug") << "PostProcDir: " << postProcDir;
        mLogFileBrowser->setPostProcDir(postProcDir);


    }
    mLogFileBrowser->resetAll();
    mLogFileBrowser->show();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Handle log selection changes [slot]
//
void MonitorLogTab::logSelectedChanged(LogFileBrowser::LogFileEntry logFileEntry)
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


