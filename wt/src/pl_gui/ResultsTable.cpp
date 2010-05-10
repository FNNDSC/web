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
#include "ResultsTable.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include "MonitorResultsTab.h"
#include "MonitorLogTab.h"
#include "ResultsFilterProxyModel.h"
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
#include <Wt/WTreeView>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WDateTime>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <mxml.h>
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
ResultsTable::ResultsTable(WContainerWidget *parent) :
    WContainerWidget(parent),
    mSortFilterProxyModel(NULL)
{
    setStyleClass("tabdiv");

    mModel = new WStandardItemModel(this);
    mModel->insertColumns(0, 11);
    mModel->setHeaderData(0, boost::any(WString::fromUTF8("Date")));
    mModel->setHeaderData(1, boost::any(WString::fromUTF8("User")));
    mModel->setHeaderData(2, boost::any(WString::fromUTF8("MRID")));
    mModel->setHeaderData(3, boost::any(WString::fromUTF8("Name")));
    mModel->setHeaderData(4, boost::any(WString::fromUTF8("Scan Name")));
    mModel->setHeaderData(5, boost::any(WString::fromUTF8("Birthday")));
    mModel->setHeaderData(6, boost::any(WString::fromUTF8("Scan Date")));
    mModel->setHeaderData(7, boost::any(WString::fromUTF8("Manufacturer")));
    mModel->setHeaderData(8, boost::any(WString::fromUTF8("Model")));
    mModel->setHeaderData(9, boost::any(WString::fromUTF8("Software Ver")));
    mModel->setHeaderData(10, boost::any(WString::fromUTF8("Pipeline")));
    mModel->invisibleRootItem()->setRowCount(0);

    mTreeView = new WTreeView();
    mTreeView->setRootIsDecorated(false);
    mTreeView->setAlternatingRowColors(true);
    mTreeView->setSelectionMode(SingleSelection);
    mTreeView->doubleClicked().connect(SLOT(this, ResultsTable::jobSelected));
    mTreeView->clicked().connect(SLOT(this, ResultsTable::jobClicked));

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mTreeView);

    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
ResultsTable::~ResultsTable()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
// Reset all widgets to the default state
//
void ResultsTable::resetAll()
{
    if (mSortFilterProxyModel != NULL)
    {
        delete mSortFilterProxyModel;
    }

    mModel->removeRows(0, mModel->rowCount());

    populateResultsTable();

    mSortFilterProxyModel = new ResultsFilterProxyModel(this);
    mSortFilterProxyModel->setSourceModel(mModel);
    mSortFilterProxyModel->setDynamicSortFilter(true);
    mSortFilterProxyModel->setFilterKeyColumn(0);
    mSortFilterProxyModel->setFilterRole(DisplayRole);
    mSortFilterProxyModel->sort(0, DescendingOrder);
    mSortFilterProxyModel->setUserColumn(1);

    mTreeView->setModel(mSortFilterProxyModel);
    mTreeView->setColumnWidth(0, WLength(125, WLength::Pixel));
}


///
//  Called when item is selected form list [slot]
//
void ResultsTable::jobSelected(const WModelIndex& item)
{
    int modelRow = item.row();

    boost::any d = mSortFilterProxyModel->data(modelRow, 0, UserRole);
    boost::any d1 = mSortFilterProxyModel->data(modelRow, 0, UserRole + 1);
    boost::any d2 = mSortFilterProxyModel->data(modelRow, 0, UserRole + 2);
    if (!d.empty() && !d1.empty() && !d2.empty())
    {
        WString clusterCommand = boost::any_cast<WString>(d);
        WString metaScript = boost::any_cast<WString>(d1);
        WString arguments = boost::any_cast<WString>(d2);

        mResultSelected.emit(clusterCommand.toUTF8(), metaScript.toUTF8(), arguments.toUTF8());
    }
}

///
//  Called when item is selected form list [slot]
//
void ResultsTable::jobClicked(const WModelIndex& item)
{
    int modelRow = item.row();

    boost::any d = mSortFilterProxyModel->data(modelRow, 0, UserRole);
    boost::any d1 = mSortFilterProxyModel->data(modelRow, 0, UserRole + 1);
    boost::any d2 = mSortFilterProxyModel->data(modelRow, 0, UserRole + 2);
    boost::any d3 = mSortFilterProxyModel->data(modelRow, 0, UserRole + 3);
    boost::any d4 = mSortFilterProxyModel->data(modelRow, 1, DisplayRole);

    if (!d.empty() && !d1.empty() && !d2.empty())
    {
        WString clusterCommand = boost::any_cast<WString>(d);
        WString metaScript = boost::any_cast<WString>(d1);
        WString arguments = boost::any_cast<WString>(d2);

        WString jobID;
        if (!d3.empty())
        {
            jobID = boost::any_cast<WString>(d3);
        }

        WString user;
        if (!d4.empty())
        {
            user = boost::any_cast<WString>(d4);
        }

        mResultClicked.emit(clusterCommand.toUTF8(), metaScript.toUTF8(), arguments.toUTF8(),
                            jobID.toUTF8(), user.toUTF8());
    }
}

