//
//
//  Description:
//      This is a filter proxy model for the ResultsTable widget.  It provides custom filtering
//      to allow searches on any field.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef RESULTSFILTERPROXYMODEL_H
#define RESULTSFILTERPROXYMODEL_H

#include <Wt/WSortFilterProxyModel>
#include <Wt/WRegExp>
#include <string>


using namespace Wt;


///
/// \class ResultsFilterProxyModel
/// \brief This is a filter proxy model for the ResultsTable widget.  It provides custom filtering
///        to allow searches on any field.
///
class ResultsFilterProxyModel : public WSortFilterProxyModel
{
public:

    ///
    /// Constructor
    ///
    ResultsFilterProxyModel(WObject *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ResultsFilterProxyModel();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Set user filter
    ///
    void setUserFilter(bool filter, std::string userFilter = "");

    ///
    /// Set search term
    ///
    void setSearchTerm(bool search, std::string searchTerm = "");

    ///
    /// Set project filter
    ///
    void setProjectFilter(bool filter, std::string projectFilter = "");

    ///
    /// Set user column
    ///
    void setUserColumn(int col);

    ///
    ///  Set user column
    ///
    void setProjectColumnAndRole(int col, int role);


protected:

    ///
    /// Custom filter, override base class implementation
    ///
    virtual bool filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const;

private:

    /// User filter enabled
    bool mUserFilter;

    /// Search Term enabled
    bool mSearchTerm;

    /// Project filter enabled
    bool mProjectFilter;

    /// Location of user column in model
    int mUserColumn;

    /// Location of the project column in model
    int mProjectColumn;

    /// Role of project in model column
    int mProjectRole;

    /// User filter
    WRegExp mUserFilterRegExp;

    /// Search term
    WRegExp mSearchTermRegExp;

    ///Project filter
    WRegExp mProjectFilterRegExp;

};

#endif // RESULTSFILTERPROXYMODEL_H
