//
//
//  Description:
//      This is the widget for Freesurfer pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSFS_H
#define PIPELINEOPTIONSFS_H

#include "PipelineOptions.h"

///
//  Constants
//
const int FIRST_OPTIONAL_FS_STAGE = 3; // Stages 1 and 2 are required
#if 0 // Temporarily disable stage 4 and 5
    const int NUM_OPTIONAL_FS_STAGES = 3;
#else
    const int NUM_OPTIONAL_FS_STAGES = 1;
#endif

///
/// \class PipelineOptionsFS
/// \brief Class for options to configure freesurfer pipeline
///
class PipelineOptionsFS : public PipelineOptions
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsFS(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsFS();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Generate command-line options string based on user choices
    ///
    std::string getCommandLineString() const;

private:


private:

    /// Buttons for pipeline stages
    WCheckBox *mStageBoxes[NUM_OPTIONAL_FS_STAGES];

};

#endif // PIPELINEOPTIONSFS_H