///
// Set user filter
//
void ResultsTable::setUserFilter(bool filter, std::string userFilter)
{
    mSortFilterProxyModel->setUserFilter(filter, userFilter);
    mTreeView->sortByColumn(0, DescendingOrder);
}

///
// Set search term
//
void ResultsTable::setSearchTerm(bool search, std::string searchTerm)
{
    mSortFilterProxyModel->setSearchTerm(search, searchTerm);
    mTreeView->sortByColumn(0, DescendingOrder);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Populate results table by reading the schedule.log.xml file
//
bool ResultsTable::populateResultsTable()
{
    std::string scheduleLogFile;
    scheduleLogFile = getConfigOptionsPtr()->GetOutDir() +
                           "/" +  getConfigOptionsPtr()->GetClusterName() +
                           "/schedule.log.xml";

    FILE *fp = fopen(scheduleLogFile.c_str(), "r");
    if (fp == NULL)
    {
       WApplication::instance()->log("error") << "Failed opening schedule XML log file for reading: " << scheduleLogFile;
       return false;
    }

    mxml_node_t *tree;

    tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);

    bool result = true;
    mxml_node_t *clusterJobNode;

    for (clusterJobNode = mxmlFindElement(tree, tree,
                                          "ClusterJob",
                                          NULL, NULL,
                                          MXML_DESCEND);
        clusterJobNode != NULL;
        clusterJobNode = mxmlFindElement(clusterJobNode, tree,
                                         "ClusterJob",
                                         NULL, NULL,
                                         MXML_NO_DESCEND))
    {
        int row = mModel->rowCount();
        mModel->insertRows(row, 1);

        mxml_node_t *dateNode = mxmlFindElement(clusterJobNode, clusterJobNode, "Date", NULL, NULL, MXML_DESCEND);
        if (dateNode != NULL && dateNode->child != NULL)
        {
            const char *dateStr = dateNode->child->value.opaque;
            WDateTime dateTime = WDateTime::fromString(WString(dateStr),
                                                       "MM/dd/yy HH:mm:ss ddd");
            boost::any data = boost::any(dateTime);

            mModel->setData(row, 0,  data);
        }
        // Set extended (not for display) data
        setDataColumn(clusterJobNode, "Command", row, 0, UserRole);
        setDataColumn(clusterJobNode, "MetaScript", row, 0, UserRole + 1);
        setDataColumn(clusterJobNode, "Arguments", row, 0, UserRole + 2);
        setDataColumn(clusterJobNode, "JobId", row, 0, UserRole + 3);

        // Set display data
        setDataColumn(clusterJobNode, "User", row, 1);
        setDataColumn(clusterJobNode, "PatientID", row, 2);
        setDataColumn(clusterJobNode, "PatientName", row, 3);
        setDataColumn(clusterJobNode, "ScanName", row, 4);
        setDataColumn(clusterJobNode, "PatientBirthday", row, 5);
        setDataColumn(clusterJobNode, "ImageScanDate", row, 6);
        setDataColumn(clusterJobNode, "ScannerManufacturer", row, 7);
        setDataColumn(clusterJobNode, "ScannerModel", row, 8);
        setDataColumn(clusterJobNode, "SoftwareVer", row, 9);
        setDataColumn(clusterJobNode, "MetaScript", row, 10);
    }

    fclose(fp);
    mxmlRelease(tree);

    return result;
}

///
//  Set data in the model from an XML node
//
void ResultsTable::setDataColumn(mxml_node_t *node, const char* name, int row, int col, int role)
{
    boost::any data;
    mxml_node_t *dataNode = mxmlFindElement(node, node, name, NULL, NULL, MXML_DESCEND);
    if (dataNode != NULL && dataNode->child != NULL)
    {
        std::string dataStr = std::string(dataNode->child->value.opaque);

        if (dataStr != "ERROR:")
        {
            data = boost::any(WString::fromUTF8(dataStr));
            mModel->setData(row, col,  data, role);
        }
    }
}





