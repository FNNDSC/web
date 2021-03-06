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
#include "PipelineOptionsFS.h"
#include "PipelineOptionsTract.h"
#include "PipelineOptionsFetal.h"
#include "PipelineOptionsDcmSend.h"
#include "PipelineOptionsConnectome.h"
#include "SelectScans.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WStackedWidget>

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
PipelineConfigure::PipelineConfigure(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    mPipelineStatus = new PipelineStatus();
    mStackedPipelineOptions = new WStackedWidget();

    mPipelineOptionsTract = new PipelineOptionsTract();
    mPipelineOptionsFS = new PipelineOptionsFS();
    mPipelineOptionsFetal = new PipelineOptionsFetal();
    mPipelineOptionsDcmSend = new PipelineOptionsDcmSend();
    mPipelineOptionsConnectome = new PipelineOptionsConnectome();
    mStackedPipelineOptions->addWidget(mPipelineOptionsTract);
    mStackedPipelineOptions->addWidget(mPipelineOptionsFS);
    mStackedPipelineOptions->addWidget(mPipelineOptionsFetal);
    mStackedPipelineOptions->addWidget(mPipelineOptionsDcmSend);
    mStackedPipelineOptions->addWidget(mPipelineOptionsConnectome);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mPipelineStatus, 0, 0);
    layout->addWidget(mStackedPipelineOptions, 0, 1);
    layout->setColumnStretch(1, 1);


    setLayout(layout);

    resetAll();
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
//  Reset all widgets to the default state
//
void PipelineConfigure::resetAll()
{
    mStackedPipelineOptions->setCurrentIndex(0);
    mPipelineOptionsFetal->resetAll();
    mPipelineOptionsTract->resetAll();
    mPipelineOptionsFS->resetAll();
    mPipelineOptionsDcmSend->resetAll();
    mPipelineStatus->resetAll();
    mPipelineOptionsCurrent = mPipelineOptionsTract;
}

///
///  Update all elements of widget to current values (on next clicked)
///
void PipelineConfigure::updateAll(SelectScans *selectScans)
{
    mScansToProcessData = selectScans->getScansToProcess(0);
    mPipelineType = selectScans->getCurrentPipeline();
    mPipelineStatus->updateAll(selectScans);
    switch(mPipelineType)
    {
    case Enums::PIPELINE_TYPE_TRACT:
        mStackedPipelineOptions->setCurrentIndex(0);
        mPipelineOptionsCurrent = mPipelineOptionsTract;
        break;
    case Enums::PIPELINE_TYPE_FS:
        mStackedPipelineOptions->setCurrentIndex(1);
        mPipelineOptionsCurrent = mPipelineOptionsFS;
        break;
    case Enums::PIPELINE_TYPE_FETAL:
        mStackedPipelineOptions->setCurrentIndex(2);
        mPipelineOptionsCurrent = mPipelineOptionsFetal;
        break;
    case Enums::PIPELINE_TYPE_DCMSEND:
        mStackedPipelineOptions->setCurrentIndex(3);
        mPipelineOptionsCurrent = mPipelineOptionsDcmSend;
        break;
    case Enums::PIPELINE_TYPE_CONNECTOME:
        mStackedPipelineOptions->setCurrentIndex(4);
        mPipelineOptionsCurrent = mPipelineOptionsConnectome;
        break;

    }
}


///
//  Check whether the options are valid
//
bool PipelineConfigure::validate() const
{
    return mPipelineOptionsCurrent->validate();
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineConfigure::getCommandLineString() const
{
    return mPipelineOptionsCurrent->getCommandLineString();
}

///
//  Get the current directory suffix
//
std::string PipelineConfigure::getOutputDirSuffix() const
{
    return mPipelineOptionsCurrent->getOutputDirSuffix();
}

///
//  Get the current file suffix
//
std::string PipelineConfigure::getOutputFileSuffix() const
{
    return mPipelineOptionsCurrent->getOutputFileSuffix();
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

