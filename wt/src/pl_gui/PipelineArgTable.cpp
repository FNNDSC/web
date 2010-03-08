//
//
//  Description:
//      This widget displays a table of the arguments passed to the pipeline.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineArgTable.h"
#include "GlobalEnums.h"
#include "PipelineApp.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
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
#include <Wt/WLineEdit>
#include <Wt/WDoubleValidator>
#include <Wt/WIntValidator>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTreeView>
#include <Wt/WSortFilterProxyModel>

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
PipelineArgTable::PipelineArgTable(WContainerWidget *parent) :
    WContainerWidget(parent),
    mSortFilterProxyModel(NULL)
{
    setStyleClass("tabdiv");

    mModel = new WStandardItemModel(this);
    mModel->insertColumns(0, 2);
    mModel->setHeaderData(0, boost::any(WString::fromUTF8("Pipeline Option")));
    mModel->setHeaderData(1, boost::any(WString::fromUTF8("Setting")));

    mTreeView = new WTreeView();
    mTreeView->setRootIsDecorated(false);
    mTreeView->setAlternatingRowColors(true);
    mTreeView->setSelectionMode(NoSelection);

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mTreeView);

    setLayout(layout);

    resetAll();

}

///
//  Destructor
//
PipelineArgTable::~PipelineArgTable()
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
void PipelineArgTable::resetAll()
{
    setScript("", "");
}

///
//  Populate table with data
//
void PipelineArgTable::setScript(std::string metaScript, std::string arguments)
{
    if (mSortFilterProxyModel != NULL)
    {
        delete mSortFilterProxyModel;
    }

    mModel->removeRows(0, mModel->rowCount());

    if (metaScript != "")
    {
        getConfigXMLPtr()->translateScriptArgs(mModel, metaScript, arguments);
    }

    mSortFilterProxyModel = new WSortFilterProxyModel(this);
    mSortFilterProxyModel->setSourceModel(mModel);
    mSortFilterProxyModel->setDynamicSortFilter(true);
    mSortFilterProxyModel->setFilterKeyColumn(0);
    mSortFilterProxyModel->setFilterRole(DisplayRole);
    mSortFilterProxyModel->sort(0, DescendingOrder);

    mTreeView->setModel(mSortFilterProxyModel);
    mTreeView->setColumnWidth(0, WLength(280, WLength::Pixel));
    mTreeView->setColumnWidth(1, WLength(720, WLength::Pixel));
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


