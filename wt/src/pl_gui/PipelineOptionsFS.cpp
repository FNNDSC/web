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

    mStageBoxes[0] = new WCheckBox("1 - Collect DICOM from repository");
    mStageBoxes[1] = new WCheckBox("2 - Initialize structural reconstruction");
    mStageBoxes[2] = new WCheckBox("3 - Perform structural reconstruction");

    for (int i = 0; i < NUM_FS_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
        mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addWidget(mDirectoryGroupBox, 1, 0);


    resetAll();
}

///
//  Destructor
//
PipelineOptionsFS::~PipelineOptionsFS()
{
    delete mSettingsGroupBox; // Not added to layout, so it has no parent
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
   for (int i = 0; i < NUM_FS_STAGES; i++)
   {
       mStageBoxes[i]->setChecked(true);
   }
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsFS::getCommandLineString() const
{
    std::string stageStr = "";
    std::string args;

    for (int i = 0; i < NUM_FS_STAGES; i++)
    {
        if (mStageBoxes[i]->isChecked())
        {
            ostringstream oss;

            oss << (i + 1);
            stageStr += oss.str();
        }
    }

    args = "-t " + stageStr;

    return args + " " + PipelineOptions::getCommandLineString();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


