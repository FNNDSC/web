//
//
//  Description:
//      This is a widget that is used for choosing a project to filter the list of
//      MRIDs
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ProjectPage.h"
#include "ProjectChooser.h"
#include "ProjectCreator.h"
#include "ConfigOptions.h"
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
#include <Wt/WImage>
#include <signal.h>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/process/iostreams_pipe_end_traits.hpp>
#include <boost/process/pipe_end.hpp>
#include <boost/iostreams/stream.hpp>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/des.h>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::processes;
using namespace boost::iostreams;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ProjectPage::ProjectPage(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("maindiv");


    mCurrentUserLabel = new WLabel("");
    mCurrentUserLabel->setStyleClass("projectdiv");
    mCurrentUserLabel->setWordWrap(false);

    WGridLayout *logoutLayout = new WGridLayout();
    WPushButton *logoutButton = new WPushButton("Logout");
    logoutLayout->addWidget(mCurrentUserLabel, 0, 0, AlignRight | AlignMiddle);
    logoutLayout->addWidget(logoutButton, 0, 1, AlignRight | AlignMiddle);

    WContainerWidget *topContainer = new WContainerWidget();
    WGridLayout *topLayout = new WGridLayout();
    topContainer->setStyleClass("topdiv");
    WImage *chbLogo = new WImage(tr("logo-image").toUTF8());
    chbLogo->setStyleClass("titlediv");
    topLayout->addWidget(chbLogo, 0, 0, Wt::AlignLeft);
    WLabel *titleLabel = new WLabel(tr("page-top-text"));
    titleLabel->setStyleClass("titlediv");
    topLayout->addWidget(titleLabel, 0, 1, Wt::AlignCenter | Wt::AlignMiddle);
    topLayout->addLayout(logoutLayout, 0, 2, Wt::AlignRight | Wt::AlignMiddle);
    topContainer->setLayout(topLayout);
	
    mProjectChooser = new ProjectChooser();
    mProjectChooser->resize(512, 512);
    mProjectChooser->setMinimumSize(WLength(512, WLength::Pixel),
                                    WLength(512, WLength::Pixel));

    mProjectChooser->setMaximumSize(WLength(512, WLength::Pixel),
                                    WLength(512, WLength::Pixel));
    mProjectCreator = new ProjectCreator();
    mProjectCreator->resize(512, 512);
    mProjectCreator->setMinimumSize(WLength(512, WLength::Pixel),
                                    WLength(512, WLength::Pixel));
    mProjectCreator->setMaximumSize(WLength(512, WLength::Pixel),
                                    WLength(512, WLength::Pixel));


    WGridLayout *centerLayout = new WGridLayout();
    centerLayout->addWidget(mProjectChooser, 0, 0, AlignRight);
    centerLayout->addWidget(mProjectCreator, 0, 1, AlignLeft);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(topContainer, 0, 0);
    layout->addWidget(new WText(tr("project-selection")), 1, 0);
    layout->addLayout(centerLayout, 2, 0, AlignTop);
    layout->setRowStretch(0, -1);
    layout->setRowStretch(1, -1);
    layout->setRowStretch(2, 1);
    setLayout(layout);

    // Connections
    logoutButton->clicked().connect(SLOT(this, ProjectPage::logoutClicked));
    mProjectChooser->projectChosen().connect(SLOT(this, ProjectPage::projectLoad));
    mProjectChooser->projectClicked().connect(SLOT(mProjectCreator, ProjectCreator::setFromXML));
    mProjectCreator->projectAdded().connect(SLOT(this, ProjectPage::projectAdded));
}

///
//  Destructor
//
ProjectPage::~ProjectPage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default state
//
void ProjectPage::resetAll()
{
    mProjectChooser->resetAll();
    mProjectCreator->resetAll();
    mCurrentUserLabel->setText(getCurrentUserName());
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
/// Project load clicked [slot]
///
void ProjectPage::projectLoad(std::string path)
{
    mProjectChosen.emit(path);
}


///
/// Project added[slot]
///
void ProjectPage::projectAdded(std::string projectName)
{
    mProjectChooser->resetAll();
    mProjectCreator->resetAll();
}

///
/// Logout clicked [slot]
///
void ProjectPage::logoutClicked()
{
    mLogoutUser.emit();
}
