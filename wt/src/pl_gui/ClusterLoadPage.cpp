//
//
//  Description:
//      This is the widget that controls and lays out the entire "Cluster Load" page.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "ClusterLoadPage.h"
#include "ClusterLoadChart.h"
#include "ConfigOptions.h"
#include "LogFileTailer.h"
#include "PipelineApp.h"
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
#include <Wt/WText>
#include <stdlib.h>

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
ClusterLoadPage::ClusterLoadPage(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    mTopFileTailer = new LogFileTailer(getConfigOptionsPtr()->GetTopLogFile(), false, 1000, false);
    mClusterLoadChart = new ClusterLoadChart();

    WGridLayout *gridBox = new WGridLayout();
    gridBox->addWidget(createTitle("Cluster Usage"), 0, 0, AlignRight | AlignMiddle);
    gridBox->addWidget(mClusterLoadChart, 0, 1, AlignLeft);

    WGridLayout *layout = new WGridLayout();
    layout->addLayout(gridBox, 0, 0);
    layout->addWidget(mTopFileTailer, 1, 0);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

///
//  Destructor
//
ClusterLoadPage::~ClusterLoadPage()
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
void ClusterLoadPage::resetAll()
{
}

///
//  Finalize the widget (pre-destruction)
//
void ClusterLoadPage::finalize()
{
    mTopFileTailer->finalize();
    mClusterLoadChart->finalize();
}

///
//  Start updating logs
//
void ClusterLoadPage::startUpdate()
{
    mTopFileTailer->startUpdate();
    mClusterLoadChart->startUpdate();
}

///
//  Stop updating logs
//
void ClusterLoadPage::stopUpdate()
{
    mTopFileTailer->stopUpdate();
    mClusterLoadChart->stopUpdate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Creates a title widget.
//
WText* ClusterLoadPage::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

