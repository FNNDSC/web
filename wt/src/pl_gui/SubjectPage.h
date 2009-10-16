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

using namespace Wt;

class MRIBrowser;
class ScanBrowser;

///
/// \class SubjectPage
/// \brief Container widget for the contents of the "Subjects" tab
///
class SubjectPage : public WContainerWidget
{
public:
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
    /// \brief Slot for when the MRI selection changes
    ///
    void mriChanged(std::string scanDir);

private:
    /// MRI Browser widget
    MRIBrowser *mMRIBrowser;

    /// Scan browser widget
    ScanBrowser *mScanBrowser;

};

#endif // SUBJECTPAGE

