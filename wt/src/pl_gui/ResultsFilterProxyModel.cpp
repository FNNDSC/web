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
#include "ResultsFilterProxyModel.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <boost/algorithm/string.hpp>

///
//  Namespaces
//
using namespace Wt;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ResultsFilterProxyModel::ResultsFilterProxyModel(WObject *parent) :
    WSortFilterProxyModel(parent),
    mUserColumn(0)
{
    resetAll();
}

///
//  Destructor
//
ResultsFilterProxyModel::~ResultsFilterProxyModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
// Reset all widgets to the default state
//
void ResultsFilterProxyModel::resetAll()
{
    mUserFilter = false;
    mSearchTerm = false;
    mUserFilterRegExp.setPattern(".*");
    mSearchTermRegExp.setPattern(".*");
}

///
// Set user filter
//
void ResultsFilterProxyModel::setUserFilter(bool filter, std::string userFilter)
{
    mUserFilter = filter;
    mUserFilterRegExp.setPattern(userFilter);

}

///
// Set search term
//
void ResultsFilterProxyModel::setSearchTerm(bool search, std::string searchTerm)
{
    mSearchTerm = search;
    mSearchTermRegExp.setPattern(searchTerm);
}

///
//  Set user column
//
void ResultsFilterProxyModel::setUserColumn(int col)
{
    mUserColumn = col;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
// Custom filter, override base class implementation
//
bool ResultsFilterProxyModel::filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const
{
    // Quick check to see if any filters are on
    if (mUserFilter == false && mSearchTerm == false)
        return true;

    bool matchedUser = false;
    bool matchedSearch = false;
    for(int col = 0; col < sourceModel()->columnCount(); col++)
    {
        boost::any data = sourceModel()->index(sourceRow, col, sourceParent).data(filterRole());

        if (!data.empty())
        {
            WString s = asString(data);

            if (col == mUserColumn && mUserFilter)
            {
                matchedUser = mUserFilterRegExp.exactMatch(s);
            }
            else if(col != mUserColumn && mSearchTerm)
            {
                std::string searchPattern = mSearchTermRegExp.pattern().toUTF8();
                std::string searchTarget = s.toUTF8();
                bool searchResult = mSearchTermRegExp.exactMatch(s) || boost::algorithm::icontains(searchTarget, searchPattern);
                if (searchResult)
                {
                    matchedSearch = true;
                }
            }
        }
    }

    if (mUserFilter)
    {
        if (mSearchTerm)
        {
            return matchedUser && matchedSearch;
        }
        else
        {
            return matchedUser;
        }
    }
    else
    {
        return matchedSearch;
    }
}




