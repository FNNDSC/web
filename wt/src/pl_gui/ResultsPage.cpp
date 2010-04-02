//
//
//  Description:
//      This is the widget that controls and lays out the entire "Results" page.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ResultsPage.h"
#include "ResultsTable.h"
#include "PipelineArgTable.h"
#include "ConfigOptions.h"
#include "MRIBrowser.h"
#include "MonitorLogTab.h"
#include "MonitorResultsTab.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
#include <Wt/WTabWidget>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

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
ResultsPage::ResultsPage(const MRIBrowser *mriBrowser, WContainerWidget *parent) :
    mMRIBrowser(mriBrowser),
    WContainerWidget(parent)
{
    setStyleClass("tabdiv");

    mResultsTable = new ResultsTable();

    WContainerWidget *browserContainer = new WContainerWidget();
    WContainerWidget *resultsContainer = new WContainerWidget;

    WText *showText = new WText("Show Results by User:");
    showText->setWordWrap(false);
    mUserComboBox = new WComboBox();

    WGridLayout *userBox = new WGridLayout();
    userBox->addWidget(showText, 0, 0, AlignRight | AlignMiddle);
    userBox->addWidget(mUserComboBox, 0, 1, AlignLeft | AlignMiddle );
    mUserComboBox->addItem("<user>");
    mUserComboBox->addItem("All");

    mSearchLineEdit = new WLineEdit("");
    mSearchLineEdit->setMinimumSize(150, WLength::Auto);

    mSearchPushButton = new WPushButton("GO");
    WPushButton *clearButton = new WPushButton("Clear");
    WPushButton *refreshButton = new WPushButton("Refresh");

    userBox->addWidget(new WText("Search:"), 0, 2, AlignRight | AlignMiddle);
    userBox->addWidget(mSearchLineEdit, 0, 3, AlignLeft| AlignMiddle);
    userBox->addWidget(mSearchPushButton, 0, 4, AlignLeft | AlignMiddle);
    userBox->addWidget(clearButton, 0, 5, AlignLeft | AlignMiddle);
    userBox->addWidget(refreshButton, 0, 6, AlignRight | AlignMiddle);
    userBox->setColumnStretch(2, 1);
    userBox->setColumnStretch(6, 1);
    userBox->setColumnStretch(7, 3);

    mPipelineArgTable = new PipelineArgTable();
    mPipelineArgTable->setMinimumSize(WLength(1000, WLength::Pixel),
                                      WLength(150, WLength::Pixel));
    mPipelineArgTable->setMaximumSize(WLength(1000, WLength::Pixel),
                                      WLength(150, WLength::Pixel));


    WGridLayout *browserLayout = new WGridLayout();
    browserLayout->addLayout(userBox, 0, 0);
    browserLayout->addWidget(mResultsTable, 1, 0);
    browserLayout->addWidget(mPipelineArgTable, 2, 0);
    browserLayout->setRowStretch(0, -1);
    browserLayout->setRowStretch(1, 1);
    browserLayout->setRowStretch(2, -1);
    browserContainer->setLayout(browserLayout);


    mBackButton = new WPushButton("<- Back");

    WTabWidget *tabWidget = new WTabWidget();
    tabWidget->setStyleClass("toptabdiv");
    mMonitorLogTab = new MonitorLogTab(mriBrowser);
    mMonitorResultsTab = new MonitorResultsTab(mriBrowser);
    tabWidget->addTab(mMonitorResultsTab, "Results");
    tabWidget->addTab(mMonitorLogTab, "Logs");

    WGridLayout *resultsLayout = new WGridLayout();
    resultsLayout->addWidget(mBackButton, 0, 0, AlignLeft | AlignMiddle);
    resultsLayout->addWidget(tabWidget, 1, 0);
    resultsLayout->setRowStretch(0, -1);
    resultsLayout->setRowStretch(1, 1);
    resultsContainer->setLayout(resultsLayout);

    mStackedWidget = new WStackedWidget();
    mStackedWidget->addWidget(browserContainer);
    mStackedWidget->addWidget(resultsContainer);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mStackedWidget, 0, 0);
    layout->setRowStretch(0, -1);
    setLayout(layout);

    mUserComboBox->activated().connect(SLOT(this, ResultsPage::userChanged));
    mSearchPushButton->clicked().connect(SLOT(this, ResultsPage::searchPushed));
    clearButton->clicked().connect(SLOT(this, ResultsPage::clearPushed));
    mResultsTable->resultSelected().connect(SLOT(this, ResultsPage::resultSelected));
    mResultsTable->resultClicked().connect(SLOT(this, ResultsPage::resultClicked));
    mBackButton->clicked().connect(SLOT(this, ResultsPage::backPushed));
    refreshButton->clicked().connect(SLOT(this, ResultsPage::refreshClicked));

    // All items in the tabbed widget need to be resized to 100% to
    // fill the contents.  This trick came from the Wt WTabWidget
    // documentation and took me a good half a day to figure out.
    mMonitorLogTab->resize(WLength(100, WLength::Percentage),
                           WLength(100, WLength::Percentage));
    mMonitorResultsTab->resize(WLength(100, WLength::Percentage),
                               WLength(100, WLength::Percentage));
}

