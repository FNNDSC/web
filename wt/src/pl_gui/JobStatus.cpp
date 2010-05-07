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
    WContainerWidget(parent)
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
//  Reset to default state
//
void JobStatus::resetAll()
{
    mStatusImage->setImage(NULL);
    mStatusLabel->setText("");
    mKillButton->disable();

    mClusterShFile = "";
    mMetaScript = "";
    mJobID = "";
}

///
//  Populate table with data
//
void JobStatus::setJob(const std::string& clusterShFile, const std::string& metaScriptFile,
                       const std::string& jobID)
{
    mClusterShFile = clusterShFile;
    mMetaScript = metaScriptFile;
    mJobID = jobID;

    // Cluster script dir
    std::string metaScriptLog = path(clusterShFile).branch_path().string() + "/" + metaScriptFile + ".std";

    // Run the cluster status script to get the status of the job
    // Determine the password salt by using ypmatch to get
    std::string cmdToExecute;
    cmdToExecute = "cluster_status.bash -J " + getConfigOptionsPtr()->GetJobIDPrefix() + jobID;
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

    // Get the returned encoded password
    stream<boost::processes::pipe_end> is(c.get_stdout());
    std::string process;
    std::string status;


    try
    {
        is >> process;
        is >> status;

        if (status != "")
        {
            mStatusLabel->setText(status);
        }
        else
        {
            mStatusLabel->setText("UNKNOWN");
        }
        if (status == "RUNNING")
        {
            mKillButton->enable();
            mStatusImage->setImage(new WImage("icons/lg-alertO-glass.png"));
        }
    }
    catch(...)
    {
        mStatusLabel->setText("UNKNOWN");
        mStatusImage->setImage(new WImage("icons/lg-alertY-glass.png"));
    }

    // See if we can figure out what the status of the job is
    // by looking at the meta log file
    if (mStatusLabel->text() == "UNKNOWN")
    {
        if (!exists(metaScriptLog))
        {
            mStatusLabel->setText("WAITING TO BE QUEUED");
            mStatusImage->setImage(new WImage("icons/lg-alertY-glass.png"));
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
                    mStatusLabel->setText("COMPLETED (SUCCESS)");
                    mStatusImage->setImage(new WImage("icons/lg-success-glass.png"));
                }
                else
                {
                    mStatusLabel->setText("COMPLETED (FAILURE)");
                    mStatusImage->setImage(new WImage("icons/lg-failed-glass.png"));
                }
            }
            catch(...)
            {
                mStatusLabel->setText("COMPLETED (FAILURE)");
                mStatusImage->setImage(new WImage("icons/lg-failed-glass.png"));
            }
        }
    }
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

    setJob(mClusterShFile, mMetaScript, mJobID);
}
