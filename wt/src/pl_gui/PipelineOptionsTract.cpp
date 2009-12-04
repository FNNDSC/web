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
#include <Wt/WComboBox>
#include <Wt/WDoubleValidator>
#include <Wt/WLineEdit>
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

    // Settings box configuration
    mSettingsGroupBox->show();

    WGridLayout *comboBoxLayout = new WGridLayout();

    // Reconstruction algorithm
    mReconAlgorithmComboBox = new WComboBox();
    mReconAlgorithmComboBox->addItem("FACT");
    mReconAlgorithmComboBox->addItem("2nd Order Runga-Kutta");
    comboBoxLayout->addWidget(new WLabel("Reconstruction Algorithm:"), 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    comboBoxLayout->addWidget(mReconAlgorithmComboBox, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);

    // Image model
    mImageModelComboBox = new WComboBox();
    mImageModelComboBox->addItem("DTI");
    mImageModelComboBox->addItem("HARDI");
    comboBoxLayout->addWidget(new WLabel("Image Model:"), 1, 0, Wt::AlignRight | Wt::AlignMiddle);
    comboBoxLayout->addWidget(mImageModelComboBox, 1, 1, Wt::AlignLeft | Wt::AlignMiddle);
    comboBoxLayout->setColumnStretch(1, 1);

    mEddyCurrentCheckBox = new WCheckBox("Perform Eddy Current Correction (ECC)");
    mSettingsGroupBoxLayout->addWidget(mEddyCurrentCheckBox, 0, 0, Wt::AlignMiddle);
    mFAVolumeMaskCheckBox = new WCheckBox("Use FA volume as mask filter?");
    mSettingsGroupBoxLayout->addWidget(mFAVolumeMaskCheckBox, 1, 0, Wt::AlignMiddle);

    WGridLayout *lineEditLayout = new WGridLayout();
    mFAThresholdLineEdit = new WLineEdit("0.0");
    WDoubleValidator *validator = new WDoubleValidator();
    validator->setRange(0.0, 1.0);
    mFAThresholdLineEdit->setValidator(validator);

    mFAThresholdLabel = new WLabel("Lower threshold value for mask:");
    lineEditLayout->addWidget(mFAThresholdLabel, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    lineEditLayout->addWidget(mFAThresholdLineEdit, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    lineEditLayout->setColumnStretch(1, 1);
    mSettingsGroupBoxLayout->addLayout(lineEditLayout, 2, 0, Wt::AlignMiddle);

    mSettingsGroupBoxLayout->addLayout(comboBoxLayout, 3, 0);
    mSettingsGroupBoxLayout->setRowStretch(3, 1);

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addWidget(mSettingsGroupBox, 1, 0);
    mPipelineOptionsBoxLayout->addWidget(mDirectoryGroupBox, 2, 0);

    // Connection
    mFAVolumeMaskCheckBox->clicked().connect(SLOT(this, PipelineOptionsTract::volumeMaskClicked));

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

    mEddyCurrentCheckBox->setChecked(false);
    mFAVolumeMaskCheckBox->setChecked(false);
    mFAThresholdLineEdit->setText("0.0");
    mFAThresholdLineEdit->disable();
    mFAThresholdLabel->disable();

}

///
//  Check whether the options are valid
//
bool PipelineOptionsTract::validate() const
{
    if (mFAVolumeMaskCheckBox->isChecked())
    {
        if (!mFAThresholdLineEdit->validate())
        {
            WMessageBox::show("Invalid Input",
                              "Threshold value for mask must be in the range [0.0, 1.0].  Please correct it.",
                              Wt::Ok);

            return false;
        }
    }

    return true;
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

    // Skip eddy current correction (-k)
    if (!mEddyCurrentCheckBox->isChecked())
    {
        args += " -k";
    }

    // Non-default recon algorithm (-A <reconAlg>)
    if (mReconAlgorithmComboBox->currentIndex() == 1)
    {
        args += " -A rk2";
    }

    // Non-default image model (-I <imageModel>)
    if (mReconAlgorithmComboBox->currentIndex() == 1)
    {
        args += " -I hardi";
    }

    if (mFAVolumeMaskCheckBox->isChecked())
    {
        args += " -F " + mFAThresholdLineEdit->text().toUTF8();
    }

    return args + " " + PipelineOptions::getCommandLineString();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  FA Volume mask checkbox clicked [slot]
//
void PipelineOptionsTract::volumeMaskClicked()
{
    if (mFAVolumeMaskCheckBox->isChecked())
    {
        mFAThresholdLineEdit->enable();
        mFAThresholdLabel->enable();
    }
    else
    {
        mFAThresholdLineEdit->disable();
        mFAThresholdLabel->disable();
    }
}
