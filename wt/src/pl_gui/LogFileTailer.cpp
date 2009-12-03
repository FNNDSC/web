//
//
//  Description:
//      Implementation of LogFileTailer widget.  This widget provides
//      a text area that automatically tails a log file.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "LogFileTailer.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WGroupBox>
#include <Wt/WFont>
#include <Wt/WCssDecorationStyle>
#include <Wt/WTimer>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <iostream>
#include <string>

///
//  Maximum log size
//
const int MAX_LOG_SIZE = 16384;

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
LogFileTailer::LogFileTailer(const std::string& logFileName,
                             bool bgRed,
                             int updateMS,
                             bool showEnd,
                             WContainerWidget *parent) :
    WContainerWidget(parent),
    mLogFileName(logFileName),
    mShowEnd(showEnd),
    mUpdateMS(updateMS),
    mStopRequested(true),
    mApp(WApplication::instance())
{
    WVBoxLayout *layout = new WVBoxLayout();

    mLogFileGroupBox = new WGroupBox(path(logFileName).leaf());

    mLogFileTextArea = new WTextArea("");

    if (!bgRed)
    {
        mLogFileTextArea->setStyleClass("logdiv");
    }
    else
    {
        mLogFileTextArea->setStyleClass("logdivred");
    }
    mLogFileTextArea->decorationStyle().font().setFamily(WFont::Monospace);

    WGridLayout *vbox = new WGridLayout();
    vbox->addWidget(mLogFileTextArea, 0, 0);
    vbox->setRowStretch(0, -1);
    mLogFileGroupBox->setLayout(vbox);

    layout->addWidget(mLogFileGroupBox);

    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
LogFileTailer::~LogFileTailer()
{
    stopUpdate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default state
//
void LogFileTailer::resetAll()
{
    stopUpdate();
}

///
//  Start updating log
//
void LogFileTailer::startUpdate()
{
    mStopRequested = false;
    if (mThread == NULL)
    {
        mThread = new boost::thread(boost::bind(&LogFileTailer::updateLog, this));
    }
}

///
//  Stop updating log
//
void LogFileTailer::stopUpdate()
{
   mStopRequested = true;
   if (mThread != NULL)
   {
       delete mThread;
       mThread = NULL;
   }
}

///
//  Set lot file
//
void LogFileTailer::setLogFile(const std::string& logFileName)
{
    mLogFileName = logFileName;
    mLogFileGroupBox->setTitle(path(mLogFileName).leaf());
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Update timer ticked
//
void LogFileTailer::updateLog()
{
    while(!mStopRequested)
    {
        std::ifstream logFile(mLogFileName.c_str(), ios::in);
        if (logFile.is_open())
        {
            int maxRequestSize = MAX_LOG_SIZE;

            ostringstream oss;
            oss << logFile.rdbuf();

            stringstream logTextStream;
            logTextStream << oss.str();

            string logText = logTextStream.str();

            // Truncate the log.  Avoid exceeding the MAX HTTP request size.  Note that this is configurable
            // in wt_config.xml.  This is overly aggressive for the default setting, but I was having trouble
            // predicting the HTTP request size based on the string update.
            if (logText.length() > maxRequestSize)
            {
                if (mShowEnd)
                {
                    logText.erase(logText.begin(), logText.begin() + (logText.length() - maxRequestSize));
                }
                else
                {
                    logText.erase(logText.begin() + maxRequestSize, logText.end());
                }
            }

            // If the text has changed, then do an update
            if (WString(logText.c_str()) != mLogFileTextArea->text())
            {
                // First, take the lock to safely manipulate the UI outside of the
                // normal event loop, by having exclusive access to the session.
                WApplication::UpdateLock lock = mApp->getUpdateLock();

                if (mShowEnd == true || (mShowEnd == false && logText.length() > 1))
                {
                    mLogFileTextArea->setText(logText.c_str());
                }

                if (mShowEnd)
                {
                    //
                    // Little javascript trick to make sure we scroll along with new content
                    //
                    mApp->doJavaScript(mLogFileTextArea->jsRef() + ".scrollTop += "
                                                         + mLogFileTextArea->jsRef() + ".scrollHeight;");
                }

                mApp->triggerUpdate();
            }


            logFile.close();
        }
        else
        {
            // First, take the lock to safely manipulate the UI outside of the
            // normal event loop, by having exclusive access to the session.
            WApplication::UpdateLock lock = mApp->getUpdateLock();

            mLogFileTextArea->setText(WString("Couldn't open log file {1}").arg(mLogFileName));;

            mApp->triggerUpdate();

        }

        // Sleep for 1 second
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
}


