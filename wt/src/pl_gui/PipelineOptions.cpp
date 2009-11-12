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

    mStageButtonGroup = new WGroupBox("Stages");
    mStageButtonGroup->setStyleClass("groupdiv");
    mStageButtonGroupLayout = new WVBoxLayout();
    mStageButtonGroup->setLayout(mStageButtonGroupLayout);

    mDirectoryGroupBox = new WGroupBox("Directory / File / E-mail");
    mDirectoryGroupBox->setStyleClass("groupdiv");
    mDirectoryGroupBoxLayout = new WGridLayout();
    mDirectoryGroupBox->setLayout(mDirectoryGroupBoxLayout);

    mSettingsGroupBox = new WGroupBox("Settings");
    mSettingsGroupBox->setStyleClass("groupdiv");
    mSettingsGroupBoxLayout = new WGridLayout();
    mSettingsGroupBox->setLayout(mSettingsGroupBoxLayout);
    mSettingsGroupBox->hide();

    // Output directory suffix
    mDirectoryGroupBoxLayout->addWidget(new WLabel("Output Directory Suffix (Optional):"), 0, 0, Wt::AlignRight  | Wt::AlignMiddle);
    mOutputDirSuffix = new WLineEdit();
    mOutputDirSuffix->setMinimumSize(200, WLength::Auto);
    mDirectoryGroupBoxLayout->addWidget(mOutputDirSuffix, 0, 1, Wt::AlignLeft | Wt::AlignMiddle);

    // Output file suffix
    mDirectoryGroupBoxLayout->addWidget(new WLabel("Output File Suffix (Optional):"), 1, 0, Wt::AlignRight  | Wt::AlignMiddle);
    mOutputFileSuffix = new WLineEdit();
    mOutputFileSuffix->setMinimumSize(200, WLength::Auto);
    mDirectoryGroupBoxLayout->addWidget(mOutputFileSuffix, 1, 1, Wt::AlignLeft | Wt::AlignMiddle);

    // E-mail user
    mDirectoryGroupBoxLayout->addWidget(new WLabel("E-mail User (Optional):"), 2, 0, Wt::AlignRight  | Wt::AlignMiddle);
    mEmailUser = new WLineEdit();
    mEmailUser->setMinimumSize(200, WLength::Auto);
    mDirectoryGroupBoxLayout->addWidget(mEmailUser, 2, 1, Wt::AlignLeft | Wt::AlignMiddle);
    mDirectoryGroupBoxLayout->setColumnStretch(1, 1);

    // Pipeline options box
    mPipelineOptionsBox = new WGroupBox("Pipeline Options");
    mPipelineOptionsBox->setStyleClass("groupdiv");
    mPipelineOptionsBoxLayout = new WGridLayout();
    mPipelineOptionsBoxLayout->addWidget(mStageButtonGroup, 0, 0);
    mPipelineOptionsBoxLayout->addWidget(mSettingsGroupBox, 1, 0);
    mPipelineOptionsBoxLayout->addWidget(mDirectoryGroupBox, 2, 0);


    mPipelineOptionsBox->setLayout(mPipelineOptionsBoxLayout);

    layout->addWidget(mPipelineOptionsBox);
    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
PipelineOptions::~PipelineOptions()
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
void PipelineOptions::resetAll()
{
    mEmailUser->setText("");
    mOutputDirSuffix->setText("");
    mOutputFileSuffix->setText("");
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

    if (!mEmailUser->text().empty())
    {
        args += " -M " + mEmailUser->text().toUTF8();
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
    return mOutputFileSuffix->text().toUTF8();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


