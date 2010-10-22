//
//
//  Description:
//      This is the the left pane of the pipeline configuration screen
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineStatus.h"
#include "SelectScans.h"
#include "GlobalEnums.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WSelectionBox>
#include <vector>

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
PipelineStatus::PipelineStatus(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WVBoxLayout *layout = new WVBoxLayout();
    WGroupBox *pipelineTypeBox = new WGroupBox("Pipeline Type");
    pipelineTypeBox->setStyleClass("groupdiv");
    WGroupBox *selectedScansBox = new WGroupBox("Selected Scans");
    selectedScansBox->setStyleClass("groupdiv");

    // Pipeline Type box
    WVBoxLayout *pipelineTypeBoxLayout = new WVBoxLayout();
    mPipelineTypeLabel = new WLabel("");
    pipelineTypeBoxLayout->addWidget(mPipelineTypeLabel);
    pipelineTypeBox->setLayout(pipelineTypeBoxLayout);

    // Selected Scans box
    WVBoxLayout *selectedScansBoxLayout = new WVBoxLayout();
    mScansToProcessTable = new ScansToProcessTable();
    mScansToProcessTable->resize(WLength(350, WLength::Pixel), WLength::Auto);
    selectedScansBoxLayout->addWidget(mScansToProcessTable);
    selectedScansBox->setLayout(selectedScansBoxLayout);

    layout->addWidget(pipelineTypeBox);
    layout->addWidget(selectedScansBox);
    //layout->addStretch(100);
    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
PipelineStatus::~PipelineStatus()
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
void PipelineStatus::resetAll()
{
    mPipelineTypeLabel->setText("Unknown");
    mScansToProcessTable->resetAll();
}

///
//  Update all elements of widget to current values (on next clicked)
//
void PipelineStatus::updateAll(SelectScans *selectScans)
{
    switch(selectScans->getCurrentPipeline())
    {
    case Enums::PIPELINE_TYPE_TRACT:
        mPipelineTypeLabel->setText("Tractography");
        break;
    case Enums::PIPELINE_TYPE_FS:
        mPipelineTypeLabel->setText("Structural Reconstruction");
        break;
    case Enums::PIPELINE_TYPE_FETAL:
        mPipelineTypeLabel->setText("Fetal Extraction");
        break;
    case Enums::PIPELINE_TYPE_DCMSEND:
        mPipelineTypeLabel->setText("Send to Remote PACS");
        break;
    case Enums::PIPELINE_TYPE_CONNECTOME:
        mPipelineTypeLabel->setText("Connectome");
        break;
    case Enums::PIPELINE_UNKNOWN:
        mPipelineTypeLabel->setText("Unknown");
        break;
    }

    // Update the scans to process list
    mScansToProcessTable->copyTable(selectScans->getScansToProcessTable());
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Creates a title widget.
//
WText* PipelineStatus::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}


