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

#include <Wt/WContainerWidget>

namespace Wt
{
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
    /// Populate with script data
    ///
    void setJob(const std::string& clusterShFile, const std::string& metaScript,
                const std::string& jobId, const std::string& jobOwner);

private:

    ///
    ///  Kill button clicked [slot]
    ///
    void killButtonClicked();


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
};

#endif // JOBSTATUS_H

