//
//
//  Description:
//      This is the container widget for the MRI Browser and Scan selector
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "SelectScans.h"
#include "MRIBrowser.h"
#include "ScanBrowser.h"
//#include "ClusterLoadChart.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTimer>

///
//  Namespaces
//
using namespace Wt;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
SelectScans::SelectScans(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(createTitle("MRIDs"), 0, 0);


    mMRIBrowser = new MRIBrowser();
    mScanBrowser = new ScanBrowser();

    layout->addWidget(mMRIBrowser, 1, 0);
    layout->addWidget(mScanBrowser, 1, 1);




    /*mClusterLoadChart = new ClusterLoadChart();
    WHBoxLayout *topHBox = new WHBoxLayout();
    topHBox->addWidget(createTitle("Info"), AlignTop);
    topHBox->addStretch(2000);
    topHBox->addWidget(mClusterText = createTitle("Cluster Usage"), AlignTop);
    topHBox->addWidget(mClusterLoadChart, AlignTop);
    layout->addLayout(topHBox, 0, 1);*/


    // Let row 1 and column 2 take the excess space.
    layout->setRowStretch(0, -1);
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);

    setLayout(layout);

    // Signal/slot connections
    mMRIBrowser->mriSelected().connect(SLOT(this, SelectScans::mriChanged));
    mScanBrowser->scanAdded().connect(SLOT(this, SelectScans::scanAdded));

    resetAll();
}

///
//  Destructor
//
SelectScans::~SelectScans()
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
void SelectScans::resetAll()
{
    mScanBrowser->hide();
    mScanBrowser->resetAll();
    mMRIBrowser->resetAll();
    //mClusterLoadChart->hide();
    //mClusterText->hide();
 //   mClusterLoadChart->stopUpdate();
}

///
//  Finalize the widget (pre-destruction)
//
void SelectScans::finalize()
{
//    mClusterLoadChart->finalize();
 //   mMRIBrowser->finalize();
}

///
//  Create Qt objects
//
void SelectScans::createQt()
{
  //  mMRIBrowser->createQt();
}

///
///  Destroy Qt objects
///
void SelectScans::destroyQt()
{
    //mMRIBrowser->destroyQt();
}

///
//  Get pipeline type as string (for command line arg)
//
const std::string SelectScans::getCurrentPipelineAsString() const
{
    if (getCurrentPipeline() == Enums::PIPELINE_TYPE_TRACT)
    {
        return std::string("tract");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_FS)
    {
        return std::string("FS");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_FETAL)
    {
        return std::string("fetal");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_CONNECTOME)
    {
        return std::string("connectome");
    }
    else
    {
        return std::string("dcmsend");
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Creates a title widget.
//
WText* SelectScans::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

///
//  Slot for when the MRI selection changes
//
void SelectScans::mriChanged(std::string mrid, std::string scanDir, std::string age)
{
    mScanBrowser->setCurMRID(mrid);
    mScanBrowser->setScanDir(scanDir);
    mScanBrowser->setCurAge(age);
    mScanBrowser->show();
    //mClusterLoadChart->show();
    //mClusterText->show();
    //mClusterLoadChart->startUpdate();
}


///
//  Slot for when the scan is added to the queue
//
void SelectScans::scanAdded(bool added)
{
    mScanAdded.emit(added);
}
