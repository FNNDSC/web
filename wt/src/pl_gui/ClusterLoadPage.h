//
//
//  Description:
//      This is the widget that controls and lays out the entire "Cluster Load" page.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef CLUSTERLOADPAGE_H
#define CLUSTERLOADPAGE_H

#include <Wt/WContainerWidget>

class LogFileTailer;

using namespace Wt;


///
/// \class ClusterLoadPage
/// \brief Container widget for the contents of the "Cluster Load" tab
///
class ClusterLoadPage : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    ClusterLoadPage(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ClusterLoadPage();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    ///  Start updating logs
    ///
    void startUpdate();

    ///
    ///  Stop updating logs
    ///
    void stopUpdate();

private:

    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);

private:

    /// Top log file tailer
    LogFileTailer *mTopFileTailer;
};

#endif // CLUSTERLOADPAGE_H

