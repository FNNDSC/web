//
//
//  Description:
//      This is the container widget for the pipeline configuration
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineConfigure.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>

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
PipelineConfigure::PipelineConfigure(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");
}

///
//  Destructor
//
PipelineConfigure::~PipelineConfigure()
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


///
//  Creates a title widget.
//
WText* PipelineConfigure::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

