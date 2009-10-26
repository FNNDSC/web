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
#include <fstream>
#include <iostream>
#include <string>

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
                             int updateMS,
                             WContainerWidget *parent) :
    WContainerWidget(parent),
    mLogFileName(logFileName)
{
    WVBoxLayout *layout = new WVBoxLayout();

    mLogFileGroupBox = new WGroupBox(path(logFileName).leaf());

    mLogFileTextArea = new WTextArea("");
    mLogFileTextArea->setStyleClass("logdiv");
    mLogFileTextArea->decorationStyle().font().setFamily(WFont::Monospace);

    WVBoxLayout *vbox = new WVBoxLayout();
    vbox->addWidget(mLogFileTextArea);
    mLogFileGroupBox->setLayout(vbox);

    layout->addWidget(mLogFileGroupBox);

    setLayout(layout);

    // Create update timer
    mTimer = new WTimer();
    mTimer->setInterval(updateMS);
    mTimer->timeout().connect(SLOT(this, LogFileTailer::timerTick));

    resetAll();
}

///
//  Destructor
//
LogFileTailer::~LogFileTailer()
{
    delete mTimer;
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

}

///
//  Start updating log
//
void LogFileTailer::startUpdate()
{
    // Do first update now
    timerTick();
    mTimer->start();
}

///
//  Stop updating log
//
void LogFileTailer::stopUpdate()
{
    mTimer->stop();
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
void LogFileTailer::timerTick()
{
    std::ifstream logFile(mLogFileName.c_str(), ios::in);
    if (logFile.is_open())
    {
        ostringstream oss;

        oss << logFile.rdbuf();

        stringstream logText;
        logText << oss.str();

        // If the text has changed, then do an update
        if (WString(logText.str()) != mLogFileTextArea->text())
        {
            mLogFileTextArea->setText(logText.str().c_str());
            logFile.close();

            // First, take the lock to safely manipulate the UI outside of the
            // normal event loop, by having exclusive access to the session.
            WApplication::UpdateLock lock = WApplication::instance()->getUpdateLock();

            //
            // Little javascript trick to make sure we scroll along with new content
            //
            WApplication::instance()->doJavaScript(mLogFileTextArea->jsRef() + ".scrollTop += "
                                                 + mLogFileTextArea->jsRef() + ".scrollHeight;");

            WApplication::instance()->triggerUpdate();
        }
    }
    else
    {
        mLogFileTextArea->setText(WString("Couldn't open log file {1}").arg(mLogFileName));;
    }
}
