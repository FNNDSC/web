//
//
//  Description:
//      This is a widget that is used for creating a project to filter the list of
//      MRIDs
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "ProjectCreator.h"
#include "ConfigOptions.h"
#include "SearchTerm.h"
#include "ProjectXML.h"
#include "MRIBrowser.h"
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
#include <Wt/WGroupBox>
#include <Wt/WRegExpValidator>
#include <Wt/WComboBox>
#include <Wt/WMessageBox>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>

///
//  Constants
//
const int MAX_SEARCH_TERMS=10;

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
ProjectCreator::ProjectCreator(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    setStyleClass("maindiv");
	
    WGroupBox *mainBox = new WGroupBox("Create New Project");
    mainBox->setStyleClass("groupdiv");

    mSearchTermLayout = new WVBoxLayout();
    for(int i = 0; i < MAX_SEARCH_TERMS; i++)
    {
        SearchTerm *newTerm = new SearchTerm(i);
        mSearchTerms.push_back(newTerm);
        if (i == 0)
        {
            mSearchTerms[i]->show();
        }
        else
        {
            mSearchTerms[i]->hide();
        }
        mSearchTermLayout->addWidget(mSearchTerms[i]);

        // Connections
        mSearchTerms[i]->searchAddOrRemove().connect(SLOT(this, ProjectCreator::searchAddOrRemove));
    }

    WPushButton *createButton = new WPushButton("Create...");
    mSaveButton = new WPushButton("Save");
    createButton->clicked().connect(SLOT(this, ProjectCreator::createClicked));
    mSaveButton->clicked().connect(SLOT(this, ProjectCreator::saveClicked));

    WGridLayout *buttonLayout = new WGridLayout();
    buttonLayout->addWidget(createButton, 0, 0, AlignRight | AlignMiddle);
    buttonLayout->addWidget(mSaveButton, 0, 1, AlignLeft | AlignMiddle);

    mAnyOrAllComboBox = new WComboBox();
    mAnyOrAllComboBox->addItem("any");
    mAnyOrAllComboBox->addItem("all");

    WGridLayout *anyOrAllLayout = new WGridLayout();
    WText *includeText = new WText("Include if");
    includeText->setWordWrap(false);
    WText *conditionText = new WText("of the following conditions are matched:");
    conditionText->setWordWrap(false);

    anyOrAllLayout->addWidget(includeText, 0, 0, AlignLeft);
    anyOrAllLayout->addWidget(mAnyOrAllComboBox, 0, 1, AlignLeft | AlignMiddle);
    anyOrAllLayout->addWidget(conditionText, 0, 2, AlignLeft);
    anyOrAllLayout->setColumnStretch(2, 1);


    WGridLayout *boxLayout = new WGridLayout();
    boxLayout->addLayout(anyOrAllLayout, 0, 0);
    boxLayout->addLayout(mSearchTermLayout, 1, 0);
    boxLayout->addLayout(buttonLayout, 2, 0);
    boxLayout->setRowStretch(0, -1);
    boxLayout->setRowStretch(1, 1);
    boxLayout->setRowStretch(2, -1);
    mainBox->setLayout(boxLayout);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mainBox, 0, 0);
    setLayout(layout);
    layout->setRowStretch(0, -1);


    // Create project dialog
    mCreateProjectDialog = new WDialog("Create Project");
    mCreateProjectDialog->setModal(true);

    // Project name validator
    WRegExpValidator *projectNameValidator = new WRegExpValidator("[a-zA-Z0-9_+-]+");
    projectNameValidator->setMandatory(true);

    WText *nameText = new WText("Enter project name: ", mCreateProjectDialog->contents());
    mProjectNameEdit = new WLineEdit("", mCreateProjectDialog->contents());
    mProjectNameEdit->setValidator(projectNameValidator);
    mProjectNameEdit->setMinimumSize(WLength(250, WLength::Pixel), WLength::Auto);
    WBreak *brk = new WBreak(mCreateProjectDialog->contents());

    WPushButton *okButton = new WPushButton("Ok", mCreateProjectDialog->contents());
    WPushButton *cancelButton = new WPushButton("Cancel", mCreateProjectDialog->contents());
    mCreateErrorLabel = new WLabel("", mCreateProjectDialog->contents());

    okButton->clicked().connect(SLOT(this, ProjectCreator::okCreateClicked));
    cancelButton->clicked().connect(SLOT(this, ProjectCreator::cancelCreateClicked));

    // Save dialog box
    mSaveMessageBox = new WMessageBox();
    mSaveMessageBox->buttonClicked().connect(SLOT(this, ProjectCreator::handleSaveMessageBoxClosed));
}

