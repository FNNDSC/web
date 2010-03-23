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
#include <vector>
#include "GlobalEnums.h"
#include "ScanBrowser.h"

namespace Wt
{
    class WText;
}
using namespace Wt;

class MRIBrowser;
class ClusterLoadChart;
class SubmitJobDialog;

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
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    ///  Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    ///  Create Qt objects
    ///
    void createQt();

    ///
    ///  Destroy Qt objects
    ///
    void destroyQt();

    ///
    /// Signal accessor for scan added
    ///
    Wt::Signal<bool>& getScanAdded() { return mScanAdded; }

    ///
    ///	Get the currently selected pipeline type
    ///
    const Enums::PipelineType& getCurrentPipeline() const { return mScanBrowser->getCurrentPipeline();	}

    ///
    /// Get pipeline type as string (for command line arg)
    ///
    const std::string getCurrentPipelineAsString() const;

    ///
    /// Get the list of currently selected scans
    ///
    const std::vector<ScanBrowser::ScanData>& getScansToProcess() const { return mScanBrowser->getScansToProcess(); }

    ///
    /// Get the MRI browser
    ///
    MRIBrowser* getMRIBrowser() const { return mMRIBrowser;   }

private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

    ///
    /// \brief Slot for when the MRI selection changes
    ///
    void mriChanged(std::string mrid, std::string scanDir, std::string age);

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

    /// Cluster load chart
    ClusterLoadChart *mClusterLoadChart;

    /// Cluster text
    WText *mClusterText;
};

#endif // SELECTSCANS_H

