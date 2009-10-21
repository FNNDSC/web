//
//
//  Description:
//      This is the container widget for the pipeline configuration
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineConfigure.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>

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
PipelineConfigure::PipelineConfigure(const vector<ScanBrowser::ScanData>& scansToProcess,
                                     const Enums::PipelineType& pipelineType,
                                     WContainerWidget *parent) :
    WContainerWidget(parent),
    mScansToProcessData(scansToProcess),
    mPipelineType(pipelineType)
{
    setStyleClass("tabdiv");

    mPipelineStatus = new PipelineStatus(scansToProcess, pipelineType);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mPipelineStatus, 0, 0);

    setLayout(layout);
}

///
//  Destructor
//
PipelineConfigure::~PipelineConfigure()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
///  Update all elements of widget to current values (on next clicked)
///
void PipelineConfigure::updateAll()
{
    mPipelineStatus->updateAll();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Creates a title widget.
//
WText* PipelineConfigure::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

