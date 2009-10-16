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
#include <vector>
#include <string>

namespace Wt
{
    class WGroupBox;
    class WLabel;
    class WSelectionBox;
    class WPushButton;
}

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
    /// Signal accessor for scan added
    ///
    Wt::Signal<bool>& scanAdded() { return mScanAdded; }



private:

    ///
    ///  Add scan clicked [slot]
    ///
    void addScanClicked();

    ///
    ///  Remove scan clicked [slot]
    ///
    void removeScanClicked();

private:

    /// Signal for when scan is add
    Wt::Signal<bool> mScanAdded;

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

    } ScanData;

    /// Patient info group box
    WGroupBox *mPatientInfoBox;

    /// Scan box
    WGroupBox *mScanBox;

    /// Scans to process box
    WGroupBox *mScansToProcessBox;

    /// Push button to add scans
    WPushButton *mAddScanButton;

    /// Push button to remove scans
    WPushButton *mRemoveScanButton;


    /// Patient info label
    WLabel *mPatientInfoLabel;

    /// Scans selection box
    WSelectionBox *mScansSelectionBox;

    /// Currently selected scans
    WSelectionBox *mScansToProcessList;

    /// List of scans to process
    std::vector<ScanData> mScansToProcessData;

    /// List of DICOM files for currently selected scans
    std::vector<std::string> mScansDicomFiles;

    /// Current MRID
    std::string mCurMRID;

    /// Current scan directory
    std::string mCurScanDir;

};

#endif // SCANBROWSER_H

