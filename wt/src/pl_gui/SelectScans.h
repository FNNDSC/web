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
#ifndef SELECTSCANS_H
#define SELECTSCANS_H

#include <Wt/WContainerWidget>

using namespace Wt;

class MRIBrowser;
class ScanBrowser;

///
/// \class SelectScans
/// \brief Container widget for the contents of the Select Scans stage
///
class SelectScans : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    SelectScans(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~SelectScans();

    ///
    /// Signal accessor for scan added
    ///
    Wt::Signal<bool>& getScanAdded() { return mScanAdded; }


private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

    ///
    /// \brief Slot for when the MRI selection changes
    ///
    void mriChanged(std::string mrid, std::string scanDir);

    ///
    /// \brief  Slot for when the scan is added to the queue
    ///
    void scanAdded(bool added);


private:

    // Scan added signal
    Wt::Signal<bool> mScanAdded;

    /// MRI Browser widget
    MRIBrowser *mMRIBrowser;

    /// Scan browser widget
    ScanBrowser *mScanBrowser;


};

#endif // SELECTSCANS_H

