//
//
//  Description:
//      This is the container widget for the MRI Browser and Scan selector
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "SelectScans.h"
#include "MRIBrowser.h"
#include "ScanBrowser.h"
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
SelectScans::SelectScans(WContainerWidget *parent) :
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
    layout->setColumnStretch(1, 1);

    mScanBrowser->hide();

    setLayout(layout);

    // Signal/slot connections
    mMRIBrowser->mriSelected().connect(SLOT(this, SelectScans::mriChanged));
    mScanBrowser->scanAdded().connect(SLOT(this, SelectScans::scanAdded));
}

///
//  Destructor
//
SelectScans::~SelectScans()
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
WText* SelectScans::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

///
//  Slot for when the MRI selection changes
//
void SelectScans::mriChanged(std::string mrid, std::string scanDir, std::string age)
{
    mScanBrowser->setCurMRID(mrid);
    mScanBrowser->setScanDir(scanDir);
    mScanBrowser->setCurAge(age);
    mScanBrowser->show();
}


///
//  Slot for when the scan is added to the queue
//
void SelectScans::scanAdded(bool added)
{
    mScanAdded.emit(added);
}
