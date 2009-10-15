//
//
//  Description:
//      Definition of the main application class for the pipeline GUI.  It has the
//      startup logic and has the container widgets that constitute the entirety
//      of the application.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEAPP_H
#define PIPELINEAPP_H

#include <Wt/WApplication>

using namespace Wt;


///
/// \class PipelineApp
/// \brief Main application class.
///
class PipelineApp : public WApplication
{
public:
    ///
    /// Constructor
    ///
    PipelineApp(const WEnvironment &env);

    ///
    /// Destructor
    ///
    virtual ~PipelineApp();

private:

    ///
    /// Create the user interface widgets
    ///
    void createUI();

};

#endif // PIPELINEAPP_H
