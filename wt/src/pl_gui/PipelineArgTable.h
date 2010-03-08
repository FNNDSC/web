//
//
//  Description:
//      This widget displays a table of the arguments passed to the pipeline.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEARGTABLE_H
#define PIPELINEARGTABLE_H

#include <Wt/WContainerWidget>

namespace Wt
{
    class WStandardItemModel;
    class WTreeView;
    class WSortFilterProxyModel;
};

using namespace Wt;


///
/// \class PipelineArgTable
/// \brief Widget displays a table of the arguments passed to the pipelin
///
class PipelineArgTable :  public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    PipelineArgTable(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineArgTable();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Populate with script data
    ///
    void setScript(std::string metaScript, std::string arguments);

private:

    /// Model representing cluster jobs
    WStandardItemModel *mModel;

    /// Tree view for displaying of cluster jobs
    WTreeView *mTreeView;

    /// Sort/Filter proxy model for table
    WSortFilterProxyModel *mSortFilterProxyModel;

};

#endif // PIPELINEARGTABLE_H

