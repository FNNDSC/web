//
//
//  Description:
//      This is the widget for Tractography pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSTRACT_H
#define PIPELINEOPTIONSTRACT_H

#include "PipelineOptions.h"

///
//  Constants
//
#if 0 // Temporarily disable stage 4 and 5
    const int NUM_TRACT_STAGES = 5;
#else
    const int NUM_TRACT_STAGES = 3;
#endif

namespace Wt
{
    class WCheckBox;
};

///
/// \class PipelineOptionsTract
/// \brief Class for options to configure tractography pipeline
///
class PipelineOptionsTract : public PipelineOptions
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsTract(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsTract();

    ///
    ///  Reset to default state
    ///
    void resetAll();

    ///
    /// Generate command-line options string based on user choices
    ///
    std::string getCommandLineString() const;

private:


private:

    /// Buttons for pipeline stages
    WCheckBox *mStageBoxes[NUM_TRACT_STAGES];


};

#endif // PIPELINEOPTIONSTRACT_H

