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
    mDirectoryGroupBoxLayout = new WVBoxLayout();
    mDirectoryGroupBox->setLayout(mDirectoryGroupBoxLayout);

    // Output directory suffix
    WHBoxLayout *outputDirSuffixLayout = new WHBoxLayout();
    outputDirSuffixLayout->addWidget(new WLabel("Output Directory Suffix:"), Wt::AlignRight);
    mOutputDirSuffix = new WLineEdit();
    outputDirSuffixLayout->addWidget(mOutputDirSuffix, Wt::AlignLeft);

    // Output file suffix
    WHBoxLayout *outputFileSuffixLayout = new WHBoxLayout();
    outputFileSuffixLayout->addWidget(new WLabel("Output File Suffix:"), Wt::AlignRight);
    mOutputFileSuffix = new WLineEdit();
    outputFileSuffixLayout->addWidget(mOutputFileSuffix, Wt::AlignLeft);

    // E-mail user
    WHBoxLayout *emailUserLayout = new WHBoxLayout();
    emailUserLayout->addWidget(new WLabel("E-mail User:"), Wt::AlignRight);
    mEmailUser = new WLineEdit();
    emailUserLayout->addWidget(mEmailUser, Wt::AlignLeft);

    mDirectoryGroupBoxLayout->addLayout(outputDirSuffixLayout);
    mDirectoryGroupBoxLayout->addLayout(outputFileSuffixLayout);
    mDirectoryGroupBoxLayout->addLayout(emailUserLayout);

    // Pipeline options box
    mPipelineOptionsBox = new WGroupBox("Pipeline Options");
    mPipelineOptionsBox->setStyleClass("groupdiv");
    mPipelineOptionsBoxLayout = new WGridLayout();
    mPipelineOptionsBoxLayout->addWidget(mStageButtonGroup, 0, 0);
    mPipelineOptionsBoxLayout->addWidget(mDirectoryGroupBox, 1, 0);


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

    args += ConfigOptions::GetPtr()->GetClusterName();
    args += " -O " + ConfigOptions::GetPtr()->GetOutDir();

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


