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
#include "PipelineApp.h"
#include "SubjectPage.h"
#include "SelectScans.h"
//#include "PipelineConfigure.h"
//#include "ConfigOptions.h"
//#include "SubmitJobDialog.h"
#include "MRIBrowser.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
#include <Wt/WDate>
#include <Wt/WImage>
//#include <signal.h>
//#include <boost/process/process.hpp>
//#include <boost/process/child.hpp>
//#include <boost/process/launch_shell.hpp>
//#include <boost/filesystem.hpp>
//#include <sys/time.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <ctime>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
//using namespace boost::filesystem;
//using namespace boost::processes;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
SubjectPage::SubjectPage(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();
    mSelectScans = new SelectScans();
    //mPipelineConfigure = new PipelineConfigure(mSelectScans->getScansToProcess(),
      //                                         mSelectScans->getCurrentPipeline());

    mStackedStage = new WStackedWidget();
    mStackedStage->addWidget(mSelectScans);
    //mStackedStage->addWidget(mPipelineConfigure);

    layout->addWidget(mStackedStage, 0, 0);

    mNextButton = new WPushButton("Next ->");
    mNextButton->setMinimumSize(WLength(65, WLength::Pixel), WLength::Auto);
    mBackButton = new WPushButton("<- Back");
    mBackButton->setMinimumSize(WLength(65, WLength::Pixel), WLength::Auto);

    mRefreshMRIBrowserButton = new WPushButton("New scan received, click to refresh MRID list.");
    mLoadingImage = new WImage("icons/ajax-loader-2.gif");

    WGridLayout *navLayout = new WGridLayout();
    navLayout->addWidget(mLoadingImage, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    navLayout->addWidget(mRefreshMRIBrowserButton, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    navLayout->addWidget(mBackButton, 0, 3, Wt::AlignRight);
    navLayout->addWidget(mNextButton, 0, 4, Wt::AlignLeft);
    navLayout->setColumnStretch(2, 1);
    layout->addLayout(navLayout, 1, 0);

    // Let row 0 take the excess space
    layout->setRowStretch(0, 1);
    mLoadingImage->hide();
    mRefreshMRIBrowserButton->hide();


    setLayout(layout);

    mMessageBox = new WMessageBox();
    mMessageBox->buttonClicked().connect(SLOT(this, SubjectPage::handleMessageBoxFinished));
//    mSubmitJobDialog = new SubmitJobDialog("Submit Scans");
  //  mSubmitJobDialog->finished().connect(SLOT(this, SubjectPage::handleSubmitScans));

    // Signal/slot connections
    mSelectScans->getScanAdded().connect(SLOT(this, SubjectPage::scanAdded));
    mNextButton->clicked().connect(SLOT(this, SubjectPage::nextClicked));
    mBackButton->clicked().connect(SLOT(this, SubjectPage::backClicked));
//    mSelectScans->getMRIBrowser()->mriListUpdated().connect(SLOT(this, SubjectPage::mriListUpdated));
    mRefreshMRIBrowserButton->clicked().connect(SLOT(this, SubjectPage::refreshMRIList));

    resetAll();
}

///
//  Destructor
//
SubjectPage::~SubjectPage()
{
    delete mMessageBox;
    //delete mSubmitJobDialog;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
// Reset all widgets to the default state
//
void SubjectPage::resetAll()
{
    mSubjectState = SCAN_SELECT;
    mStackedStage->setCurrentIndex(0);
    mNextButton->disable();
    mBackButton->disable();
    mNextButton->setText("Next ->");

    mSelectScans->resetAll();
    //mPipelineConfigure->resetAll();


}

///
//  Finalize the widget (pre-destruction)
//
void SubjectPage::finalize()
{
    mSelectScans->finalize();
}

///
//  Create Qt objects
//
void SubjectPage::createQt()
{
    mSelectScans->createQt();
}

///
///  Destroy Qt objects
///
void SubjectPage::destroyQt()
{
    mSelectScans->destroyQt();
}

///
//  Get the MRI browser
//
MRIBrowser* SubjectPage::getMRIBrowser() const
{
    return mSelectScans->getMRIBrowser();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Creates a title widget.
//
WText* SubjectPage::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

///
//  Slot for when the scan is added to the queue
//
void SubjectPage::scanAdded(bool added)
{
    if (mSubjectState == SCAN_SELECT)
    {
        if (added)
        {
            mNextButton->enable();
        }
        else
        {
            mNextButton->disable();
        }

    }
}

///
//  Slot for when the next button is clicked
//
void SubjectPage::nextClicked()
{
    switch(mSubjectState)
    {
    // Go to config state
    case SCAN_SELECT:
        if (mSelectScans->getCurrentPipeline() == Enums::PIPELINE_UNKNOWN)
        {
            mMessageBox->setWindowTitle("Error");
            mMessageBox->setText("Pipeline Type is UNKNOWN.  If a pipeline type was not detected automatically, "\
                                 "please override it using the 'Override' button.");
            mMessageBox->setButtons(Wt::Ok);
            mMessageBox->show();
            return;
        }


        mBackButton->enable();
        mStackedStage->setCurrentIndex((int)PIPELINE_CONFIGURE);
        //mPipelineConfigure->updateAll();
        mSubjectState = PIPELINE_CONFIGURE;
        mNextButton->setText("Finish");
        break;

    case PIPELINE_CONFIGURE:
    default:

        // Make sure the input validates before submitting
        //if (!mPipelineConfigure->validate())
        {
            return;
        }

        //mSubmitJobDialog->setCommandLine(mPipelineConfigure->getCommandLineString());
        //mSubmitJobDialog->show();
        break;
    }
}


///
//  Slot for when the back button is clicked
//
void SubjectPage::backClicked()
{
    switch(mSubjectState)
    {
    case SCAN_SELECT:
        // There is no back on scan select
        break;

    case PIPELINE_CONFIGURE:
    default:
        mStackedStage->setCurrentIndex((int)SCAN_SELECT);
        mSubjectState = SCAN_SELECT;
        mBackButton->disable();
        mNextButton->setText("Next ->");
        break;
    }

}

///
//  Submit scans for processing.  This function will generate the file and
//  execute pl_batch.bash on it to put it into the processing queue.
//
bool SubjectPage::submitForProcessing(const std::string& pipelineCommandLineString)
{

    // No need for all this in repro app
    return false;
}

///
//  Return the number of lines in a file
//
int SubjectPage::getNumberOfLines(const char *fileName)
{
    int lineCount = 0;
    ifstream ifs(fileName, ios::in);

    if (ifs.is_open())
    {
        while (!ifs.eof())
        {
            char buf[1024] = {0};
            ifs.getline( buf, sizeof(buf));

            if(strlen(buf) > 2)
            {
                lineCount++;
            }
        }

        ifs.close();
    }

    return lineCount;
}

///
//  Handle message box finished [slot]
//
void SubjectPage::handleMessageBoxFinished(StandardButton)
{
    mMessageBox->hide();
}

///
//  Handle submit scans to cluster [slot]
//
void SubjectPage::handleSubmitScans(WDialog::DialogCode dialogCode)
{
    // Repro app - not needed
    return ;
    }

///
//  MRI list updated [slot]
//
void SubjectPage::mriListUpdated()
{
    mLoadingImage->show();
    mRefreshMRIBrowserButton->show();
}

///
//  Refresh the MRI list [slot]
//
void SubjectPage::refreshMRIList()
{
    mSelectScans->getMRIBrowser()->refreshMRIList();
    mLoadingImage->hide();
    mRefreshMRIBrowserButton->hide();
}
