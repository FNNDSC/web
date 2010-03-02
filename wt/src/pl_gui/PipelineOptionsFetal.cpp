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
#include "PipelineOptionsFetal.h"
#include "GlobalEnums.h"
#include "PipelineApp.h"
#include "ConfigOptions.h"
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
#include <Wt/WLineEdit>
#include <Wt/WDoubleValidator>
#include <Wt/WIntValidator>
#include <Wt/WMessageBox>
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
PipelineOptionsFetal::PipelineOptionsFetal(WContainerWidget *parent) :
    PipelineOptions(parent)
{
    setStyleClass("tabdiv");

    mStageBoxes[0] = new WCheckBox("Perform fetal extraction");

    for (int i = 0; i < NUM_OPTIONAL_FETAL_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
        mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    WGroupBox *fetalExtractionBox = new WGroupBox("Fetal Extraction");
    fetalExtractionBox->setStyleClass("optiongroupdiv");
    WGridLayout *fetalLayout = new WGridLayout();
    WText *sliceText = new WText("Slice selection margin for extraction: ");
    WText *marginText = new WText("Margin in voxels around extracted region: ");
    WText *cirumferenceText = new WText("Head circumference (in cm): ");


    mSliceSelectionEdit = new WLineEdit("");
    mSliceSelectionEdit->setToolTip("Specifies the slice from which to perform the region extraction. "
                                    "For example, for a volume of 50 slices, a value of 0.5 means the 25th[0.5*50] slice.");
    WDoubleValidator *sliceValidator = new WDoubleValidator();
    sliceValidator->setRange(0.0, 1.0);
    sliceValidator->setMandatory(true);
    mSliceSelectionEdit->setValidator(sliceValidator);

    mMarginEdit = new WLineEdit("");
    mMarginEdit->setToolTip("Specifies the number of voxels on each side of the extraction region to pad.");
    WIntValidator *marginValidator = new WIntValidator();
    marginValidator->setRange(0, 1000);
    marginValidator->setMandatory(true);
    mMarginEdit->setValidator(marginValidator);


    mHeadCircumferenceEdit = new WLineEdit("");
    mHeadCircumferenceEdit->setToolTip("The assumed head circumference in cm, the default value of 25 is based on the mean for GA 27 weeks.");
    WDoubleValidator *headValidator = new WDoubleValidator();
    headValidator->setRange(0.0, 100.0);
    headValidator->setMandatory(true);
    mHeadCircumferenceEdit->setValidator(headValidator);

    fetalLayout->addWidget(sliceText, 0, 0, AlignRight);
    fetalLayout->addWidget(mSliceSelectionEdit, 0, 1, AlignLeft);

    fetalLayout->addWidget(marginText, 1, 0, AlignRight);
    fetalLayout->addWidget(mMarginEdit, 1, 1, AlignLeft);

    fetalLayout->addWidget(cirumferenceText, 2, 0, AlignRight);
    fetalLayout->addWidget(mHeadCircumferenceEdit, 2, 1, AlignLeft);

    fetalExtractionBox->setLayout(fetalLayout, AlignTop);

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addWidget(fetalExtractionBox, 1, 0);
    mPipelineOptionsBoxLayout->setRowStretch(0, -1);
    mPipelineOptionsBoxLayout->setRowStretch(1, 1);

    resetAll();
}

///
//  Destructor
//
PipelineOptionsFetal::~PipelineOptionsFetal()
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
void PipelineOptionsFetal::resetAll()
{
    PipelineOptions::resetAll();

    for (int i = 0; i < NUM_OPTIONAL_FETAL_STAGES; i++)
    {
       mStageBoxes[i]->setChecked(true);
    }

    mSliceSelectionEdit->setText("0.5");
    mMarginEdit->setText("5");
    mHeadCircumferenceEdit->setText("25");
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsFetal::getCommandLineString() const
{
    std::string args;
    ostringstream stageStrStream;

    for (int i = 1; i < FIRST_OPTIONAL_FETAL_STAGE; i++)
    {
        stageStrStream << i;
    }

    for (int i = 0; i < NUM_OPTIONAL_FETAL_STAGES; i++)
    {
        if (mStageBoxes[i]->isChecked())
        {
            stageStrStream << (i + FIRST_OPTIONAL_FETAL_STAGE);
        }
    }

    args = "-t " + stageStrStream.str();;

    if (getConfigOptionsPtr()->GetRemoteMatLab() != "")
    {
        args += " -r " + getConfigOptionsPtr()->GetRemoteMatLab();
    }

    args += " -s " + mSliceSelectionEdit->text().toUTF8();
    args += " -g " + mMarginEdit->text().toUTF8();
    args += " -H " + mHeadCircumferenceEdit->text().toUTF8();

    return args + " " + PipelineOptions::getCommandLineString();
}

///
//  Check whether the options are valid
//
bool PipelineOptionsFetal::validate() const
{

    if (!PipelineOptions::validate())
        return false;

    if (!mSliceSelectionEdit->validate() || mSliceSelectionEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("Slice selection value for mask must be in the range [0.0, 1.0].  Please correct it.");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }


    if (!mMarginEdit->validate() || mMarginEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("Margin in voxels must be an integer number greater than 1.  Please correct it.");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }

    if (!mHeadCircumferenceEdit->validate() || mHeadCircumferenceEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("Head circumference must be a value greater than 0.0.  Please correct it.");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


