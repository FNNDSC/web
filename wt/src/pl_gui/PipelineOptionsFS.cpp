//
//
//  Description:
//      This is the widget for Freesurfer pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineOptionsFS.h"
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
PipelineOptionsFS::PipelineOptionsFS(WContainerWidget *parent) :
    PipelineOptions(parent)
{
    setStyleClass("tabdiv");

 }

///
//  Destructor
//
PipelineOptionsFS::~PipelineOptionsFS()
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


