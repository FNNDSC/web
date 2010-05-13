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
#include "JobStatus.h"
#include "GlobalEnums.h"
#include "PipelineApp.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WImage>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/process/iostreams_pipe_end_traits.hpp>
#include <boost/process/pipe_end.hpp>
#include <boost/iostreams/stream.hpp>


///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::processes;
using namespace boost::iostreams;
using namespace boost::filesystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
JobStatus::JobStatus(WContainerWidget *parent) :
    WContainerWidget(parent),
    mStopUpdateThread(false),
    mUpdateStatus(false),
    mApp(WApplication::instance())
{
    setStyleClass("tabdiv");

    WGroupBox *statusBox = new WGroupBox("Job Status");
    statusBox->setStyleClass("groupdiv");

    WGridLayout *statusLayout = new WGridLayout();
    mStatusImage = new WLabel("");
    mStatusLabel = new WLabel("");
    mKillButton = new WPushButton("Kill Job");
    statusLayout->addWidget(mStatusImage, 0, 0, AlignLeft | AlignMiddle);
    statusLayout->addWidget(mStatusLabel, 0, 1, AlignLeft | AlignMiddle);
    statusLayout->addWidget(mKillButton, 0, 2, AlignLeft | AlignMiddle);
    statusLayout->setColumnStretch(2, 2);
    statusBox->setLayout(statusLayout);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(statusBox, 0, 0);

    setLayout(layout);

    mKillButton->clicked().connect(SLOT(this, JobStatus::killButtonClicked));

    mThread = new boost::thread(boost::bind(&JobStatus::updateStatus, this));

    resetAll();

}

///
//  Destructor
//
JobStatus::~JobStatus()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//


///
//  Start updating
//
void JobStatus::startUpdate()
{
    mUpdateStatus = true;
}

///
//  Stop updating
//
void JobStatus::stopUpdate()
{
    mUpdateStatus = false;
}

///
// Finalize the widget (pre-destruction)
//
void JobStatus::finalize()
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
//  Reset to default state
//
void JobStatus::resetAll()
{
    mStatusImage->setImage(NULL);
    mStatusLabel->setText("");
    mKillButton->disable();

    mCurStatus = STATUS_UNSET;

    mClusterShFile = "";
    mMetaScript = "";
    mJobID = "";
    mJobOwner = "";

    stopUpdate();
}

