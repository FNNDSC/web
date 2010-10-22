//
//
//  Description:
//      This is the widget for connectome pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineOptionsConnectome.h"
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
PipelineOptionsConnectome::PipelineOptionsConnectome(WContainerWidget *parent) :
    PipelineOptionsTract(parent)
{

    mTalairachCheckBox = new WCheckBox("Perform Talairach registration");
    mStageButtonGroupLayout->addWidget(mTalairachCheckBox);

    resetAll();
}

///
//  Destructor
//
PipelineOptionsConnectome::~PipelineOptionsConnectome()
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
void PipelineOptionsConnectome::resetAll()
{
    PipelineOptionsTract::resetAll();
    mTalairachCheckBox->setChecked(false);
}

///
//  Check whether the options are valid
//
bool PipelineOptionsConnectome::validate() const
{
    // Validate the base class first
    if (!PipelineOptionsTract::validate())
        return false;


    return true;
}

///
//  Generate command-line options string based on user choices
//
std::string PipelineOptionsConnectome::getCommandLineString() const
{
    std::string args = PipelineOptionsTract::getCommandLineString();

    // Transform "-t" into "-tract-meta-stages" which is the
    // argument required for connectome_meta.bash
    std::string stages = "-t ";
    size_t found = args.find(stages);
    if (found != string::npos)
    {
        args.replace(found, stages.size(), "--tract-meta-stages ");
    }

    if (!mTalairachCheckBox->isChecked())
    {
        args += " -F -notalairach ";
    }

    return args;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//
