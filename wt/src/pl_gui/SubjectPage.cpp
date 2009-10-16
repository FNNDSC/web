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
#include "MRIBrowser.h"
#include "ScanBrowser.h"
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WLabel>

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
    layout->addWidget(createTitle("MRIDs"), 0, 0);
    layout->addWidget(createTitle("Info"), 0, 1);

    mMRIBrowser = new MRIBrowser();
    mScanBrowser = new ScanBrowser();

    layout->addWidget(mMRIBrowser, 1, 0);
    layout->addWidget(mScanBrowser, 1, 1);

    // Let row 1 and column 2 take the excess space.
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(2, 1);

    mScanBrowser->hide();

    setLayout(layout);

    mMRIBrowser->mriSelected().connect(SLOT(this, SubjectPage::mriChanged));
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
//  Slot for when the MRI selection changes
//
void SubjectPage::mriChanged(std::string scanDir)
{
    mScanBrowser->setScanDir(scanDir);
    mScanBrowser->show();
}
