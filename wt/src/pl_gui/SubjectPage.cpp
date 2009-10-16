//
//
//  Description:
//      This is the widget that controls and lays out the entire "Subject" page.
//      This includes the MRID browser, Scan browser, and widgets to kick off
//      a pipeline analysis.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "SubjectPage.h"
#include "SelectScans.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>

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
SubjectPage::SubjectPage(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    WGridLayout *layout = new WGridLayout();
    mSelectScans = new SelectScans();

    mStackedStage = new WStackedWidget();
    mStackedStage->addWidget(mSelectScans);

    layout->addWidget(mStackedStage, 0, 0);

    mNextButton = new WPushButton("Next ->");
    mBackButton = new WPushButton("<- Back");
    mNextButton->disable();
    mBackButton->disable();

    WHBoxLayout *navLayout = new WHBoxLayout();
    navLayout->addStretch(100);
    navLayout->addWidget(mBackButton, Wt::AlignRight);
    navLayout->addWidget(mNextButton, Wt::AlignRight);
    layout->addLayout(navLayout, 1, 0);

    // Let row 0 take the excess space
    layout->setRowStretch(0, 1);


    setLayout(layout);


    // Signal/slot connections
    mSelectScans->getScanAdded().connect(SLOT(this, SubjectPage::scanAdded));
    mNextButton->clicked().connect(SLOT(this, SubjectPage::nextClicked));
    mBackButton->clicked().connect(SLOT(this, SubjectPage::backClicked));
}

///
//  Destructor
//
SubjectPage::~SubjectPage()
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
WText* SubjectPage::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

///
//  Slot for when the scan is added to the queue
//
void SubjectPage::scanAdded(bool added)
{
    if (mSubjectState == SCAN_SELECT)
    {
        if (added)
        {
            mNextButton->enable();
        }
        else
        {
            mNextButton->disable();
        }

    }
}

///
//  Slot for when the next button is clicked
//
void SubjectPage::nextClicked()
{
    switch(mSubjectState)
    {
    // Go to config state
    case SCAN_SELECT:
        mBackButton->enable();
        mStackedStage->setCurrentIndex((int)PIPELINE_CONFIGURE);
        mSubjectState = PIPELINE_CONFIGURE;
        break;

    case PIPELINE_CONFIGURE:
    default:
        break;
    }
}


///
//  Slot for when the back button is clicked
//
void SubjectPage::backClicked()
{
    switch(mSubjectState)
    {
    case SCAN_SELECT:
        // There is no back on scan select
        break;

    case PIPELINE_CONFIGURE:
    default:
        mStackedStage->setCurrentIndex((int)SCAN_SELECT);
        mSubjectState = SCAN_SELECT;
        mBackButton->disable();
        break;
    }

}

