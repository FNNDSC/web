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
const int NUM_FS_STAGES = 3;

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
    WCheckBox *mStageBoxes[NUM_FS_STAGES];

};

#endif // PIPELINEOPTIONSFS_H

