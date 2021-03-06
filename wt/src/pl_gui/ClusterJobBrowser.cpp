//
//
//  Description:
//      Implementation of Cluster job browser widget.  This widget provides
//      a browser for the currently schedule cluster jobs that are in
//      the schedule.log
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ClusterJobBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WLogger>
#include <Wt/WDate>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <stack>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ClusterJobBrowser::ClusterJobBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    // Populate the list of MRIDs
    mClusterJobTreeView = new WTreeView();
    mClusterJobModel = new WStandardItemModel(this);

    mClusterJobTreeView->setModel(mClusterJobModel);
    mClusterJobTreeView->setSelectionMode(SingleSelection);
    mClusterJobTreeView->expandToDepth(1);
    mClusterJobTreeView->selectionChanged().connect(SLOT(this, ClusterJobBrowser::jobChanged));
    mClusterJobTreeView->setMinimumSize(300, WLength::Auto);
    mClusterJobTreeView->setHeaderHeight(0);

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mClusterJobTreeView);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
ClusterJobBrowser::~ClusterJobBrowser()
{
}

///
//  Reset to default state
//
void ClusterJobBrowser::resetAll()
{
    WModelIndexSet noSelection;
    mClusterJobTreeView->setSelectedIndexes(noSelection);
    mClusterJobModel->clear();

    string scheduleFileName = getConfigOptionsPtr()->GetClusterDir() + "/schedule.log";
    populateClusterJobs(scheduleFileName);

    if (mClusterJobModel->rowCount() == 0)
    {
        WStandardItem *newItem = new WStandardItem("NO JOBS FOUND");
        newItem->setFlags(newItem->flags().clear(ItemIsSelectable));
        newItem->setIcon("icons/folder.gif");
        mClusterJobModel->appendRow(newItem);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Populate the Cluster job model by reading the schedule.log file.
//
bool ClusterJobBrowser::populateClusterJobs(const std::string& scheduleLogFile)
{
    std::ifstream inFile(scheduleLogFile.c_str());
    stack<WStandardItem*> jobStack;

    if (!inFile.is_open())
    {
        WApplication::instance()->log("warn") << "WARNING: failed opening schedule log file for reading: " << scheduleLogFile;
        return false;
    }

    while (!inFile.eof())
    {
        char buf[1024];
        inFile.getline( buf, sizeof(buf));
        string token;

        string jobDesc = "",
               jobBash = "",
               jobStage = "";

        istringstream istr(string(buf), ios_base::out);

        int index = 0;
        while (getline(istr, token, '|'))
        {
            switch(index)
            {
            case 0:
                jobDesc = token;
                break;
            case 1:
                jobBash = token;
                boost::algorithm::trim(jobBash);
                break;
            case 2:
                {
                    // Formatted "Stage directory", strip out "Stage"
                    string tmp;
                    istringstream iss(token);
                    iss >> tmp;
                    iss >> jobStage;
                }
                break;
            default:
                break;
            }
            index++;
        }

        if (jobDesc != "")
        {
            string jobDate;
            string jobTime;
            string jobServer;
            string userName;

            istringstream iss(jobDesc);
            int index = 0;

            // Parse string of form "ddd MMM d hh:mm:ss TZN server"
            while(!iss.eof())
            {
                string tmp;
                iss >> tmp;

                if (index < 6)
                {
                    if (index != 3 && index != 4)
                    {
                        jobDate += tmp + " ";
                    }
                    else if (index != 4)
                    {
                        jobTime = tmp;
                    }
                }
                else if (index == 6)
                {
                    jobServer = tmp;
                }
                else if (index == 7)
                {
                    userName = tmp;
                }
                index++;
            }

            WStandardItem *newItem = new WStandardItem(jobDate + jobTime + " [" + userName + "]");
            newItem->setData(jobStage, UserRole);
            newItem->setData(jobBash, UserRole + 1);
            newItem->setIcon("icons/folder.gif");

            jobStack.push(newItem);
        }
    }

    while(!jobStack.empty())
    {
        mClusterJobModel->appendRow(jobStack.top());
        jobStack.pop();
    }

}


///
//  Cluster job selection changed by user
//
void ClusterJobBrowser::jobChanged()
{
    if (mClusterJobTreeView->selectedIndexes().empty())
        return;

    WModelIndex selected = *mClusterJobTreeView->selectedIndexes().begin();
    boost::any displayData = selected.data(DisplayRole);
    boost::any d = selected.data(UserRole);
    boost::any d1 = selected.data(UserRole + 1);

    if (!displayData.empty() && !d.empty())
    {
        string clusterScript = boost::any_cast<std::string>(d);
        string bashScript = boost::any_cast<std::string>(d1);

        std::string bashScriptPath = path(clusterScript).branch_path().string();
        bashScriptPath += string("/")  + bashScript;


        mClusterJobSelected.emit(bashScriptPath);
    }
}

