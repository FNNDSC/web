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
#include <Wt/WIntValidator>
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


    mStageBoxes[0] = new WCheckBox("Slice 3D model along anatomical planes");
#if 0 // Temporarily disabled until we get MatLAB
    mStageBoxes[1] = new WCheckBox("Convert slices to DICOM");
    mStageBoxes[2] = new WCheckBox("Transmit slices to PACS");
#endif
    for (int i = 0; i < NUM_OPTIONAL_TRACT_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
        mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    WGroupBox *processingBox = new WGroupBox("Tractography");
    processingBox->setStyleClass("optiongroupdiv");
    WVBoxLayout *processingBoxLayout = new WVBoxLayout();

    mEddyCurrentCheckBox = new WCheckBox("Perform Eddy Current Correction (ECC)");
    mVolumeMaskCheckBox = new WCheckBox("");
    mVolumeMaskComboBox = new WComboBox();
    mVolumeMaskComboBox->addItem("FA");
    mVolumeMaskComboBox->addItem("ADC");
    mVolumeMaskComboBox->setToolTip("Set the volume (FA or ADC) that will be used as a mask filter.");
    WText *useText =  new WText("Use");
    WText *maskText =  new WText("volume as mask filter?");
    maskText->setWordWrap(false);

    WGridLayout *volumeMaskLayout = new WGridLayout();
    volumeMaskLayout->addWidget(mVolumeMaskCheckBox, 0, 0, AlignMiddle);
    volumeMaskLayout->addWidget(useText, 0, 1, AlignMiddle);
    volumeMaskLayout->addWidget(mVolumeMaskComboBox, 0, 2, AlignMiddle);
    volumeMaskLayout->addWidget(maskText, 0, 3, AlignMiddle);
    volumeMaskLayout->setColumnStretch(4, 1);

    mLowerThresholdLineEdit = new WLineEdit("0.0");
    mUpperThresholdLineEdit = new WLineEdit("1.0");
    WDoubleValidator *validator = new WDoubleValidator();
    validator->setRange(0.0, 1.0);
    mLowerThresholdLineEdit->setValidator(validator);
    mUpperThresholdLineEdit->setValidator(validator);

    mLowerThresholdLineEdit->setToolTip("Enter a lower threshold value between 0.0 and 1.0. A value of 0.0 will use the entire mask volume.");
    mUpperThresholdLineEdit->setToolTip("Enter an upper threshold value between 0.0 and 1.0. A value of 1.0 will use the entire mask volume.");
    mLowerThresholdLabel = new WLabel("Lower threshold value for mask:");
    mUpperThresholdLabel = new WLabel("Upper threshold value for mask:");

    WGridLayout *lowerThresholdLayout = new WGridLayout();
    WGridLayout *upperThresholdLayout = new WGridLayout();
    lowerThresholdLayout->addWidget(mLowerThresholdLabel, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    lowerThresholdLayout->addWidget(mLowerThresholdLineEdit, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    upperThresholdLayout->addWidget(mUpperThresholdLabel, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    upperThresholdLayout->addWidget(mUpperThresholdLineEdit, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);

    lowerThresholdLayout->setColumnStretch(1, 1);
    upperThresholdLayout->setColumnStretch(1, 1);

    WVBoxLayout *vboxThresholdLayout = new WVBoxLayout();
    vboxThresholdLayout->addLayout(volumeMaskLayout);
    vboxThresholdLayout->addLayout(lowerThresholdLayout);
    vboxThresholdLayout->addLayout(upperThresholdLayout);
    vboxThresholdLayout->addSpacing(WLength(20.0, WLength::Pixel));

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
    processingBoxLayout->addWidget(mEddyCurrentCheckBox);
    processingBoxLayout->addSpacing(WLength(20.0, WLength::Pixel));
    processingBoxLayout->addLayout(vboxThresholdLayout);
    processingBoxLayout->addLayout(comboBoxLayout);
    processingBox->setLayout(processingBoxLayout, AlignTop);

    WGroupBox *gradientTableBox = new WGroupBox("Gradients");
    gradientTableBox->setStyleClass("optiongroupdiv");
    WVBoxLayout *gradientLayout = new WVBoxLayout();

    // Gradient File
    mGradientFileCheckBox = new WCheckBox("Manually upload gradient file?");
    mGradientFileUpload = new WFileUpload();

    mB0VolumesCheckBox = new WCheckBox("Manually specify the number of B0 volumes?");
    WGridLayout *b0LineEditLayout = new WGridLayout();
    mB0VolumesLineEdit = new WLineEdit("1");
    WIntValidator *intValidator = new WIntValidator();
    intValidator->setRange(1, 1000);
    mB0VolumesLineEdit->setValidator(intValidator);
    mB0VolumesLineEdit->setToolTip("Enter the number of B0 volumes.  This must match the number of B0 volumes in the MRI.");
    mB0VolumesLabel = new WLabel("Number of B0 volumes:");
    b0LineEditLayout->addWidget(mB0VolumesLabel, 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    b0LineEditLayout->addWidget(mB0VolumesLineEdit, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    b0LineEditLayout->setColumnStretch(1, 1);
    mGradientInvertX = new WCheckBox("X");
    mGradientInvertY = new WCheckBox("Y");
    mGradientInvertZ = new WCheckBox("Z");

    WVBoxLayout *vboxB0Layout = new WVBoxLayout();
    vboxB0Layout->addWidget(mB0VolumesCheckBox);
    vboxB0Layout->addLayout(b0LineEditLayout);
    vboxB0Layout->addSpacing(WLength(20.0, WLength::Pixel));

    WVBoxLayout *uploadLayout = new WVBoxLayout();
    uploadLayout->addWidget(mGradientFileCheckBox);
    uploadLayout->addWidget(mGradientFileUpload);
    uploadLayout->addSpacing(WLength(20.0, WLength::Pixel));

    WHBoxLayout *invertCheckBoxes = new WHBoxLayout;
    invertCheckBoxes->addWidget(mGradientInvertX);
    invertCheckBoxes->addWidget(mGradientInvertY);
    invertCheckBoxes->addWidget(mGradientInvertZ);

    WGridLayout *invertLayout = new WGridLayout();
    invertLayout->addWidget(new WLabel("Invert Gradients: "), 0, 0, Wt::AlignRight | Wt::AlignMiddle);
    invertLayout->addLayout(invertCheckBoxes, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);
    invertLayout->setColumnStretch(1, 1);

    gradientLayout->addLayout(uploadLayout);
    gradientLayout->addLayout(vboxB0Layout);
    gradientLayout->addLayout(invertLayout);
    gradientTableBox->setLayout(gradientLayout, AlignTop);

    WVBoxLayout *rightSettingsLayout = new WVBoxLayout();
    rightSettingsLayout->addWidget(gradientTableBox);

    WHBoxLayout *settingsHBoxLayout = new WHBoxLayout();
    settingsHBoxLayout->addWidget(processingBox);
    settingsHBoxLayout->addLayout(rightSettingsLayout);

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addLayout(settingsHBoxLayout, 1, 0);
    mPipelineOptionsBoxLayout->setRowStretch(0, -1);
    mPipelineOptionsBoxLayout->setRowStretch(1, 1);


    // Connection
    mVolumeMaskCheckBox->clicked().connect(SLOT(this, PipelineOptionsTract::volumeMaskClicked));
    mB0VolumesCheckBox->clicked().connect(SLOT(this, PipelineOptionsTract::b0VolumeClicked));
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
    for (int i = 0; i < NUM_OPTIONAL_TRACT_STAGES; i++)
    {
        mStageBoxes[i]->setChecked(true);
    }

    mEddyCurrentCheckBox->setChecked(false);
    mVolumeMaskCheckBox->setChecked(false);
    mVolumeMaskComboBox->setCurrentIndex(0);
    mLowerThresholdLineEdit->setText("0.0");
    mLowerThresholdLineEdit->disable();
    mLowerThresholdLabel->disable();
    mUpperThresholdLineEdit->setText("1.0");
    mUpperThresholdLineEdit->disable();
    mUpperThresholdLabel->disable();

    mB0VolumesCheckBox->setChecked(false);
    mB0VolumesLineEdit->setText("1");
    mB0VolumesLineEdit->disable();
    mB0VolumesLabel->disable();
    mGradientFileCheckBox->setChecked(false);
    mGradientServerFile = "";
    mGradientInvertX->setChecked(false);
    mGradientInvertY->setChecked(false);
    mGradientInvertZ->setChecked(false);
}

///
//  Check whether the options are valid
//
bool PipelineOptionsTract::validate() const
{
    // Validate the base class first
    if (!PipelineOptions::validate())
        return false;

    if (mVolumeMaskCheckBox->isChecked())
    {
        if (!mLowerThresholdLineEdit->validate())
        {
            mMessageBox->setWindowTitle("Invalid Input");
            mMessageBox->setText("Lower threshold value for mask must be in the range [0.0, 1.0].  Please correct it.");
            mMessageBox->setButtons(Wt::Ok);
            mMessageBox->show();
            return false;
        }

        if (!mUpperThresholdLineEdit->validate())
        {
            mMessageBox->setWindowTitle("Invalid Input");
            mMessageBox->setText("Upper threshold value for mask must be in the range [0.0, 1.0].  Please correct it.");
            mMessageBox->setButtons(Wt::Ok);
            mMessageBox->show();
            return false;
        }
    }

    if (mB0VolumesCheckBox->isChecked())
    {
        if (!mB0VolumesLineEdit->validate())
        {
            mMessageBox->setWindowTitle("Invalid Input");
            mMessageBox->setText("Number of B0 volumes must be an integer number greater than 1.  Please correct it.");
            mMessageBox->setButtons(Wt::Ok);
            mMessageBox->show();
            return false;
        }
    }

    if (mGradientFileCheckBox->isChecked())
    {
        if (mGradientServerFile == "")
        {
            mMessageBox->setWindowTitle("Gradient File");
            mMessageBox->setText("If you want to manually provide a gradient file, you must upload it.  Please upload the file first.");
            mMessageBox->setButtons(Wt::Ok);
            mMessageBox->show();
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
    ostringstream stageStrStream;

    std::string args;

    for (int i = 1; i < FIRST_OPTIONAL_TRACT_STAGE; i++)
    {
        stageStrStream << i;
    }

    for (int i = 0; i < NUM_OPTIONAL_TRACT_STAGES; i++)
    {
        if (mStageBoxes[i]->isChecked())
        {
            stageStrStream << (i + FIRST_OPTIONAL_TRACT_STAGE);
        }
    }

    args = "-t " + stageStrStream.str();

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
    if (mImageModelComboBox->currentIndex() == 1)
    {
        args += " -I hardi";
    }

    if (mVolumeMaskCheckBox->isChecked())
    {
        args += " -i " + mVolumeMaskComboBox->currentText().toUTF8();
        args += " -F " + mLowerThresholdLineEdit->text().toUTF8();
        args += " -u " + mUpperThresholdLineEdit->text().toUTF8();
    }

    if (mGradientFileCheckBox->isChecked())
    {
        args += " -g \"" + mGradientServerFile + "\"";
    }

    if (mB0VolumesCheckBox->isChecked())
    {
        args += " -B " + mB0VolumesLineEdit->text().toUTF8();
    }

    if (mGradientInvertX->isChecked())
    {
        args += " -X";
    }

    if (mGradientInvertY->isChecked())
    {
        args += " -Y";
    }

    if (mGradientInvertZ->isChecked())
    {
        args += " -Z";
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
    if (mVolumeMaskCheckBox->isChecked())
    {
        mLowerThresholdLineEdit->enable();
        mLowerThresholdLabel->enable();
        mUpperThresholdLineEdit->enable();
        mUpperThresholdLabel->enable();
    }
    else
    {
        mLowerThresholdLineEdit->disable();
        mLowerThresholdLabel->disable();
        mUpperThresholdLineEdit->disable();
        mUpperThresholdLabel->disable();
    }
}

///
//  B0 Volumes checkbox clicked [slot]
//
void PipelineOptionsTract::b0VolumeClicked()
{
    if (mB0VolumesCheckBox->isChecked())
    {
        mB0VolumesLineEdit->enable();
        mB0VolumesLabel->enable();
    }
    else
    {
        mB0VolumesLineEdit->disable();
        mB0VolumesLabel->disable();
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
        std::string clientBaseName = "uploaded_gradient_file.txt";

        gradientFileDir = gradientFileDir + "/" + curDate;
        try
        {
            // Create the directory
            create_directory(path(gradientFileDir));

            // Copy the file
            gradientFile = gradientFileDir + "/" + clientBaseName;
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


