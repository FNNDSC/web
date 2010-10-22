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
#include "ScansToProcessTable.h"
#include "GlobalEnums.h"

namespace Wt
{
    class WLabel;
    class WSelectionBox;
}

class SelectScans;

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
    PipelineStatus(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineStatus();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    ///  Update all elements of widget to current values (on next clicked)
    ///
    void updateAll(SelectScans *selectScans);

private:

    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

private:

    /// Pipeline type label
    WLabel *mPipelineTypeLabel;

    /// Currently selected scans
    ScansToProcessTable *mScansToProcessTable;

};

#endif // PIPELINESTATUS_H

