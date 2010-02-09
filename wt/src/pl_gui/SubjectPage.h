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
#include <Wt/WDialog>
#include <string>

namespace Wt
{
    class WPushButton;
    class WStackedWidget;
    class WMessageBox;
}

using namespace Wt;

class SelectScans;
class PipelineConfigure;
class MRIBrowser;
class SubmitJobDialog;

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

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Get the MRI browser
    ///
    const MRIBrowser* getMRIBrowser() const;

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

    ///
    ///  Submit scans for processing.  This function will generate the file and
    ///  execute pl_batch.bash on it to put it into the processing queue.
    ///
    bool submitForProcessing(const std::string& pipelineCommandLineString);

    ///
    ///  Return the number of lines in a file
    ///
    int getNumberOfLines(const char *fileName);

    ///
    ///  Handle message box finished [slot]
    ///
    void handleMessageBoxFinished(StandardButton);

    ///
    /// Handle submit scans to cluster [slot]
    ///
    void handleSubmitScans(WDialog::DialogCode dialogCode);

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

    /// Box for displaying messages
    WMessageBox *mMessageBox;

    /// Submit job dialog
    SubmitJobDialog *mSubmitJobDialog;
};

#endif // SUBJECTPAGE

