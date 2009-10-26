//
//
//  Description:
//      Definition of Cluster job browser widget.  This widget provides
//      a browser for the currently schedule cluster jobs that are in
//      the schedule.log
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef CLUSTERJOBBROWSER_H
#define CLUSTERJOBBROWSER_H

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>
#include <Wt/WStandardItemModel>

#include <string>

using namespace Wt;

///
/// \class ClusterJobBrowser
/// \brief Provides a browser for all of the jobs in the cluster
///
class ClusterJobBrowser : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    ClusterJobBrowser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ClusterJobBrowser();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Signal accessor for cluster job selection
    ///
    Wt::Signal<std::string>& clusterJobSelected() { return mClusterJobSelected; }

private:

    ///
    ///  Cluster job selection changed by user
    ///
    void jobChanged();

    ///
    ///  Populate the Cluster job model by reading the schedule.log file.
    ///
    bool populateClusterJobs(const std::string& scheduleLogFile);

private:

    /// Signal for when a job is selected
    Wt::Signal<std::string> mClusterJobSelected;

    /// MRID Tree view
    WTreeView *mClusterJobTreeView;

    /// MRID Model
    WStandardItemModel *mClusterJobModel;

};

#endif // CLUSTERJOBBROWSER_H

