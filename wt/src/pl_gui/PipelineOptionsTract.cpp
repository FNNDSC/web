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
#include "PipelineApp.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
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
#include <Wt/WFileUpload>
#include <Wt/WDate>
#include <boost/filesystem.hpp>
#include <vector>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;

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

    WGroupBox *processingBox = new WGroupBox("Processing");
    processingBox->setStyleClass("optiongroupdiv");
    WGridLayout *processingBoxLayout = new WGridLayout();

    mEddyCurrentCheckBox = new WCheckBox("Perform Eddy Current Correction (ECC)");
    mFAVolumeMaskCheckBox = new WCheckBox("Use FA volume as mask filter?");


    WGridLayout *lineEditLayout = new WGridLayout();
    mFAThresholdLineEdit = new WLineEdit("0.0");
    WDoubleValidator *validator = new WDoubleValidator();
    validator->setRange(0.0, 1.0);
    mFAThresholdLineEdit->setValidator(validator);

    mFAThresholdLineEdit->setToolTip("Enter a lower threshold value between 0.0 and 1.0. A value of 0.0 will use the entire FA volume.");
    mFAThresholdLabel = new WLabel("Lower threshold value for mask:");
    lineEditLayout->addWidget(mFAThresholdLabel, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    lineEditLayout->addWidget(mFAThresholdLineEdit, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    lineEditLayout->setColumnStretch(1, 1);

    WVBoxLayout *vboxFALayout = new WVBoxLayout();
    vboxFALayout->addWidget(mFAVolumeMaskCheckBox);
    vboxFALayout->addLayout(lineEditLayout);

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


    // Processing group box layout
    processingBoxLayout->addWidget(mEddyCurrentCheckBox, 0, 0, Wt::AlignMiddle);
    processingBoxLayout->addLayout(vboxFALayout, 1, 0, Wt::AlignMiddle);
    processingBoxLayout->addLayout(comboBoxLayout, 2, 0, Wt::AlignTop);
    processingBox->setLayout(processingBoxLayout);

    WGroupBox *gradientTableBox = new WGroupBox("Gradient Table");
    gradientTableBox->setStyleClass("optiongroupdiv");
    WGridLayout *gradientLayout = new WGridLayout();

    // Gradient File
    mGradientFileCheckBox = new WCheckBox("Manually upload gradient file?");
    mGradientFileUpload = new WFileUpload();
    gradientLayout->addWidget(mGradientFileCheckBox, 0, 0, Wt::AlignMiddle);
    gradientLayout->addWidget(mGradientFileUpload, 1, 0, Wt::AlignMiddle);
    gradientTableBox->setLayout(gradientLayout);

    WVBoxLayout *rightSettingsLayout = new WVBoxLayout();
    rightSettingsLayout->addWidget(gradientTableBox);
    rightSettingsLayout->addStretch();

    WGridLayout *settingsGridLayout = new WGridLayout();
    settingsGridLayout->addWidget(processingBox, 0, 0);
    settingsGridLayout->addLayout(rightSettingsLayout, 0, 1);
    settingsGridLayout->setRowStretch(0, -1);

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addLayout(settingsGridLayout, 1, 0);
    mPipelineOptionsBoxLayout->addWidget(mDirectoryGroupBox, 2, 0);
    mPipelineOptionsBoxLayout->setRowStretch(0, -1);
    mPipelineOptionsBoxLayout->setRowStretch(1, 1);
    mPipelineOptionsBoxLayout->setRowStretch(2, -1);

    // Connection
    mFAVolumeMaskCheckBox->clicked().connect(SLOT(this, PipelineOptionsTract::volumeMaskClicked));
    mGradientFileUpload->changed().connect(SLOT(mGradientFileUpload, WFileUpload::upload));
    mGradientFileUpload->uploaded().connect(SLOT(this, PipelineOptionsTract::fileUploaded));

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
    mGradientFileCheckBox->setChecked(false);
    mGradientServerFile = "";
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

    if (mGradientFileCheckBox->isChecked())
    {
        if (mGradientServerFile == "")
        {
            WMessageBox::show("Gradient File",
                              "If you want to manually provide a gradient file, you must upload it.  Please upload the file first.",
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

    if (mGradientFileCheckBox->isChecked())
    {
        args += " -g \"" + mGradientServerFile + "\"";
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

///
//  Gradient file uploaded
//
void PipelineOptionsTract::fileUploaded()
{
    if (!mGradientFileUpload->emptyFileName())
    {
        // Check the upload check box if it is not already checked
        mGradientFileCheckBox->setChecked(true);

        // Grab the file from the upload widget
        const WString& clientFileName = mGradientFileUpload->clientFileName();
        const std::string& spoolFileName = mGradientFileUpload->spoolFileName();
        mGradientFileUpload->stealSpooledFile();

        // Copy it out to our directory for storing gradient table files
        std::string gradientFileDir = getConfigOptionsPtr()->GetOutGradientDir();
        std::string gradientFile;

        // Create unique identifier for gradient file based on date/time
        time_t curTime;
        tm *t;
        time( &curTime );
        t = localtime( &curTime );

        std::string curDate = (WDate::currentDate().toString("yyyyMMdd") +
                               WString("-{1}{2}{3}").arg(t->tm_hour).arg(t->tm_min).arg(t->tm_sec)).toUTF8();

        gradientFileDir = gradientFileDir + "/" + curDate;
        try
        {
            // Create the directory
            create_directory(path(gradientFileDir));

            // Copy the file
            gradientFile = gradientFileDir + "/" + clientFileName.toUTF8();
            copy_file(spoolFileName, gradientFile);

            // Remove the uploaded copy
            remove(spoolFileName);
        }
        catch(...)
        {
            WApplication::instance()->log("error") << "Error copying gradient file " << spoolFileName << " to " << gradientFile;
            mGradientServerFile = "";
            return;
        }

        mGradientServerFile = gradientFile;
    }
}
