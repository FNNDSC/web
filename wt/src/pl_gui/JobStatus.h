//
//
//  Description:
//      This widget displays the current job status and for running jobs allows them to be killed
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef JOBSTATUS_H
#define JOBSTATUS_H

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <Wt/WContainerWidget>

namespace Wt
{
    class WApplication;
    class WLabel;
    class WPushButton;
    class WImage;
};

using namespace Wt;


///
/// \class JobStatus
/// \brief Displays the current job status and for running jobs allows them to be killed
///
class JobStatus :  public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    JobStatus(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~JobStatus();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    ///  Start updating
    ///
    void startUpdate();

    ///
    ///  Stop updating
    ///
    void stopUpdate();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Populate with script data
    ///
    void setJob(const std::string& clusterShFile, const std::string& metaScript,
                const std::string& jobId, const std::string& jobOwner);

private:

    // Status enumerant
    typedef enum
    {
        STATUS_UNSET = -1,
        STATUS_UNKNOWN,
        STATUS_WAITING,
        STATUS_QUEUED,
        STATUS_RUNNING,
        STATUS_COMPLETED_FAILURE,
        STATUS_COMPLETED_SUCCESS,

        NUM_STATUS

    } StatusEnum;

    ///
    ///  Kill button clicked [slot]
    ///
    void killButtonClicked();

    ///
    ///  Update thread for checking for status updates
    ///
    void updateStatus();


    /// Cluster sh file
    std::string mClusterShFile;

    /// Cluster sh file
    std::string mMetaScript;

    /// Job ID
    std::string mJobID;

    /// Job owner
    std::string mJobOwner;

    /// Label for status image
    WLabel *mStatusImage;

    /// Current status label
    WLabel *mStatusLabel;

    /// Kill button
    WPushButton *mKillButton;

    /// Application instance pointer
    WApplication *mApp;

    /// Thread for background processing
    boost::thread  *mThread;

    /// Current status
    volatile StatusEnum mCurStatus;

    /// Stop updating thread
    volatile bool mStopUpdateThread;

    /// Do update
    volatile bool mUpdateStatus;
};

#endif // JOBSTATUS_H

