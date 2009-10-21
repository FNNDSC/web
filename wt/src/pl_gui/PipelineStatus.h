//
//
//  Description:
//      This is the the left pane of the pipeline configuration screen
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINESTATUS_H
#define PIPELINESTATUS_H

#include <Wt/WContainerWidget>
#include "ScanBrowser.h"
#include "GlobalEnums.h"

namespace Wt
{
    class WLabel;
    class WSelectionBox;
}

using namespace Wt;

///
/// \class PipelineStatus
/// \brief Left pane of the pipeline configuration screen
///
class PipelineStatus : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    PipelineStatus(const std::vector<ScanBrowser::ScanData> &scansToProcess,
                   const Enums::PipelineType &pipelineType,
                   WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineStatus();

    ///
    ///  Update all elements of widget to current values (on next clicked)
    ///
    void updateAll();

private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);


private:

    /// Pipeline type label
    WLabel *mPipelineTypeLabel;

    /// Currently selected scans
    WSelectionBox *mScansToProcessList;

    /// Scans to process (passed in from SubjectPage)
    const std::vector<ScanBrowser::ScanData>& mScansToProcessData;

    /// Current pipeline type (passed in from SubjectPage)
    const Enums::PipelineType& mPipelineType;

};

#endif // PIPELINESTATUS_H

