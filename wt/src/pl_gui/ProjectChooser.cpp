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
#include "ProjectChooser.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WSelectionBox>
#include <Wt/WText>
#include <Wt/WTimer>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WGroupBox>
#include <Wt/WMessageBox>
#include <signal.h>
#include <boost/filesystem.hpp>
///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ProjectChooser::ProjectChooser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("maindiv");

    WGroupBox *mainBox = new WGroupBox("Select Existing Project");
    mainBox->setStyleClass("groupdiv");

    mProjectSelectionBox = new WSelectionBox();
    mProjectSelectionBox->setSelectionMode(SingleSelection);

    WPushButton *loadButton = new WPushButton("Load");
    mDeleteButton = new WPushButton("Delete");
    WGridLayout *buttonLayout = new WGridLayout();
    buttonLayout->addWidget(loadButton, 0, 0, AlignCenter | AlignMiddle);
    buttonLayout->addWidget(mDeleteButton, 0, 1, AlignCenter | AlignMiddle);

    WGridLayout *boxLayout = new WGridLayout();
    boxLayout->addWidget(new WText("Projects:"), 0, 0);
    boxLayout->addWidget(mProjectSelectionBox, 1, 0);
    boxLayout->addLayout(buttonLayout, 2, 0, AlignCenter);
    boxLayout->setRowStretch(0, -1);
    boxLayout->setRowStretch(1, 1);
    boxLayout->setRowStretch(2, -1);
    mainBox->setLayout(boxLayout);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mainBox, 0, 0);
    setLayout(layout);

    loadButton->clicked().connect(SLOT(this, ProjectChooser::loadClicked));
    mProjectSelectionBox->doubleClicked().connect(SLOT(this, ProjectChooser::loadClicked));
    mDeleteButton->clicked().connect(SLOT(this, ProjectChooser::deleteClicked));
    mProjectSelectionBox->clicked().connect(SLOT(this, ProjectChooser::selectionChanged));

    // Message Box
    mMessageBox = new WMessageBox();
    mMessageBox->buttonClicked().connect(SLOT(this, ProjectChooser::handleMessageBoxFinished));


    resetAll();
}

///
//  Destructor
//
ProjectChooser::~ProjectChooser()
{
    delete mMessageBox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset to default widget state
//
void ProjectChooser::resetAll()
{
    WAbstractItemModel *model = mProjectSelectionBox->model();

    mProjectSelectionBox->clear();
    mProjectSelectionBox->addItem("Default (All MRIDs)");
    model->setData(model->index(0, 0), boost::any(std::string("")), UserRole);
    mProjectSelectionBox->setCurrentIndex(0);
    mDeleteButton->setEnabled(false);


    std::string userProjectDir = getConfigOptionsPtr()->GetProjectDir() + "/" +
                                 getCurrentUserName();

    try
    {
        for(directory_iterator dirIter(userProjectDir); dirIter != directory_iterator(); ++dirIter)
        {
            const string& extension = dirIter->path().extension().string();

            if (extension == ".xml")
            {
                mProjectSelectionBox->addItem(dirIter->path().string());
            }
        }
    }
    catch(...)
    {
        // Empty directory, which is fine
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Load button clicked [slot]
//
void ProjectChooser::loadClicked()
{
    if (mProjectSelectionBox->currentIndex() < 0)
            return;

    WAbstractItemModel *model = mProjectSelectionBox->model();
    boost::any userData = model->data(model->index(mProjectSelectionBox->currentIndex(), 0), DisplayRole);
    std::string result;
    if (!userData.empty())
    {
        result =  boost::any_cast<WString>(userData).toUTF8();
    }
    mProjectChosen.emit(result);
}


///
//  Delete button clicked [slot]
//
void ProjectChooser::deleteClicked()
{
    if (mProjectSelectionBox->currentIndex() < 0)
            return;

    mMessageBox->setWindowTitle("Confirm Delete");
    mMessageBox->setText("Are you sure you want to delete project '" + mProjectSelectionBox->currentText().toUTF8() + "'?" );
    mMessageBox->setButtons(Wt::Yes | Wt::No);
    mMessageBox->show();
}

///
//  Selection changed [slot]
//
void ProjectChooser::selectionChanged()
{
    if (mProjectSelectionBox->currentIndex() > 0)
    {
        mDeleteButton->setEnabled(true);
    }
    else
    {
        mDeleteButton->setEnabled(false);
    }

    mProjectClicked.emit(mProjectSelectionBox->currentText().toUTF8());
}

///
//  Handle message box finished [slot]
//
void ProjectChooser::handleMessageBoxFinished(StandardButton button)
{
    if (button == Wt::Yes)
    {
        try
        {
            boost::filesystem::remove(mProjectSelectionBox->currentText().toUTF8());
        }
        catch(...)
        {
            WApplication::instance()->log("error") << "Could not delete project file: " << mProjectSelectionBox->currentText().toUTF8();
        }
    }

    mMessageBox->hide();

    resetAll();
}



