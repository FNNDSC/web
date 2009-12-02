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

class MonitorPage;
class ConfigOptions;
class ConfigXML;
class SubjectPage;
class ClusterLoadPage;

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

    ///
    /// Slot for handling when the main tab changes
    ///
    void mainTabChanged(int currentIndex);

    ///
    /// Get the configuration XML object
    ///
    ConfigXML* getConfigXML() const { return mConfigXML; }

    ///
    /// Get the configuration options object
    ///
    ConfigOptions* getConfigOptions() const { return mConfigOptions; }

private:

    ///
    /// Create the user interface widgets
    ///
    void createUI();

    ///
    /// Handle too large a request being sent
    ///
    void largeRequest(int size);

private:
    /// Monitor page
    MonitorPage *mMonitorPage;

    /// Subject page
    SubjectPage *mSubjectPage;

    /// Cluster load page
    ClusterLoadPage *mClusterLoadPage;

    /// Configuration options
    ConfigOptions *mConfigOptions;

    /// Configuration XML
    ConfigXML *mConfigXML;
};

///
/// Return the ConfigXML pointer from the application instance.  Each
/// session has its own WApplication instance.
///
ConfigXML* getConfigXMLPtr();

///
/// Return the ConfigOptions pointer from the application instance.  Each
/// session has its own WApplication instance.
///
ConfigOptions* getConfigOptionsPtr();

#endif // PIPELINEAPP_H
