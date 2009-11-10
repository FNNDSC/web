//
//
//  Description:
//      This is the widget for tractograpy pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineOptionsTract.h"
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
PipelineOptionsTract::PipelineOptionsTract(WContainerWidget *parent) :
    PipelineOptions(parent)
{
    setStyleClass("tabdiv");

    mStageBoxes[0] = new WCheckBox("1 - Collect DICOM from repository");
    mStageBoxes[1] = new WCheckBox("2 - Construct tractograpy");
    mStageBoxes[2] = new WCheckBox("3 - Slice 3D model along anatomical planes");
#if 0 // Temporarily disabled until we get MatLAB
    mStageBoxes[3] = new WCheckBox("4 - Convert slices to DICOM");
    mStageBoxes[4] = new WCheckBox("5 - Transmit slices to PACS");
#endif
    for (int i = 0; i < NUM_TRACT_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
        mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    resetAll();
}

///
//  Destructor
//
PipelineOptionsTract::~PipelineOptionsTract()
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
void PipelineOptionsTract::resetAll()
{
    PipelineOptions::resetAll();
    for (int i = 0; i < NUM_TRACT_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
    }
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsTract::getCommandLineString() const
{
    std::string stageStr = "";
    std::string args;

    for (int i = 0; i < NUM_TRACT_STAGES; i++)
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


