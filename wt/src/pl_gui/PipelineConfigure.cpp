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
PipelineConfigure::PipelineConfigure(const vector<ScanBrowser::ScanData>& scansToProcess,
                                     const Enums::PipelineType& pipelineType,
                                     WContainerWidget *parent) :
    WContainerWidget(parent),
    mScansToProcessData(scansToProcess),
    mPipelineType(pipelineType)
{
    setStyleClass("tabdiv");

    mPipelineStatus = new PipelineStatus(scansToProcess, pipelineType);
    mStackedPipelineOptions = new WStackedWidget();

    mPipelineOptionsTract = new PipelineOptionsTract();
    mPipelineOptionsFS = new PipelineOptionsFS();
    mStackedPipelineOptions->addWidget(mPipelineOptionsTract);
    mStackedPipelineOptions->addWidget(mPipelineOptionsFS);


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
    mPipelineOptionsTract->resetAll();
    mPipelineOptionsFS->resetAll();
    mPipelineStatus->resetAll();
}

///
///  Update all elements of widget to current values (on next clicked)
///
void PipelineConfigure::updateAll()
{
    mPipelineStatus->updateAll();

    switch(mPipelineType)
    {
    case Enums::PIPELINE_TYPE_TRACT:
        mStackedPipelineOptions->setCurrentIndex(0);
        break;
    case Enums::PIPELINE_TYPE_FS:
        mStackedPipelineOptions->setCurrentIndex(1);
        break;
    }
}


///
//  Check whether the options are valid
//
bool PipelineConfigure::validate() const
{
    if (mPipelineType == Enums::PIPELINE_TYPE_TRACT)
    {
        return mPipelineOptionsTract->validate();
    }
    else
    {
        return mPipelineOptionsFS->validate();
    }
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineConfigure::getCommandLineString() const
{
    if (mPipelineType == Enums::PIPELINE_TYPE_TRACT)
    {
        return mPipelineOptionsTract->getCommandLineString();
    }
    else
    {
        return mPipelineOptionsFS->getCommandLineString();
    }
}

///
//  Get the current directory suffix
//
std::string PipelineConfigure::getOutputDirSuffix() const
{
    if (mPipelineType == Enums::PIPELINE_TYPE_TRACT)
    {
        return mPipelineOptionsTract->getOutputDirSuffix();
    }
    else
    {
        return mPipelineOptionsFS->getOutputDirSuffix();
    }
}

///
//  Get the current file suffix
//
std::string PipelineConfigure::getOutputFileSuffix() const
{
    if (mPipelineType == Enums::PIPELINE_TYPE_TRACT)
    {
        return mPipelineOptionsTract->getOutputFileSuffix();
    }
    else
    {
        return mPipelineOptionsFS->getOutputFileSuffix();
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
WText* PipelineConfigure::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

