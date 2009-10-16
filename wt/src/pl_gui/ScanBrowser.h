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


private:

    /// Patient info group box
    WGroupBox *mPatientInfoBox;

    /// Scan box
    WGroupBox *mScanBox;

    /// Patient info label
    WLabel *mPatientInfoLabel;

    /// Scans selection box
    WSelectionBox *mScansSelectionBox;

    /// List of DICOM files for currently selected scans
    std::vector<std::string> mScansDicomFiles;

};

#endif // SCANBROWSER_H

