//
//
//  Description:
//      This is a dialog box for submitting jobs to the cluster.  It allows for configuration
//		of the command-line parameters for the job.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef SUBMITJOBDIALOG_H
#define SUBMITJOBDIALOG_H

#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <string>

namespace Wt
{
    class WText;
    class WLineEdit;
    class WPushButton;
}
using namespace Wt;

///
/// \class SubmitJobDialog
/// \brief Dialog box for submit scans to cluster that allows editing of
///		   command-line options for job
///
class SubmitJobDialog : public WDialog
{
public:
    ///
    /// Constructor
    ///
	SubmitJobDialog(const WString& windowTitle);

    ///
    /// Destructor
    ///
    virtual ~SubmitJobDialog();

    ///
    ///	Set the command-line string
    ///
    void setCommandLine(const std::string& commandLine);

    ///
    ///	Get the command-line string
    ///
    const std::string& getCommandLine() const;

private:

    ///
    ///	Edit the command line [slot]
    ///
    void editCommandLine();

    ///
    ///	Command Line Changed [slot]
    ///
    void commandLineChanged();


private:

    /// Command line
    std::string mCommandLine;

    /// Command line editor
    WLineEdit *mCommandLineEdit;

    ///	Advanced button
    WPushButton *mAdvancedButton;

    /// Command line edit text label
    WText *mCommandLineLabelText;
};

#endif // SUBMITJOBDIALOG_H

