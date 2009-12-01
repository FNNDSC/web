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

namespace Wt
{
    class WStandardItemModel;
    class WTimer;
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
    /// Start updating
    ///
    void startUpdate();

    ///
    /// Stop updating
    ///
    void stopUpdate();

private:

    ///
    /// Timer tick
    ///
    void timerTick();

    ///
    /// Add a reading
    ///
    void addCPUReading(float value);

private:

    /// Model that holds the chart data
    WStandardItemModel *mModel;

    /// Chart to display data
    WCartesianChart *mChart;

    /// Timer
    WTimer *mTimer;

    /// Timings queue
    std::deque<float> mCPUReadings;

    /// Utilization timers
    unsigned int mCPUUtilization[4];
};

#endif // CLUSTERLOADCHART_H

