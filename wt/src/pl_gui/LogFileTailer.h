//
//
//  Description:
//      Definition of LogFileTailer widget.  This widget provides
//      a text area that automatically tails a log file.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef LOGFILETAILER_H
#define LOGFILETAILER_H

#include <Wt/WContainerWidget>
#include <string>

namespace Wt
{
    class WTextArea;
    class WGroupBox;
    class WTimer;
}

using namespace Wt;

///
/// \class ClusterJobBrowser
/// \brief Provides a widget that automatically tails a given log file
///
class LogFileTailer : public WContainerWidget
{
public:
    ///
    /// Constructor
    ///
    LogFileTailer(const std::string& logFileName, bool bgRed, int updateMS = 1000, bool showEnd = true, WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~LogFileTailer();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Set lot file
    ///
    void setLogFile(const std::string& logFileName);

    ///
    ///  Update timer ticked
    ///
    void timerTick();

    ///
    ///  Start updating log
    ///
    void startUpdate();

    ///
    ///  Stop updating log
    ///
    void stopUpdate();


private:



private:

    /// Log file name
    std::string mLogFileName;

    /// Log file text area
    WTextArea *mLogFileTextArea;

    /// Group box container
    WGroupBox *mLogFileGroupBox;

    /// Timer for triggering updates
    WTimer *mTimer;

    /// Whether to show the end of the log
    bool mShowEnd;
};

#endif // LOGFILETAILER_H

