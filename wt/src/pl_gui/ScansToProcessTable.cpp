//
//
//  Description:
//      Implementation of scans to process table widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "ScansToProcessTable.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTableView>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WMessageBox>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

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
ScansToProcessTable::ScansToProcessTable(WContainerWidget *parent) :
    WTableView(parent)
{
    mModel = new WStandardItemModel(this);
    mModel->invisibleRootItem()->setRowCount(0);


    setModel(mModel);
    setSortingEnabled(false);
    setSelectionBehavior(SelectRows);
    setSelectionMode(ExtendedSelection);

    // Connect signals/slots
    selectionChanged().connect(SLOT(this, ScansToProcessTable::selectionChangedSlot));

    resetAll();
}

///
//  Destructor
//
ScansToProcessTable::~ScansToProcessTable()
{
    mModel->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset all widgets to the default state
//
void ScansToProcessTable::resetAll()
{
    mModel->clear();
}

///
//  Set the list of scan inputs
//
void ScansToProcessTable::setPipelineInputs(const std::vector<ConfigXML::InputNode>& pipelineInputs)
{

    mModel->clear();
    mModel->insertColumns(0, pipelineInputs.size() + 1);
    mModel->setHeaderData(0, boost::any(WString::fromUTF8("Patient ID")));
    setColumnWidth(0, WLength(200, WLength::Pixel));
    for(unsigned int col = 0; col < pipelineInputs.size(); col++)
    {
        setColumnWidth(col+1, WLength(200, WLength::Pixel));
        mModel->setHeaderData(col + 1, boost::any(WString::fromUTF8(pipelineInputs[col].mName)));
    }
}


///
//  Append entry to table
//
void ScansToProcessTable::addEntry(int inputIndex, const ScanData& scanData)
{
    // Determine first whether to add a new row or add to current row
    if (mModel->rowCount() == 0)
    {
        mModel->insertRows(0, 1);
        mModel->setData(0, 0, boost::any(WString::fromUTF8(scanData.mMRID)));
        mModel->setData(0, inputIndex + 1, boost::any(WString::fromUTF8(scanData.mScanName)));
        mModel->setData(0, inputIndex + 1, boost::any(scanData), UserRole);
    }
    else
    {
        for (int row = 0; row < mModel->rowCount(); row++)
        {
            boost::any d = mModel->data(row, inputIndex + 1);
            if (d.empty())
            {
                mModel->setData(row, inputIndex + 1, boost::any(WString::fromUTF8(scanData.mScanName)));
                mModel->setData(row, inputIndex + 1, boost::any(scanData), UserRole);
                return;
            }
        }

        mModel->insertRows(mModel->rowCount(), 1);
        mModel->setData(mModel->rowCount() - 1, 0, boost::any(WString::fromUTF8(scanData.mMRID)));
        mModel->setData(mModel->rowCount() - 1, inputIndex + 1, boost::any(WString::fromUTF8(scanData.mScanName)));
        mModel->setData(mModel->rowCount() - 1, inputIndex + 1, boost::any(scanData), UserRole);
    }
}

///
//  Remove currently selected scans
//
void ScansToProcessTable::removeSelected()
{
    WModelIndexSet selected = selectedIndexes();

    int rowsRemoved = 0;
    for(WModelIndexSet::iterator i = selected.begin();
        i != selected.end(); i++)
    {
        WModelIndex index = *i;

        mModel->removeRows(index.row() - rowsRemoved, 1);
        rowsRemoved++;
    }
}

///
/// Get the list of currently selected scans
///
std::vector<ScansToProcessTable::ScanData> ScansToProcessTable::getScansToProcess(int inputIndex) const
{
    std::vector<ScanData> scansToProcessList;

    for (int i = 0; i < mModel->rowCount(); i++)
    {
        boost::any d = mModel->data(i, inputIndex + 1, UserRole);
        if (!d.empty())
        {
            ScanData scanData = boost::any_cast<ScanData>(d);
            scansToProcessList.push_back(scanData);
        }
    }

    return scansToProcessList;
}

///
/// Validate that the current table has all valid and complete entries
///
bool ScansToProcessTable::validateTable() const
{
    if (mModel->rowCount() == 0)
    {
        return false;
    }

    for (int row = 0; row < mModel->rowCount(); row++)
    {
        for (int col = 1; col < mModel->columnCount(); col++)
        {
            boost::any d = mModel->data(row, col, UserRole);
            // Empty data means table is not complete
            if (d.empty())
            {
                return false;
            }

            ScanData curData = boost::any_cast<ScanData>(d);

            // Non matching DICOM folders is not allowed
            for (int prevCol = 1; prevCol < col; prevCol++)
            {
                boost::any prevD = mModel->data(row, prevCol, UserRole);
                if (prevD.empty())
                {
                    return false;
                }

                ScanData prevData = boost::any_cast<ScanData>(prevD);
                if (curData.mScanDir != prevData.mScanDir)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

///
/// Copy table from one widget to another
///
void ScansToProcessTable::copyTable(const ScansToProcessTable *prevTable)
{
    setSelectionMode(NoSelection);
    mModel->clear();
    mModel->insertRows(0, prevTable->mModel->rowCount());
    for (int row = 0; row < prevTable->mModel->rowCount(); row++)
    {
        mModel->insertColumns(row, prevTable->mModel->columnCount());
        if (row == 0)
        {
            setColumnWidth(0, WLength(75, WLength::Pixel));
        }
        for (int col = 0; col < prevTable->mModel->columnCount(); col++)
        {
            mModel->setHeaderData(col, prevTable->mModel->headerData(col));
            mModel->setData(row, col, prevTable->mModel->data(row, col, DisplayRole), DisplayRole);
            mModel->setData(row, col, prevTable->mModel->data(row, col, UserRole), UserRole);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
/// Selection changed [slot]
///
void ScansToProcessTable::selectionChangedSlot()
{
    WModelIndexSet selected = selectedIndexes();

    mScanSelected.emit(!selected.empty());
}

