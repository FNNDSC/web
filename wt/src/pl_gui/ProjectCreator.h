//
//
//  Description:
//      This is a widget that is used for creating a project to filter the list of
//      MRIDs
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PROJECTCREATOR_H
#define PROJECTCREATOR_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <string>
#include <vector>

class SearchTerm;
namespace Wt
{
    class WText;
    class WLineEdit;
    class WPushButton;
    class WVBoxLayout;
    class WGridLayout;
    class WComboBox;
    class WMessageBox;
}
using namespace Wt;

///
/// \class ProjectCreator
/// \brief widget that is used for creating a project to filter the list of MRIDs
///
class ProjectCreator : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ProjectCreator(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ProjectCreator();

    ///
    /// Emit signal that new project was added [signal]
    ///
    Wt::Signal<std::string>& projectAdded() {   return mProjectAdded;  }

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    ///  Set from XML
    ///
    void setFromXML(std::string projectPathXML);

private:

    /// Project added signal
    Wt::Signal<std::string> mProjectAdded;

    ///
    /// Search term added or removed [slot]
    ///
    void searchAddOrRemove(bool add, int ID);

    ///
    /// Create clicked [slot]
    ///
    void createClicked();

    ///
    /// Save clicked [slot]
    ///
    void saveClicked();

    ///
    /// OK Create (dialog) clicked [slot]
    ///
    void okCreateClicked();

    ///
    /// Cancel Create (dialog) clicked [slot]
    ///
    void cancelCreateClicked();

    ///
    ///  Save project to XML
    ///
    bool saveProjectToXML(std::string projectFilePath);

    ///
    /// Handle save message box closed [slot]
    ///
    void handleSaveMessageBoxClosed(StandardButton);

    /// List of search terms
    std::vector<SearchTerm*> mSearchTerms;

    /// Layout
    WVBoxLayout *mSearchTermLayout;

    /// Create project dialog
    WDialog *mCreateProjectDialog;

    /// Error label
    WLabel *mCreateErrorLabel;

    /// Project name edit
    WLineEdit *mProjectNameEdit;

    /// AnyOrAll combo box
    WComboBox *mAnyOrAllComboBox;

    /// Save button
    WPushButton *mSaveButton;

    /// Current project path
    std::string mCurrentProjectPath;

    /// Save message box
    WMessageBox *mSaveMessageBox;
};

#endif // PROJECTCREATOR_H

