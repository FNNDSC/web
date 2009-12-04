//
//
//  Description:
//      Definition of Cluster load chart widget.  This widget graphs
//      the current load of the cluster
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef CLUSTERLOADCHART_H
#define CLUSTERLOADCHART_H

#include <Wt/WContainerWidget>
#include <string>
#include <deque>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace Wt
{
    class PipelineApp;
    class WStandardItemModel;
    class WTimer;
    class WApplication;
    namespace Chart
    {
        class WCartesianChart;
    }
}

using namespace Wt;
using namespace Wt::Chart;

///
/// \class ClusterLoadCharter
/// \brief Provides a 2D graph of the current load of the cluster
///
class ClusterLoadChart : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    ClusterLoadChart(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ClusterLoadChart();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Start updating
    ///
    void startUpdate();

    ///
    /// Stop updating
    ///
    void stopUpdate();

private:

    ///
    ///  Threaded callback which handles doing updates to the chart
    ///
    void updateChart();

    ///
    /// Add a reading
    ///
    void addCPUReading(float value);

private:

    /// Model that holds the chart data
    WStandardItemModel *mModel;

    /// Chart to display data
    WCartesianChart *mChart;

    /// Timings queue
    std::deque<float> mCPUReadings;

    /// Utilization timers
    unsigned int mCPUUtilization[4];

    /// Application pointer
    WApplication *mApp;

    /// Thread for background processing
    boost::thread *mThread;

    /// Stop requested
   volatile bool mStopUpdateThread;

   /// Update chart
   volatile bool mUpdateChart;

};

#endif // CLUSTERLOADCHART_H