///
//  Destructor
//
ProjectCreator::~ProjectCreator()
{
    delete mCreateProjectDialog;
    delete mSaveMessageBox;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Result to default state
//
void ProjectCreator::resetAll()
{
    for(int i = 0; i < MAX_SEARCH_TERMS; i++)
    {
        mSearchTerms[i]->resetAll();
        if (i == 0)
        {
            mSearchTerms[i]->show();
        }
        else
        {
            mSearchTerms[i]->hide();
        }
    }

    mSaveButton->disable();
    mCurrentProjectPath = "";
}

///
//  Set from XML
//
void ProjectCreator::setFromXML(std::string projectPathXML)
{
    ProjectXML projectXML;

    resetAll();

    if (!projectXML.loadFromFile(projectPathXML))
    {
        return;
    }

    if (projectXML.getSearchMatchType() == "any")
    {
        mAnyOrAllComboBox->setCurrentIndex(0);
    }
    else
    {
        mAnyOrAllComboBox->setCurrentIndex(1);

    }

    std::list<ProjectXML::SearchTermNode>::const_iterator iter = projectXML.getSearchTerms().begin();
    int index = 0;
    while (iter != projectXML.getSearchTerms().end())
    {
        const ProjectXML::SearchTermNode *searchNode = &(*iter);

        mSearchTerms[index]->show();
        mSearchTerms[index]->setFromXMLData(searchNode->mField,
                                            searchNode->mType,
                                            searchNode->mExpr);


        index++;
        iter++;
    }

    mSaveButton->enable();
    mCurrentProjectPath = projectPathXML;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
// Create clicked [slot]
//
void ProjectCreator::createClicked()
{
    mCreateErrorLabel->setText("");
    mCreateProjectDialog->show();
}

///
// Save clicked [slot]
//
void ProjectCreator::saveClicked()
{
    mSaveMessageBox->setWindowTitle("Confirm Save");
    mSaveMessageBox->setText("Are you sure you want to overwrite project '" + mCurrentProjectPath + "'?" );
    mSaveMessageBox->setButtons(Wt::Yes | Wt::No);
    mSaveMessageBox->show();
}

///
// OK Create (dialog) clicked [slot]
//
void ProjectCreator::okCreateClicked()
{
    if (!mProjectNameEdit->validate() || mProjectNameEdit->text().empty())
    {
        mCreateErrorLabel->setText("Invalid project name.");
        return;
    }

    // Check to see if file already exists
    std::string userProjDir = getConfigOptionsPtr()->GetProjectDir() + "/" +
                              getCurrentUserName();
    std::string projectFileName = mProjectNameEdit->text().toUTF8();
    std::string projectFilePath = userProjDir + "/" + projectFileName + ".xml";

    if (exists(projectFilePath))
    {
        mCreateErrorLabel->setText("Project already exists.  Delete it or choose another name.");
        return;
    }

    if (!exists(userProjDir))
    {
        if (!create_directory(userProjDir))
        {
            mCreateErrorLabel->setText("Error creating user directory.  Contact webmaster.");
            WApplication::instance()->log("error") << "Could not create directory: " << userProjDir;
            return;
        }
    }


    // Open  file for writing
    if (!saveProjectToXML(projectFilePath))
    {
        mCreateErrorLabel->setText("Can't create file.");
        return;
    }
    mCreateProjectDialog->hide();
    mProjectAdded.emit(mProjectNameEdit->text().toUTF8());
}

///
// Cancel Create (dialog) clicked [slot]
//
void ProjectCreator::cancelCreateClicked()
{
    mCreateProjectDialog->hide();
}

///
//  Search term added or removed [slot]
//
void ProjectCreator::searchAddOrRemove(bool add, int ID)
{
    int numShowing = 0;
    for (int i = 0; i < MAX_SEARCH_TERMS; i++)
    {
        if(mSearchTerms[i]->isVisible())
            numShowing++;
    }

    if (add)
    {
        if (numShowing < MAX_SEARCH_TERMS)
        {
            mSearchTerms[numShowing]->resetAll();
            mSearchTerms[numShowing]->show();
        }
    }
    else
    {
        if (numShowing > 1)
        {
            if (ID == (numShowing - 1))
            {
                mSearchTerms[numShowing - 1]->hide();
            }
            else
            {
                mSearchTerms[numShowing - 1]->hide();

                // Need to shift up
                for(int i = ID; i < numShowing-1; i++)
                {
                    mSearchTerms[i]->copySearchTerm(mSearchTerms[i+1]);
                }
            }
        }
    }
}

///
//  Save project to XML
//
bool ProjectCreator::saveProjectToXML(std::string projectFilePath)
{
    // Open  file for writing
    ofstream ofs(projectFilePath.c_str(), ios::out);
    if (!ofs.is_open())
    {
        return false;
    }

    ofs << "<?xml version=\"1.0\"?>" << endl;
    ofs << "<SearchMatch type=\"" << mAnyOrAllComboBox->currentText().toUTF8() << "\" />" << endl;

    for (int i = 0; i < MAX_SEARCH_TERMS; i++)
    {
        if(mSearchTerms[i]->isVisible())
        {
            ofs << mSearchTerms[i]->getAsXML();
        }
    }

    ofs.close();
    return true;
}

///
//  Handle save message box finished
//
void ProjectCreator::handleSaveMessageBoxClosed(StandardButton button)
{
    if (button == Wt::Yes)
    {
        saveProjectToXML(mCurrentProjectPath);
    }

    mSaveMessageBox->hide();
}
