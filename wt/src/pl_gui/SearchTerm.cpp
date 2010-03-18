//
//
//  Description:
//      This is a widget which contains a search term (used by ProjectCreator)
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "SearchTerm.h"
#include "MRIBrowser.h"
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
#include <Wt/WGroupBox>
#include <Wt/WComboBox>
#include <signal.h>


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
SearchTerm::SearchTerm(int uniqueID, WContainerWidget *parent) :
    mUniqueID(uniqueID),
    WContainerWidget(parent)
{
    setStyleClass("maindiv");

    mSearchField = new WComboBox();

    for (int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
    {
        const MRIBrowser::MRIField *field = MRIBrowser::getMRIField(i);
        if (field != NULL && field->mSearchable)
        {
            mSearchField->addItem(field->mDescription);
        }
    }

    mSearchType = new WComboBox();
    for (int i = 0; i < MRIBrowser::NUM_TYPES; i++)
    {
        const MRIBrowser::MRISearchType *searchType = MRIBrowser::getMRISearchType(i);
        if (searchType != NULL)
        {
            mSearchType->addItem(searchType->mDescription);
        }
    }

    mSearchString = new WLineEdit("");
    mPlusButton = new WPushButton("+");
    mMinusButton = new WPushButton("-");

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mSearchField, 0, 0, AlignMiddle);
    layout->addWidget(mSearchType, 0, 1, AlignMiddle);
    layout->addWidget(mSearchString, 0, 2, AlignMiddle);
    layout->addWidget(mPlusButton, 0, 3, AlignMiddle | AlignCenter);
    layout->addWidget(mMinusButton, 0, 4, AlignMiddle | AlignCenter);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
    layout->setRowStretch(0, -1);

    mPlusButton->clicked().connect(SLOT(this, SearchTerm::plusClicked));
    mMinusButton->clicked().connect(SLOT(this, SearchTerm::minusClicked));

    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
SearchTerm::~SearchTerm()
{
}

///
// Reset to default state
//
void SearchTerm::resetAll()
{
    mSearchField->setCurrentIndex(0);
    mSearchType->setCurrentIndex(0);
    mSearchString->setText("");
}


///
// Copy values from another search term
//
void SearchTerm::copySearchTerm(const SearchTerm* src)
{
    mSearchField->setCurrentIndex(src->mSearchField->currentIndex());
    mSearchType->setCurrentIndex(src->mSearchType->currentIndex());
    mSearchString->setText(src->mSearchString->text());
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Get the search term as XML for storage
//
std::string SearchTerm::getAsXML()
{
    std::string result;

    if (mSearchString->text() == "")
        return result;

    result = "<SearchTerm field=\"";

    for (int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
    {
        const MRIBrowser::MRIField *field = MRIBrowser::getMRIField(i);
        if (field != NULL && field->mDescription == mSearchField->currentText().toUTF8())
        {
            result += field->mTagName + "\" ";
            break;
        }
    }

    for (int i = 0; i < MRIBrowser::NUM_TYPES; i++)
    {
        const MRIBrowser::MRISearchType *searchType = MRIBrowser::getMRISearchType(i);
        if (searchType != NULL && searchType->mDescription == mSearchType->currentText().toUTF8())
        {
            result += "type=\"" + searchType->mXMLValue + "\" ";
            break;
        }
    }

    result += "expr=\"" + mSearchString->text().toUTF8() + "\" />\n";

    return result;
}

///
//  Set the search term from the XML data
//
void SearchTerm::setFromXMLData(const std::string& fieldTagName, const std::string &type, const std::string &expr)
{
    for (int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
    {
        const MRIBrowser::MRIField *field = MRIBrowser::getMRIField(i);
        if (field != NULL && field->mTagName == fieldTagName)
        {
            int index = mSearchField->findText(field->mDescription);
            if (index >= 0)
            {
                mSearchField->setCurrentIndex(index);
                break;
            }
        }
    }

    for (int i = 0; i < MRIBrowser::NUM_TYPES; i++)
    {
        const MRIBrowser::MRISearchType *searchType = MRIBrowser::getMRISearchType(i);
        if (searchType != NULL && searchType->mXMLValue == mSearchType->currentText().toUTF8())
        {
            int index = mSearchType->findText(searchType->mDescription);
            if (index >= 0)
            {
                mSearchType->setCurrentIndex(index);
                break;
            }
        }
    }

    mSearchString->setText(expr);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Plus clicked [slot]
//
void SearchTerm::plusClicked()
{
    mSearchAddOrRemove.emit(true, mUniqueID);
}

///
//  Minus clicked [slot]
//
void SearchTerm::minusClicked()
{
    mSearchAddOrRemove.emit(false, mUniqueID);
}

