//
//
//  Description:
//      This is a widget that is used for choosing a project to filter the list of
//      MRIDs
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <string>

namespace Wt
{
    class WText;
    class WLineEdit;
    class WPushButton;
}

class ProjectChooser;
class ProjectCreator;

using namespace Wt;

///
/// \class ProjectPage
/// \brief widget that is used for choosing a project to filter the list of MRIDs
///
class ProjectPage : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
	ProjectPage(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ProjectPage();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Project chosen signal accessor
    ///
    Wt::Signal<std::string>& projectChosen()    {   return mProjectChosen;  }

private:

    ///
    /// Project load clicked [slot]
    ///
    void projectLoad(std::string path);

    ///
    /// Project added[slot]
    ///
    void projectAdded(std::string projectName);

    /// Project chosen [signal]
    Wt::Signal<std::string> mProjectChosen;

    /// Project chooser
    ProjectChooser *mProjectChooser;

    /// Project creator
    ProjectCreator *mProjectCreator;
};

#endif // PROJECTPAGE_H

