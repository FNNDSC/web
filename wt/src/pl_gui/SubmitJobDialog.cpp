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
#include "SubmitJobDialog.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTimer>
#include <Wt/WDialog>
#include <Wt/WLineEdit>

///
//  Namespaces
//
using namespace Wt;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
SubmitJobDialog::SubmitJobDialog(const WString &windowTitle) :
    WDialog(windowTitle)
{
	setModal(true);

	WText* dialogText = new WText("<B><center>Are you sure that you want to submit for processing?</center></B>");

	mCommandLineEdit = new WLineEdit("");
	mCommandLineLabelText = new WText("Command Arguments:");
	mCommandLineLabelText->setWordWrap(false);
	mCommandLineEdit->disable();
	mCommandLineEdit->setTextSize(70);

    WPushButton *yesButton = new WPushButton("Yes");
    WPushButton *noButton = new WPushButton("No");
    mAdvancedButton = new WPushButton("Edit Command");

    WHBoxLayout *buttonLayout = new WHBoxLayout();
    buttonLayout->addStretch(100);
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    buttonLayout->addStretch(100);

	WGridLayout *commandLayout = new WGridLayout();
	commandLayout->addWidget(mCommandLineLabelText, 0, 0, AlignMiddle);
	commandLayout->addWidget(mCommandLineEdit, 0, 1);
	commandLayout->addWidget(mAdvancedButton, 0, 2);
	commandLayout->setColumnStretch(1, 1);

    WVBoxLayout *dialogLayout = new WVBoxLayout();
    dialogLayout->addWidget(dialogText, AlignCenter | AlignMiddle);
    dialogLayout->addWidget(new WBreak);
    dialogLayout->addLayout(commandLayout);
    dialogLayout->addWidget(new WBreak);
    dialogLayout->addLayout(buttonLayout);

    contents()->setLayout(dialogLayout);

    yesButton->clicked().connect(SLOT(this, WDialog::accept));
    noButton->clicked().connect(SLOT(this, WDialog::reject));
    mAdvancedButton->clicked().connect(SLOT(this, SubmitJobDialog::editCommandLine));
    mCommandLineEdit->changed().connect(SLOT(this, SubmitJobDialog::commandLineChanged));

}

///
//  Destructor
//
SubmitJobDialog::~SubmitJobDialog()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//	Set the command-line string
//
void SubmitJobDialog::setCommandLine(const std::string& commandLine)
{
	mCommandLine = commandLine;
	mCommandLineEdit->setText(mCommandLine);
}

///
//	Get the command-line string
//
const std::string& SubmitJobDialog::getCommandLine() const
{
	return mCommandLine;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//	Edit the command line [slot]
//
void SubmitJobDialog::editCommandLine()
{
	mAdvancedButton->disable();
	mCommandLineEdit->enable();
}

///
//	Command Line Changed [slot]
//
void SubmitJobDialog::commandLineChanged()
{
	mCommandLine = mCommandLineEdit->text().toUTF8();
}
