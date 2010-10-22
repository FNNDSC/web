//
//
//  Description:
//      Definition of Scan browser widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef SCANBROWSER_H
#define SCANBROWSER_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <vector>
#include <string>
#include "ScansToProcessTable.h"
#include "GlobalEnums.h"

namespace Wt
{
    class WGroupBox;
    class WLabel;
    class WSelectionBox;
    class WPushButton;
    class WButtonGroup;
    class WMessageBox;
}

class MRIInfoBox;
class PatientInfoBox;

using namespace Wt;

///
/// \class ScanBrowser
/// \brief Provides a browser for all of the scans for a particular MRID
///
class ScanBrowser : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ScanBrowser(WContainerWidget *parent = 0);

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Destructor
    ///
    virtual ~ScanBrowser();

    ///
    /// \brief Set the scan directory
    ///
    void setScanDir(std::string scanDir);

    ///
    /// \brief Set the current MRID
    ///
    void setCurMRID(std::string mrid);

    ///
    /// \brief Set the curreng age
    ///
    void setCurAge(std::string age);

    ///
    /// Signal accessor for scan added
    ///
    Wt::Signal<bool>& scanAdded() { return mScanAdded; }

    ///
    ///	Get the currently selected pipeline type
    ///
    const Enums::PipelineType& getCurrentPipeline() const { return mPipelineType;	}

    ///
    /// Get the currently selected pipeline as a string
    ///
    const std::string getCurrentPipelineAsString() const;

    ///
    /// Get the list of currently selected scans
    ///
    std::vector<ScansToProcessTable::ScanData> getScansToProcess(int inputIndex = 0) const;

    ///
    /// Get the ScansToProcessTable widget
    ///
    const ScansToProcessTable* getScansToProcessTable() const { return mScansToProcessTable; }

    ///
    /// Handle scan selected for remove
    ///
    void scanSelectedForRemove(bool selected);

private:

    ///
    /// Set the current pipeline type
    ///
    void setCurrentPipeline(Enums::PipelineType pipelineType);

    ///
    /// Find a match between a string a comma separated list of strings
    /// \param seriesList Comma separated list of series to search for
    /// \param serisName Series name on which to match
    ///
    bool findSeriesMatch(const std::string& seriesList,
                         const std::string& seriesName) const;

    ///
    ///  Add scan clicked [slot]
    ///
    void addScanClicked();

    ///
    ///  Remove scan clicked [slot]
    ///
    void removeScanClicked();

    ///
    ///	Pipeline override clicked [slot]
    ///
    void pipelineOverrideClicked();

    ///
    //  Scan selection changed
    //
    void scanSelectionChanged();

    ///
    /// Handle pipeline dialog closed [slot]
    ///
    void handlePipelineDialogClosed(WDialog::DialogCode dialogCode);

    ///
    /// Handle message box finished [slot]
    ///
    void handleMessageBoxFinished(StandardButton dialogCode);

private:

    /// Signal for when scan is add
    Wt::Signal<bool> mScanAdded;

    /// Patient info box
    PatientInfoBox *mPatientInfoBox;

    /// MRI info box
    MRIInfoBox *mMRIInfoBox;

    /// Scans to process box
    WGroupBox *mScansToProcessBox;

    /// Push button to add scans
    std::vector<WPushButton*> mAddScanButtonList;

    /// Push button to remove scans
    WPushButton* mRemoveScanButton;

    /// Scans selection box
    WSelectionBox *mScansSelectionBox;

    /// Scans to process table
    ScansToProcessTable *mScansToProcessTable;

    /// Current pipeline mode
    WLabel *mPipelineModeLabel;

    /// Override pipeline button
    WPushButton *mPipelineOverrideButton;

    /// Current pipeline type
    Enums::PipelineType mPipelineType;

    /// List of DICOM files for currently selected scans
    std::vector<std::string> mScansDicomFiles;

    /// List of scan dates for currently selected scans
    std::string mScansDate;

    /// Current MRID
    std::string mCurMRID;

    /// Current scan directory
    std::string mCurScanDir;

    /// Current age
    std::string mCurAge;

    /// Current scan to add
    ScansToProcessTable::ScanData mNewScanData;

    /// Pipeline select dialog
    WDialog *mPipelineDialog;

    /// Pipline dialog group
    WButtonGroup *mPipelineDialogGroup;

    /// Message box
    WMessageBox *mMessageBox;

    /// Add scan message box
    WMessageBox *mAddScanMessageBox;
};

#endif // SCANBROWSER_H

