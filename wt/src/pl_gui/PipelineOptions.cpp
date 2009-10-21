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


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


