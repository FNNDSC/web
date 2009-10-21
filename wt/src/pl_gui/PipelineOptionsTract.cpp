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


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