///
//  Destructor
//
ResultsPage::~ResultsPage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Create Qt objects
//
void ResultsPage::createQt()
{
    mMonitorLogTab->createQt();
    mMonitorResultsTab->createQt();
}

///
//  Destroy Qt objects
//
void ResultsPage::destroyQt()
{
    mMonitorLogTab->destroyQt();
    mMonitorResultsTab->destroyQt();
}

///
// Reset all widgets to the default state
//
void ResultsPage::resetAll()
{
    mResultsTable->resetAll();

    resetUser(getCurrentUserName());
}

///
// Reset to the current user name
//
void ResultsPage::resetUser(const std::string &userName)
{
    if (mUserComboBox->itemText(0).toUTF8() != userName)
    {
        mUserComboBox->setItemText(0, userName);
        mUserComboBox->setCurrentIndex(0);
        userChanged(0);

        mSearchLineEdit->setText("");
    }
}

///
//  Finalize the widget (pre-destruction)
//
void ResultsPage::finalize()
{
    mMonitorLogTab->finalize();
    mMonitorResultsTab->finalize();
}

///
//  Start updating logs
//
void ResultsPage::startUpdate()
{
    mMonitorLogTab->startUpdate();
}

///
//  Stop updating logs
//
void ResultsPage::stopUpdate()
{
    mMonitorLogTab->stopUpdate();
    backPushed();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Called when user changed [slot]
//
void ResultsPage::userChanged(int index)
{
    if (index == 0)
    {
        mResultsTable->setUserFilter(true, mUserComboBox->itemText(index).toUTF8());
    }
    else
    {
        mResultsTable->setUserFilter(false);
    }
}

///
/// Called when search button pushed [slot]
///
void ResultsPage::searchPushed()
{
    mResultsTable->setSearchTerm(true, mSearchLineEdit->text().toUTF8());
}

///
/// Called when clear button pushed [slot]
///
void ResultsPage::clearPushed()
{
    mResultsTable->setSearchTerm(false);
    mSearchLineEdit->setText("");
}

///
//  Called when back button pushed [slot]
//
void ResultsPage::backPushed()
{
    mStackedWidget->setCurrentIndex(0);
    mMonitorLogTab->resetAll();
    mMonitorResultsTab->resetAll();
}


///
//  New result clicked (single click) [slot]
//
void ResultsPage::resultClicked(std::string clusterShFile, std::string metaScript, std::string arguments)
{
    mPipelineArgTable->setScript(metaScript, arguments);
}

///
//  New result selected (double click) [slot]
//
void ResultsPage::resultSelected(std::string clusterShFile, std::string metaScript, std::string arguments)
{
    std::string jobSelectedFile = path(clusterShFile).branch_path().string() + "/" + metaScript;
    mMonitorLogTab->jobSelectedChanged(jobSelectedFile);
    mMonitorResultsTab->jobSelectedChanged(jobSelectedFile);
    mStackedWidget->setCurrentIndex(1);
}

///
/// Refresh clicked [slot]
///
void ResultsPage::refreshClicked()
{
    mResultsTable->resetAll();
    resetAll();
}

///
//  Creates a title widget.
//
WText* ResultsPage::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

