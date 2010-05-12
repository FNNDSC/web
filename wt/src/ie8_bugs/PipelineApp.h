//
//
//  Description:
//      IE8 bugs reproducer app
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


namespace Wt
{
    class WLabel;
    class WStackedWidget;
    class WContainerWidget;
}

using namespace Wt;

class SubjectPage;

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

    ///
    /// Handle too large a request being sent
    ///
    void largeRequest(int size);

private:

    /// Main site widget
    WContainerWidget *mMainSiteWidget;

    /// Subject page
    SubjectPage *mSubjectPage;
};


#endif // PIPELINEAPP_H
