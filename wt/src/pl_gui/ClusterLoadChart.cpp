//
//
//  Description:
//      Implementation of Cluster load chart widget.  This widget graphs
//      the current load of the cluster
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ClusterLoadChart.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/Chart/WCartesianChart>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLogger>
#include <Wt/WDate>
#include <Wt/WTimer>
#include <Wt/WPushButton>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

///
//  Namespaces
//
using namespace Wt;
using namespace Wt::Chart;
using namespace std;
using namespace boost::filesystem;

///
//  Constants
//
const int NUM_SAMPLES = 40;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//


///
//  Constructor
//
ClusterLoadChart::ClusterLoadChart(WContainerWidget *parent) :
    WContainerWidget(parent),
    mStopUpdateThread(false),
    mUpdateChart(false),
    mThread(NULL)
{
    mApp = WApplication::instance();
    mApp->enableUpdates();

    mModel = new WStandardItemModel(NUM_SAMPLES, 2, this);
    mModel->setHeaderData(0, boost::any(WString("Time")));
    mModel->setHeaderData(1, boost::any(WString("CPU Load %")));

    for (int i = 0; i < NUM_SAMPLES; i++ )
    {
        mModel->setData(i, 0, boost::any(i));
        mModel->setData(i, 1, boost::any(0.0));
    }

    for (int i = 0; i < 4; i++)
    {
        mCPUUtilization[i] = 0;
    }

    mChart = new WCartesianChart();
    mChart->setModel(mModel);
    mChart->setXSeriesColumn(0);
    mChart->setLegendEnabled(false);
    mChart->setType(ScatterPlot);

    WDataSeries dataSeries(1, CurveSeries);
    dataSeries.setFillRange(MinimumValueFill);
    WPen dataPen = dataSeries.pen();
    dataPen.setColor(WColor("green"));
    dataSeries.setPen(dataPen);

    mChart->addSeries(dataSeries);
    mChart->resize(150, 25);
    mChart->setMargin(0, Top | Bottom);
    mChart->setMargin(0, Left | Right);
    mChart->setPlotAreaPadding(0, Top | Bottom | Left | Right);

    WAxis& xAxis = mChart->axis(XAxis);
    xAxis.setGridLinesEnabled(false);
    WAxis& yAxis = mChart->axis(YAxis);
    yAxis.setGridLinesEnabled(false);
    yAxis.setRange(0.0, 1.0);

    mChart->update();

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mChart, AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    mThread = new boost::thread(boost::bind(&ClusterLoadChart::updateChart, this));
}


///
//  Destructor
//
ClusterLoadChart::~ClusterLoadChart()
{

}

///
//  Reset to default state
//
void ClusterLoadChart::resetAll()
{
    stopUpdate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Start updating
//
void ClusterLoadChart::startUpdate()
{
    mUpdateChart = true;
}

///
//  Stop updating
//
void ClusterLoadChart::stopUpdate()
{
    mUpdateChart = false;
}

///
// Finalize the widget (pre-destruction)
//
void ClusterLoadChart::finalize()
{
    if (mThread != NULL)
    {
        mStopUpdateThread = true;
        mThread->join();
        delete mThread;
        mThread = NULL;
    }
}

///
/// Add a reading
///
void ClusterLoadChart::addCPUReading(float value)
{
    // Only store as many samples as are in the model
    while (mCPUReadings.size() >= mModel->rowCount())
    {
        mCPUReadings.pop_front();
    }

    mCPUReadings.push_back(value);

    // Initialized unused entries to 0
    int startRow = mModel->rowCount() - mCPUReadings.size();
    for(int i = 0; i < startRow; i++)
    {
        mModel->setData(i, 1, boost::any(0.0));
    }

    // Now set the model to the data that we have
    deque<float>::iterator iter(mCPUReadings.begin());
    int row = startRow;
    while (iter != mCPUReadings.end())
    {
        mModel->setData(row, 1, boost::any(*iter));
        row++;
        iter++;
    }

    // Set the color based on the latest reading
    WDataSeries& dataSeries = mChart->series(1);
    WPen dataPen = dataSeries.pen();
    WBrush dataBrush = dataSeries.brush();

    if (value >= 0.0 && value < 0.40)
    {
        dataPen.setColor(WColor("green"));
        dataBrush.setColor(WColor("green"));
    }
    else if (value >= 0.40 && value < 0.80)
    {
        dataPen.setColor(WColor("orange"));
        dataBrush.setColor(WColor("orange"));
    }
    else
    {
        dataPen.setColor(WColor("red"));
        dataBrush.setColor(WColor("red"));
    }
    dataSeries.setPen(dataPen);
    dataSeries.setBrush(dataBrush);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Threaded callback which handles doing updates to the chart
//
void ClusterLoadChart::updateChart()
{
    while (!mStopUpdateThread)
    {
        if (mUpdateChart)
        {
            std::ifstream inFile(getConfigOptionsPtrTS(mApp)->GetCPUUsageFile().c_str());
            if (inFile.is_open() && !inFile.eof())
            {
                char buf[1024] = {0};

                inFile.getline( buf, sizeof(buf));
                std::string token;

                istringstream istr( string(buf), ios_base::out);
                istr >> token;
                float usagePercent = 0.0;
                try
                {
                    usagePercent = boost::lexical_cast<float>(token);
                }
                catch(...)
                {
                    WApplication::instance()->log("warn") << "Could not cast value from file " <<  getConfigOptionsPtrTS(mApp)->GetCPUUsageFile() <<
                                                            " : " << token;
                    continue;
                }

                // First, take the lock to safely manipulate the UI outside of the
                // normal event loop, by having exclusive access to the session.
                WApplication::UpdateLock lock = mApp->getUpdateLock();

                addCPUReading(usagePercent / 100.0f);

                mChart->update();
                mApp->triggerUpdate();
            }
        }

        // Sleep for 1 second
        boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
    }
}


