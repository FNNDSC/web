//
//
//  Description:
//      This is the widget for DICOM Send pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineOptionsDcmSend.h"
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
PipelineOptionsDcmSend::PipelineOptionsDcmSend(WContainerWidget *parent) :
    PipelineOptions(parent)
{
    setStyleClass("tabdiv");

    mStageBoxes[0] = new WCheckBox("Anonymize Patient Data");

    for (int i = 0; i < NUM_OPTIONAL_DCMSEND_STAGES; i++)
    {
       mStageBoxes[i]->setChecked(true);
       mStageButtonGroupLayout->addWidget(mStageBoxes[i]);
    }

    mAnonymizeComboBox = new WComboBox();
    mAnonymizeComboBox->addItem("Anonymize Patient ID, Name, BirthDate, Study");
    mAnonymizeComboBox->addItem("Full DICOM PS 3.15 Anonymize");
    mStageButtonGroupLayout->addWidget(mAnonymizeComboBox);


    WGroupBox *dcmSendExtractionBox = new WGroupBox("DICOM Send to Remote PACS");
    dcmSendExtractionBox->setStyleClass("optiongroupdiv");
    WGridLayout *dcmSendLayout = new WGridLayout();
    WText *hostNameText = new WText("PACS Hostname or IP Address:");
    WText *portText = new WText("PACS TCP Port Number:");
    WText *aeTitleText = new WText("Application Entity (AE) Title:");

    mHostNameEdit = new WLineEdit("");
    mHostNameEdit->setMinimumSize(250, WLength::Auto);
    mHostNameEdit->resize(250, WLength::Auto);
    mPortEdit = new WLineEdit("");
    mPortEdit->setMinimumSize(250, WLength::Auto);
    mPortEdit->resize(250, WLength::Auto);
    mAETitleEdit = new WLineEdit("");
    mAETitleEdit->setMinimumSize(250, WLength::Auto);
    mAETitleEdit->resize(250, WLength::Auto);

    dcmSendLayout->addWidget(hostNameText, 0, 0, AlignRight);
    dcmSendLayout->addWidget(mHostNameEdit, 0, 1, AlignLeft);

    dcmSendLayout->addWidget(portText, 1, 0, AlignRight);
    dcmSendLayout->addWidget(mPortEdit, 1, 1, AlignLeft);

    dcmSendLayout->addWidget(aeTitleText, 2, 0, AlignRight);
    dcmSendLayout->addWidget(mAETitleEdit, 2, 1, AlignLeft);
    dcmSendLayout->setColumnStretch(1, 1);

    dcmSendExtractionBox->setLayout(dcmSendLayout, AlignTop);

    // Add to the base class layout
    mPipelineOptionsBoxLayout->addWidget(dcmSendExtractionBox, 1, 0);

    mStageBoxes[0]->clicked().connect(SLOT(this, PipelineOptionsDcmSend::anonymizeClicked));

    resetAll();
}

///
//  Destructor
//
PipelineOptionsDcmSend::~PipelineOptionsDcmSend()
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
void PipelineOptionsDcmSend::resetAll()
{
    PipelineOptions::resetAll();

    for (int i = 0; i < NUM_OPTIONAL_DCMSEND_STAGES; i++)
    {
       mStageBoxes[i]->setChecked(true);
    }

    mHostNameEdit->setText("");
    mPortEdit->setText("");
    mAETitleEdit->setText("");

    // Do not show Directory suffix
    mOutputDirSuffix->disable();
    mOutputDirSuffix->setText("NA");
    mOutputDirSuffixLabel->disable();

    mAnonymizeComboBox->setCurrentIndex(0);
    mAnonymizeComboBox->enable();
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsDcmSend::getCommandLineString() const
{
    std::string args;

    // Check if "Anonymize" is set
    if (mStageBoxes[0]->isChecked())
    {
        // Partial anonymize
        if (mAnonymizeComboBox->currentIndex() == 0)
        {
            args += " -P ";
        }
        // Full anonymize
        else
        {
            args += " -A -K " + getConfigOptionsPtr()->GetAnonCertificate();
        }
    }

    args += " -h " + mHostNameEdit->text().toUTF8();
    args += " -p " + mPortEdit->text().toUTF8();
    args += " -a " + mAETitleEdit->text().toUTF8();

    return args + " " + PipelineOptions::getCommandLineString();
}

///
//  Check whether the options are valid
//
bool PipelineOptionsDcmSend::validate() const
{

    if (!PipelineOptions::validate())
        return false;

    if (mHostNameEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("PACS Hostname must be specified.  Please correct it.");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }


    if (mPortEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("PACS TCP Port Number must be specified.  Please correct it.");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }

    if (mAETitleEdit->text().empty())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("Application Entity (AE) Title must be specified.  Please correct it.");
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

///
//  Anonymize checkbox clicked [slot]
//
void PipelineOptionsDcmSend::anonymizeClicked()
{
    if (mStageBoxes[0]->isChecked())
    {
        mAnonymizeComboBox->enable();
    }
    else
    {
        mAnonymizeComboBox->disable();
    }
}


