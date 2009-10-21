//
//
//  Description:
//      This is the widget that controls and lays out the entire "Subject" page.
//      This includes the MRID browser, Scan browser, and widgets to kick off
//      a pipeline analysis.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef SUBJECTPAGE_H
#define SUBJECTPAGE_H

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WText>
#include <string>

namespace Wt
{
    class WPushButton;
    class WStackedWidget;
}

using namespace Wt;

class SelectScans;
class PipelineConfigure;

///
/// \class SubjectPage
/// \brief Container widget for the contents of the "Subjects" tab
///
class SubjectPage : public WContainerWidget
{
public:

    // State of subject page
    typedef enum
    {
        SCAN_SELECT = 0,
        PIPELINE_CONFIGURE,

        NUM_STATES

    } SubjectState;

    ///
    /// Constructor
    ///
    SubjectPage(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~SubjectPage();

private:
    ///
    /// \brief Creates a title widget.
    ///
    WText *createTitle(const WString& title);


    ///
    /// \brief  Slot for when the scan is added to the queue
    ///
    void scanAdded(bool added);

    ///
    /// \brief  Slot for when the next button is clicked
    ///
    void nextClicked();

    ///
    /// \brief  Slot for when the back button is clicked
    ///
    void backClicked();

private:

    /// Current state of widget
    SubjectState mSubjectState;

    /// Stacked widget that holds the various stages
    WStackedWidget *mStackedStage;

    /// Select scans widget
    SelectScans *mSelectScans;

    /// Pipeline configure widget
    PipelineConfigure *mPipelineConfigure;

    /// Next button (navigation)
    WPushButton *mNextButton;

    /// Back button (navigation)
    WPushButton *mBackButton;

};

#endif // SUBJECTPAGE

