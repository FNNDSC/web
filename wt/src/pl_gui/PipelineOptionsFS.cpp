//
//
//  Description:
//      This is the widget for Freesurfer pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineOptionsFS.h"
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
#include <Wt/WButtonGroup>
#include <Wt/WCheckBox>
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
PipelineOptionsFS::PipelineOptionsFS(WContainerWidget *parent) :
    PipelineOptions(parent)
{
    setStyleClass("tabdiv");

    mStageBoxes[0] = new WCheckBox("Perform structural reconstruction");

    for (int i = 0; i < NUM_OPTIONAL_FS_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
        mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    mTalairachCheckBox = new WCheckBox("Perform Talairach registration");
    mStageButtonGroupLayout->addWidget(mTalairachCheckBox);

    resetAll();
}

///
//  Destructor
//
PipelineOptionsFS::~PipelineOptionsFS()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
//  Reset to default state
//
void PipelineOptionsFS::resetAll()
{
    PipelineOptions::resetAll();

    for (int i = 0; i < NUM_OPTIONAL_FS_STAGES; i++)
    {
       mStageBoxes[i]->setChecked(true);
    }

    mTalairachCheckBox->setChecked(false);
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsFS::getCommandLineString() const
{
    std::string args;
    ostringstream stageStrStream;

    for (int i = 1; i < FIRST_OPTIONAL_FS_STAGE; i++)
    {
        stageStrStream << i;
    }

    for (int i = 0; i < NUM_OPTIONAL_FS_STAGES; i++)
    {
        if (mStageBoxes[i]->isChecked())
        {
            stageStrStream << (i + FIRST_OPTIONAL_FS_STAGE);
        }
    }

    args = "-t " + stageStrStream.str();;

    if (!mTalairachCheckBox->isChecked())
    {
        args += " -F -notalairach ";
    }

    return args + " " + PipelineOptions::getCommandLineString();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