///
//  Populate table with data
//
void JobStatus::setJob(const std::string& clusterShFile, const std::string& metaScriptFile,
                       const std::string& jobID, const std::string& jobOwner)
{
    boost::mutex::scoped_lock lock(mUpdateMutex);

    resetAll();

    mClusterShFile = clusterShFile;
    mMetaScript = metaScriptFile;
    mJobID = jobID;
    mJobOwner = jobOwner;

    mStatusImage->setImage(new WImage("icons/ajax-loader-trans.gif"));
    mStatusLabel->setText("Determing status...");

    // Trigger the thread to start updating
    startUpdate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Kill button clicked [slot]
//
void JobStatus::killButtonClicked()
{
    // Run the cluster kill script to kill the job
    std::string cmdToExecute;
    cmdToExecute = "cluster_kill.bash -J " + getConfigOptionsPtr()->GetJobIDPrefix() + mJobID;
    cmdToExecute += " -C "+ getConfigOptionsPtr()->GetClusterType();
    if (getConfigOptionsPtr()->GetClusterHeadNode() != "")
    {
        cmdToExecute += " -r " + getConfigOptionsPtr()->GetClusterHeadNode();
    }

    context ctx;
    ctx.add(capture_stream(stdout_fileno));
    ctx.environment = current_environment();
    ctx.environment["PATH"] = getConfigOptionsPtr()->GetScriptDir() + ":/bin:/usr/bin:/usr/local/bin:/opt/local/bin:" + ctx.environment["PATH"];

    child c = launch_shell(cmdToExecute, ctx);
    boost::processes::status s = c.wait();

    mKillButton->disable();
}


///
//  Update thread for checking for status updates
//
void JobStatus::updateStatus()
{
    while (!mStopUpdateThread)
    {
        if (mUpdateStatus)
        {
            // Lock mutex for safety again JobStatus::setJob()
            bool updateStatus = false;
            mUpdateMutex.lock();

            if (mClusterShFile != "" && mJobID != "")
            {
                // Cluster script dir
                std::string metaScriptLog = path(mClusterShFile).branch_path().string() + "/" + mMetaScript + ".std";

                // Run the cluster status script to get the status of the job
                // Determine the password salt by using ypmatch to get
                std::string cmdToExecute;
                cmdToExecute = "cluster_status.bash -J " + getConfigOptionsPtrTS(mApp)->GetJobIDPrefix() + mJobID;
                cmdToExecute += " -C "+ getConfigOptionsPtrTS(mApp)->GetClusterType();
                if (getConfigOptionsPtrTS(mApp)->GetClusterHeadNode() != "")
                {
                    cmdToExecute += " -r " + getConfigOptionsPtrTS(mApp)->GetClusterHeadNode();
                }

                context ctx;
                ctx.add(capture_stream(stdout_fileno));
                ctx.environment = current_environment();
                ctx.environment["PATH"] = getConfigOptionsPtrTS(mApp)->GetScriptDir() + ":/bin:/usr/bin:/usr/local/bin:/opt/local/bin:" + ctx.environment["PATH"];

                child c = launch_shell(cmdToExecute, ctx);
                boost::processes::status s = c.wait();

                // Get the returned encoded password
                stream<boost::processes::pipe_end> is(c.get_stdout());
                std::string process;
                std::string status;

                StatusEnum curStatus = STATUS_UNSET;


                try
                {
                    is >> process;
                    is >> status;

                    if (status != "")
                    {
                        if (status == "QUEUED")
                        {
                            curStatus = STATUS_QUEUED;
                        }
                        else
                        {
                            curStatus = STATUS_RUNNING;
                        }
                    }
                    else
                    {
                        curStatus = STATUS_UNKNOWN;
                    }
                }
                catch(...)
                {
                    curStatus = STATUS_UNKNOWN;
                }

                // See if we can figure out what the status of the job is
                // by looking at the meta log file
                if (curStatus == STATUS_UNKNOWN)
                {
                    if (!exists(metaScriptLog))
                    {
                        curStatus = STATUS_WAITING;
                    }
                    else
                    {
                        // Use the following command to get the result code from the script:
                        //  tail -2 <filename> | grep code | awk '{print $5}'
                        // This looks at the "Shutting down with code #..." at the bottom of the script
                        cmdToExecute = "tail -2 " + metaScriptLog + "| grep code | awk '{print $5}'";

                        child c = launch_shell(cmdToExecute, ctx);
                        boost::processes::status s = c.wait();

                        // Get the result code
                        stream<boost::processes::pipe_end> is(c.get_stdout());

                        try
                        {
                            std::string returnCode;
                            is >> returnCode;

                            if (returnCode == "0")
                            {
                                curStatus = STATUS_COMPLETED_SUCCESS;
                            }
                            else
                            {
                                curStatus = STATUS_COMPLETED_FAILURE;
                            }
                        }
                        catch(...)
                        {
                            curStatus = STATUS_COMPLETED_FAILURE;
                        }
                    }
                }

                if (curStatus != mCurStatus)
                {
                    updateStatus = true;
                    mCurStatus = curStatus;
                }
	        }

            // Unlock mutex before doing mApp->getUpdateLock().  This is crucial because
            // otherwise a deadlock can occur from setJob() locking this same mutex.
            mUpdateMutex.unlock();
            if (updateStatus == true)
            {
                // First, take the lock to safely manipulate the UI outside of the
                // normal event loop, by having exclusive access to the session.
                WApplication::UpdateLock lock = mApp->getUpdateLock();
 
                switch (mCurStatus)
                {
                case STATUS_UNKNOWN:
                    mStatusLabel->setText("UNKNOWN");
                    mStatusImage->setImage(new WImage("icons/lg-alertY-glass.png"));
                    break;
                case STATUS_RUNNING:
                case STATUS_QUEUED:
                    if (mCurStatus == STATUS_RUNNING)
                        mStatusLabel->setText("RUNNING");
                    else
                        mStatusLabel->setText("QUEUED");
                    mStatusImage->setImage(new WImage("icons/lg-alertO-glass.png"));
                    if (getCurrentUserName() == mJobOwner)
                    {
                        mKillButton->enable();
                    }
                    break;
                case STATUS_WAITING:
                    mStatusLabel->setText("WAITING TO BE QUEUED");
                    mStatusImage->setImage(new WImage("icons/lg-alertO-glass.png"));
                    break;
                case STATUS_COMPLETED_SUCCESS:
                    mStatusLabel->setText("COMPLETED (SUCCESS)");
                    mStatusImage->setImage(new WImage("icons/lg-success-glass.png"));
                    break;
                case STATUS_COMPLETED_FAILURE:
                    mStatusLabel->setText("COMPLETED (FAILURE)");
                    mStatusImage->setImage(new WImage("icons/lg-failed-glass.png"));
                    break;
                default:
                    // Unset state, don't display anything
                    break;
                }

                mApp->triggerUpdate();
            }
        }

        // Sleep for 1 second
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
}

