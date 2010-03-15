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
#ifndef PROJECTCHOOSER_H
#define PROJECTCHOOSER_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <string>

namespace Wt
{
    class WText;
    class WLineEdit;
    class WPushButton;
    class WSelectionBox;
    class WComboBox;
    class WMessageBox;
}
using namespace Wt;

///
/// \class ProjectChooser
/// \brief widget that is used for choosing a project to filter the list of MRIDs
///
class ProjectChooser : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ProjectChooser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ProjectChooser();

    ///
    /// Reset to default widget state
    ///
    void resetAll();

    ///
    /// Project chosen signal accessor
    ///
    Wt::Signal<std::string>& projectChosen()    {   return mProjectChosen;  }

    ///
    /// Project clicked signal accessor
    ///
    Wt::Signal<std::string>& projectClicked()   {   return mProjectClicked;  }

private:

    ///
    /// Handle message box finished [slot]
    ///
    void handleMessageBoxFinished(StandardButton);

    /// Project chosen [signal]
    Wt::Signal<std::string> mProjectChosen;

    /// Project chosen [signal]
    Wt::Signal<std::string> mProjectClicked;

    ///
    ///  Load button clicked [slot]
    ///
    void loadClicked();

    ///
    ///  Delete button clicked [slot]
    ///
    void deleteClicked();

    ///
    ///  Selection changed [slot]
    ///
    void selectionChanged();

    /// Project selection box
    WSelectionBox *mProjectSelectionBox;

    /// Delete button
    WPushButton *mDeleteButton;

    /// Message box dialog
    WMessageBox *mMessageBox;
};

#endif // PROJECTCHOOSER_H

