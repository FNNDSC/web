//
//
//  Description:
//      This is the base widget for pipeline options (tract, fs, etc.).  It contains
//      the options which are common amongst all pipelines.  The other options are
//      in sub-classed objects.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONS_H
#define PIPELINEOPTIONS_H

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
/// \class PipelineOptions
/// \brief Base class for options to configure pipeline
///
class PipelineOptions : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    PipelineOptions(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptions();


private:


private:


};

#endif // PIPELINEOPTIONS_H

