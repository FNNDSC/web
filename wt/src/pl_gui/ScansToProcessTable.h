//
//
//  Description:
//      Definition of scans to process table widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef SCANSTOPROCESSTABLE_H
#define SCANSTOPROCESSTABLE_H

#include <Wt/WTableView>
#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include <vector>
#include <string>
#include "GlobalEnums.h"
#include "ConfigXML.h"

namespace Wt
{
    class WTableView;
    class WStandardItemModel;
}

using namespace Wt;

///
/// \class ScansToProcessTable
/// \brief Provides a widget that displays a list of scans to process per-subject
///
class ScansToProcessTable : public WTableView
{
public:
    /// Storage of scan data
    typedef struct
    {
        /// MRID
        std::string mMRID;

        /// Scan DICOM file
        std::string mDicomFile;

        /// Scan name
        std::string mScanName;

        /// Scan directory
        std::string mScanDir;

        /// Age
        std::string mAge;

        /// Scan date
        std::string mScanDate;
    } ScanData;


    ///
    /// Constructor
    ///
    ScansToProcessTable(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ScansToProcessTable();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    ///  Set the list of scan inputs
    ///
    void setPipelineInputs(const std::vector<ConfigXML::InputNode>& pipelineInputs);

    ///
    ///  Append entry to table
    ///
    void addEntry(int inputIndex, const ScanData& scanData);

    ///
    /// Signal to emit when scan is selected
    ///
    Wt::Signal<bool>& scanSelected() { return mScanSelected; }

    ///
    //  Remove currently selected scans
    //
    void removeSelected();

    ///
    /// Get the list of currently selected scans
    ///
    std::vector<ScansToProcessTable::ScanData> getScansToProcess(int inputIndex) const;

    ///
    /// Validate that the current table has all valid and complete entries
    ///
    bool validateTable() const;

    ///
    /// Copy contents of one table to another
    ///
    void copyTable(const ScansToProcessTable *prevTable);


private:

    ///
    /// Selection changed [slot]
    ///
    void selectionChangedSlot();


private:
    /// Signal for when scan is selected
    Wt::Signal<bool> mScanSelected;


    /// Tree view of patient info
    WTableView *mTableView;

    /// Model data
    WStandardItemModel *mModel;
};

#endif // SCANSTOPROCESSTABLE_H

