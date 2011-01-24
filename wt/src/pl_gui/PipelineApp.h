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

// #define USE_QT // This seems to be broken in Wt 3.1.7a so I am dropping it for now

#ifdef USE_QT
  #include "WQApplication"
#else
  #include <Wt/WApplication>
#endif

namespace Wt
{
    class WLabel;
    class WStackedWidget;
	class WContainerWidget;
}

using namespace Wt;

class ConfigOptions;
class ConfigXML;
class SubjectPage;
class ResultsPage;
class ProjectPage;
class LoginPage;

///
/// \class PipelineApp
/// \brief Main application class.
///
class PipelineApp : 
#ifdef USE_QT
	public WQApplication
#else
	public WApplication
#endif
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
    /// Callback for creating Qt objects
    ///
    virtual void create();

    ///
    /// Callback for destroying Qt objects
    ///
    virtual void destroy();

    ///
    /// Override finalize to handle destruction of threads
    ///
    virtual void finalize();


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

    ///
    /// Get the current user
    ///
    const std::string& getCurrentUser() const { return mCurrentUser; }

    ///
    /// Get the current E-mail
    ///
    const std::string& getCurrentEmail() const { return mCurrentEmail; }

private:

    ///
    /// Create the user interface widgets
    ///
    void createUI();

    ///
    /// Handle too large a request being sent
    ///
    void largeRequest(int size);

    ///
    ///	User logged in succesfully [slot]
    ///
    void userLoggedIn(std::string userName, std::string email);

    ///
    ///  Project chosen [slot]
    ///
    void projectChosen(std::string path);

    ///
    ///  Go home [slot]
    ///
    void goHome();

    ///
    ///  Log out of site [slot]
    ///
    void logout();

private:

    typedef enum
    {
        SUBJECTS_TAB = 0,
        RESULTS_TAB = 1,
        CLUSTER_LOAD_TAB = 2
    } PageEnum;

    /// Results page
    ResultsPage *mResultsPage;

    /// Subject page
    SubjectPage *mSubjectPage;

    /// Project Page
    ProjectPage *mProjectPage;

    /// Login page
    LoginPage *mLoginPage;

    /// Configuration options
    ConfigOptions *mConfigOptions;

    /// Configuration XML
    ConfigXML *mConfigXML;

    /// Stacked widget
    WStackedWidget *mStackedWidget;

    /// Main site widget
    WContainerWidget *mMainSiteWidget;

    /// Current user name
    std::string mCurrentUser;

    /// Current E-mail
    std::string mCurrentEmail;

    /// Current project label
    WLabel *mCurrentProjectLabel;

    /// Current user label
    WLabel *mCurrentUserLabel;

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
///     NOTE: This function is not thread safe, it should only be called
///           from the main thread.
///
ConfigOptions* getConfigOptionsPtr();

///
/// Return the ConfigOptions pointer from the application instance.  Each
/// session has its own WApplication instance.
///
///     THREAD-SAFE version of function
///
ConfigOptions* getConfigOptionsPtrTS(WApplication *appInstance);

///
/// Return the current logged in user name
///
const std::string getCurrentUserName();

///
/// Return the current E-mail address
///
const std::string getCurrentUserEmail();


#endif // PIPELINEAPP_H
