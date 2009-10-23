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


private:


private:


};

#endif // PIPELINEOPTIONSFS_H

