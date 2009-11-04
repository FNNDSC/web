//
//
//  Description:
//      This is the widget that contains the results browser
//      chosen cluster job.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "MonitorResultsTab.h"
#include "ConfigOptions.h"
#include "MRIBrowser.h"
#include "ResultsBrowser.h"
#include "FilePreviewBox.h"
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
MonitorResultsTab::MonitorResultsTab(const MRIBrowser *mriBrowser,
                                     WContainerWidget *parent) :
    WContainerWidget(parent),
    mMRIBrowser(mriBrowser)
{
    setStyleClass("tabdiv");

    mResultsBrowser = new ResultsBrowser();
    mFilePreviewBox = new FilePreviewBox();
    mFilePreviewBox->setStyleClass("smallgroupdiv");

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mResultsBrowser, 0, 0);
    layout->addWidget(mFilePreviewBox, 0, 1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);

    // Make connections
    mResultsBrowser->resultFileSelected().connect(SLOT(this, MonitorResultsTab::resultFileChanged));

    mResultsBrowser->hide();
    mFilePreviewBox->hide();
}

///
//  Destructor
//
MonitorResultsTab::~MonitorResultsTab()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
//  Reset all widgets to the default state
//
void MonitorResultsTab::resetAll()
{
    mResultsBrowser->hide();
    mFilePreviewBox->hide();
}

///
//  Handle job selection changes
//
void MonitorResultsTab::jobSelectedChanged(std::string jobSelectedFile)
{
    path logBaseDir = path(jobSelectedFile).branch_path();
    std::string scanDir = logBaseDir.branch_path().string();
    string scriptFileName = path(jobSelectedFile).leaf();

    mResultsBrowser->setPipelineName(scriptFileName);

    std::string logDirName = logBaseDir.leaf();

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

       mResultsBrowser->setResultsBaseDir(postProcDir);


    }

   mResultsBrowser->resetAll();
   mResultsBrowser->show();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Handle result file selection changes [slot]
//
void MonitorResultsTab::resultFileChanged(std::string resultFilePath)
{
    mFilePreviewBox->setFilePath(resultFilePath);
    mFilePreviewBox->show();
}

