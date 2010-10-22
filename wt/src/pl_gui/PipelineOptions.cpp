//
//
//  Description:
//      This is the base widget for pipeline options (tract, fs, etc.).  It contains
//      the options which are common amongst all pipelines.  The other options are
//      in sub-classed objects.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "PipelineOptions.h"
#include "ConfigOptions.h"
#include "GlobalEnums.h"
#include <Wt/WContainerWidget>
#include <Wt/WLogger>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WButtonGroup>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WCheckBox>
#include <Wt/WRegExpValidator>
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
PipelineOptions::PipelineOptions(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WVBoxLayout *layout = new WVBoxLayout();

    mStageButtonGroup = new WGroupBox("Optional Stages");
    mStageButtonGroup->setStyleClass("groupdiv");
    mStageButtonGroupLayout = new WVBoxLayout();
    mStageButtonGroup->setLayout(mStageButtonGroupLayout, AlignTop);

    mDirectoryGroupBox = new WGroupBox("Directory / E-mail");
    mDirectoryGroupBox->setStyleClass("groupdiv");
    mDirectoryGroupBoxLayout = new WGridLayout();
    mDirectoryGroupBox->setLayout(mDirectoryGroupBoxLayout, AlignTop);

    // Output directory suffix
    WRegExpValidator *dirSuffixValidator = new WRegExpValidator("[a-zA-Z0-9_+-]+");
    dirSuffixValidator->setMandatory(true);
    mOutputDirSuffixLabel = new WLabel("Directory Suffix:");
    mDirectoryGroupBoxLayout->addWidget(mOutputDirSuffixLabel, 0, 0, Wt::AlignRight  | Wt::AlignMiddle);
    mOutputDirSuffix = new WLineEdit("");
    mOutputDirSuffix->setValidator(dirSuffixValidator);
    mOutputDirSuffix->setToolTip("This string will be appended to the directory name that is output for the scans.");
    mOutputDirSuffix->setMinimumSize(200, WLength::Auto);
    mOutputDirSuffix->resize(200, WLength::Auto);
    mDirectoryGroupBoxLayout->addWidget(mOutputDirSuffix, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);

    // E-mail user
    mEmailCheckBox = new WCheckBox("E-mail User:");
    mEmailCheckBox->setToolTip("If checked, the specified E-mail address will receive an E-mail when each cluster job finishes.");
    mDirectoryGroupBoxLayout->addWidget(mEmailCheckBox, 1, 0, Wt::AlignRight  | Wt::AlignMiddle);
    mEmailUser = new WLineEdit();
    mEmailUser->setMinimumSize(200, WLength::Auto);
    mEmailUser->resize(200, WLength::Auto);
    mDirectoryGroupBoxLayout->addWidget(mEmailUser, 1, 1, Wt::AlignLeft | Wt::AlignMiddle);
    mDirectoryGroupBoxLayout->setColumnStretch(1, 1);

    WHBoxLayout *topHBox = new WHBoxLayout();
    topHBox->addWidget(mDirectoryGroupBox);
    topHBox->addWidget(mStageButtonGroup);

    // Pipeline options box
    mPipelineOptionsBox = new WGroupBox("Pipeline Options");
    mPipelineOptionsBox->setStyleClass("pipelinediv");
    mPipelineOptionsBoxLayout = new WGridLayout();
    mPipelineOptionsBoxLayout->addLayout(topHBox, 0, 0);

    mPipelineOptionsBox->setLayout(mPipelineOptionsBoxLayout, AlignTop);

    /// Create message box with no parent
    mMessageBox = new WMessageBox();
    mMessageBox->buttonClicked().connect(SLOT(this, PipelineOptions::handleMessageBoxFinished));

    layout->addWidget(mPipelineOptionsBox);
    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
PipelineOptions::~PipelineOptions()
{
    delete mMessageBox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
//  Reset to default state
//
void PipelineOptions::resetAll()
{
    std::string  currentEmail = getCurrentUserEmail();

    mEmailCheckBox->setChecked(true);
    mEmailUser->setText(currentEmail);
    mOutputDirSuffix->setText("");
}

///
// Check whether the options are valid
//
bool PipelineOptions::validate() const
{
    if (mOutputDirSuffix->text().empty() || !mOutputDirSuffix->validate())
    {
        mMessageBox->setWindowTitle("Invalid Input");
        mMessageBox->setText("You must enter a directory suffix.  It can contain any combination of the following characters (A-Z, a-z, 0-9, _, +, -).");
        mMessageBox->setButtons(Wt::Ok);
        mMessageBox->show();
        return false;
    }

    return true;
}


///
//  Generate command-line options string based on user choices
//
std::string PipelineOptions::getCommandLineString() const
{
    // Default args: -v 10 (verbosity: 10)
    //               -c    (run on cluster)
    //               -C <clusterName> (cluster name from config file)
    //               -O <outputDir>   (output directory from config file)
    //
    std::string args = "-v 10 -c -C ";

    args += getConfigOptionsPtr()->GetClusterName();
    args += " -O " + getConfigOptionsPtr()->GetOutDir();
    args += " -n " + getCurrentUserName();
    if (mEmailCheckBox->isChecked() && !mEmailUser->text().empty())
    {
        args += " -M " + mEmailUser->text().toUTF8();
    }
    if (getConfigOptionsPtr()->GetAnalysisDir() != "")
    {
        args += " --migrate-analysis " + getConfigOptionsPtr()->GetAnalysisDir();
    }


    return args;
}

///
//  Get the current directory suffix
//
std::string PipelineOptions::getOutputDirSuffix() const
{
    return mOutputDirSuffix->text().toUTF8();
}

///
//  Get the current file suffix
//
std::string PipelineOptions::getOutputFileSuffix() const
{
    return mOutputDirSuffix->text().toUTF8();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Handle message box finished [slot]
//
void PipelineOptions::handleMessageBoxFinished(StandardButton)
{
    mMessageBox->hide();
}



