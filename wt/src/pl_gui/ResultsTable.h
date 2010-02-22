//
//
//  Description:
//      This is a widget that displays a sortable/filterable table of results.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef RESULTSTABLE_H
#define RESULTSTABLE_H

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <string>
#include <mxml.h>
#include <Wt/WStandardItemModel>

namespace Wt
{
    class WTreeView;
    class WSortFilterProxyModel;
}

class ResultsFilterProxyModel;

using namespace Wt;


///
/// \class ResultsPage
/// \brief Container widget that displays a sortable/filterable table of results
///
class ResultsTable : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ResultsTable(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ResultsTable();

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
    /// Signal new result selected (double click)
    ///
    Wt::Signal<std::string, std::string>& resultSelected()  {   return mResultSelected; }

private:

    ///
    /// Called when item is selected form list [slot]
    ///
    void jobSelected(const WModelIndex&);

    ///
    ///  Populate results table by reading the schedule.log.xml file
    ///
    bool populateResultsTable();

    ///
    ///  Set data in the model from an XML node
    ///
    void setDataColumn(mxml_node_t *node, const char* name, int row, int col, int role=EditRole);


private:

    /// Result selected signal
    Wt::Signal<std::string, std::string> mResultSelected;

    /// Model representing cluster jobs
    WStandardItemModel *mModel;

    /// Tree view for displaying of cluster jobs
    WTreeView *mTreeView;

    /// Sort/Filter proxy model for table
    ResultsFilterProxyModel *mSortFilterProxyModel;
};

#endif // RESULTSTABLE_H
