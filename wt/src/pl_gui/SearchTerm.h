//
//
//  Description:
//      This is a widget which contains a search term (used by ProjectCreator)
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef SEARCHTERM_H
#define SEARCHTERM_H

#include <Wt/WContainerWidget>
#include <string>

namespace Wt
{
    class WComboBox;
    class WLineEdit;
    class WPushButton;
}
using namespace Wt;

///
/// \class SearchTerm
/// \brief Widget which contains a search term (used by ProjectCreator)
///
class SearchTerm : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    SearchTerm(int uniqueID, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~SearchTerm();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Copy values from another search term
    ///
    void copySearchTerm(const SearchTerm* src);

    ///
    /// Search add or remove [signal]
    ///
    Wt::Signal<bool, int>& searchAddOrRemove()  {   return mSearchAddOrRemove;  }

    ///
    ///  Get the search term as XML for storage
    ///
    std::string getAsXML();

    ///
    /// Set the search term from the XML data
    ///
    void setFromXMLData(const std::string& field, const std::string &type, const std::string &expr);

protected:

    ///
    /// Plus clicked [slot]
    ///
    void plusClicked();

    ///
    /// Minus clicked [slot]
    ///
    void minusClicked();

    /// Search add or remove
    Wt::Signal<bool, int> mSearchAddOrRemove;

    /// Unique ID
    int mUniqueID;

    /// Search field
    WComboBox *mSearchField;

    /// Search type
    WComboBox *mSearchType;

    /// Search string
    WLineEdit *mSearchString;

    /// Plus button
    WPushButton *mPlusButton;

    /// Minus button
    WPushButton *mMinusButton;
};

#endif // SEARCHTERM_H

