//
//
//  Description:
//      Implementation of the MRID browser widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "MRIBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WRegExp>
#include <Wt/WSuggestionPopup>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
//
//  MRIFilterProxyModel
//
//


///
//  Constructor
//
MRIFilterProxyModel::MRIFilterProxyModel(WObject *parent) :
    WSortFilterProxyModel(parent)
{
}

///
//  Destructor
//
MRIFilterProxyModel::~MRIFilterProxyModel()
{
}

///
// Custom filter, override base class implementation
//
bool MRIFilterProxyModel::filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const
{
    if (filterRegExp().empty())
        return true;

    for(int col = 0; col < sourceModel()->columnCount(); col++)
    {
        boost::any data = sourceModel()->index(sourceRow, col, sourceParent).data(filterRole());

        if (!data.empty())
        {
            WString s = asString(data);
            std::string searchTarget = s.toUTF8();

            // Break search pattern into multiple tokens separated by spaces
            std::string searchPattern = filterRegExp().toUTF8();
            istringstream istr(searchPattern, ios_base::out);
            std::string curSearchPattern;
            while (getline(istr, curSearchPattern, ' '))
            {
                if (curSearchPattern != "" && curSearchPattern != " " && curSearchPattern != "\n")
                {
                    WRegExp regExp(curSearchPattern);
                    bool searchResult = regExp.exactMatch(s) || boost::algorithm::icontains(searchTarget, curSearchPattern);
                    if (searchResult)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
MRIBrowser::MRIBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    // Populate the list of MRIDs
    mMRITreeView = new WTreeView();
    mMRIModel = new WStandardItemModel(this);
    populateMRIDs(getConfigOptionsPtr()->GetDicomDir() + + "/dcm_MRID_age.log");

    mSearchButton = new WPushButton("Search");

    WContainerWidget *searchContainer = new WContainerWidget();
    mSearchLineEdit = new WLineEdit("", searchContainer);
    mSearchLineEdit->setToolTip("Enter a string or regular expression to filter MRIDs.  Multiple expressions can be separated by spaces.");
    mSearchLineEdit->setTextSize(27);

    mSortFilterProxyModel = new MRIFilterProxyModel(this);
    mSortFilterProxyModel->setSourceModel(mMRIModel);
    mSortFilterProxyModel->setDynamicSortFilter(true);
    mSortFilterProxyModel->setFilterKeyColumn(0);
    mSortFilterProxyModel->setFilterRole(DisplayRole);

    mMRITreeView->setModel(mSortFilterProxyModel);
    mMRITreeView->resize(250, WLength::Auto);
    mMRITreeView->setSelectionMode(SingleSelection);
    mMRITreeView->expandToDepth(1);
    mMRITreeView->selectionChanged().connect(SLOT(this, MRIBrowser::mriChanged));
    mMRITreeView->setHeaderHeight(0);

    // options for email address suggestions
    WSuggestionPopup::Options searchOptions
     = { "<span class=\"highlight\">", // highlightBeginTag
             "</span>",                    // highlightEndTag
             ' ',           // listSeparator      (for multiple addresses)
             " ",        // whitespace
             " ", // wordSeparators     (within an address)
             " "           // appendReplacedText (prepare next email address)
       };

    WSuggestionPopup *popup
       = new Wt::WSuggestionPopup(Wt::WSuggestionPopup::generateMatcherJS(searchOptions),
                                  Wt::WSuggestionPopup::generateReplacerJS(searchOptions),
                                  searchContainer);
    popup->setStyleClass("suggest");
    popup->forEdit(mSearchLineEdit);
    popup->setModel(mMRIModel);
    popup->setModelColumn(0);

    WGridLayout *searchLayout = new WGridLayout();
    searchLayout->addWidget(searchContainer, 0, 0, AlignRight);
    searchLayout->addWidget(mSearchButton, 0, 1, AlignLeft);
    searchLayout->setColumnStretch(0, 1);

    WGridLayout *layout = new WGridLayout();
    layout->addLayout(searchLayout, 0, 0);
    layout->addWidget(mMRITreeView, 1, 0);
    layout->setRowStretch(1, 1);
    setLayout(layout);

    mSearchButton->clicked().connect(SLOT(this, MRIBrowser::searchClicked));

    resetAll();
}

///
//  Destructor
//
MRIBrowser::~MRIBrowser()
{
}

///
//  Reset to default state
//
void MRIBrowser::resetAll()
{
    WModelIndexSet noSelection;
    mMRITreeView->setSelectedIndexes(noSelection);

    mSortFilterProxyModel->setFilterRegExp("");
    mSearchButton->setText("Filter");
    mSearchLineEdit->setText("");
    mSearchLineEdit->setEnabled(true);
    mFiltering = false;

}


///
/// Given a scan directory, determine the MRID
///
std::string MRIBrowser::getMRIDFromScanDir(const std::string& scanDir) const
{
    path scanPath(scanDir);

    for (int i = 0; i < mMRIModel->rowCount(); i++)
    {
        WStandardItem *item = mMRIModel->item(i);

        if (item != NULL)
        {
            boost::any displayData = item->data(DisplayRole);
            boost::any d = item->data(UserRole + 1);
            if (!displayData.empty() && !d.empty())
            {
                std::string curScanDir = boost::any_cast<std::string>(d);
                path curScanPath(curScanDir);

                if (scanPath.normalize() == curScanPath.normalize())
                {

                    WString mrid = boost::any_cast<WString>(displayData);
                    return mrid.toUTF8();
                }
            }
        }
    }

    return ("");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Populate the MRIDs model by reading the dcmMRID.log file.
//
void MRIBrowser::populateMRIDs(const std::string& mridLogFile)
{
    std::ifstream inFile(mridLogFile.c_str());
    while (!inFile.eof())
    {
        string scanName,
               MRID,
               age;

        char buf[1024] = {0};
        inFile.getline( buf, sizeof(buf));

        istringstream istr( string(buf), ios_base::out);
        istr >> scanName >> MRID >> age;

        // If age was an error, then just map it to UNKNOWN_AGE
        if (age == "ERROR:")
        {
            age = "UNKNOWN_AGE";
        }

        stringstream scanDir;
        scanDir << getConfigOptionsPtr()->GetDicomDir() << "/" << scanName;

        stringstream tocURL;

        tocURL << scanDir.str() << "/toc.txt";


        std::ifstream tocFile(tocURL.str().c_str());
        if (tocFile.is_open())
        {
            mMRIModel->appendRow(createMRIItem(MRID, scanDir.str(), age));
        }
    }
}


///
//  Create an MRI item.
//
WStandardItem *MRIBrowser::createMRIItem(const std::string& MRID,
                                         const std::string& scanDir,
                                         const std::string& age)
{
    WStandardItem *result = new WStandardItem(MRID);

    result->setData(scanDir, UserRole + 1);
    result->setData(age, UserRole + 2);
    result->setIcon("icons/folder.gif");

    return result;
}



///
//  MRI selection changed by user
//
void MRIBrowser::mriChanged()
{
    if (mMRITreeView->selectedIndexes().empty())
        return;

    WModelIndex selected = *mMRITreeView->selectedIndexes().begin();
    boost::any displayData = selected.data(DisplayRole);
    boost::any d = selected.data(UserRole + 1);
    boost::any d1 = selected.data(UserRole + 2);
    if (!displayData.empty() && !d.empty() && !d1.empty())
    {
        WString mrid = boost::any_cast<WString>(displayData);
        std::string scanDir = boost::any_cast<std::string>(d);
        std::string age = boost::any_cast<std::string>(d1);

        mMRISelected.emit(mrid.toUTF8(), scanDir, age);
    }
}

///
//  Search clicked [slot]
//
void MRIBrowser::searchClicked()
{
    if (!mFiltering)
    {
        if(!mSearchLineEdit->text().empty())
        {
            mFiltering = true;
            mSortFilterProxyModel->setFilterRegExp(mSearchLineEdit->text());
            mSearchButton->setText("Clear");
            mSearchLineEdit->setEnabled(false);
        }
    }
    else if (mFiltering)
    {
        mSortFilterProxyModel->setFilterRegExp("");
        mSearchButton->setText("Filter");
        mSearchLineEdit->setEnabled(true);
        mFiltering = false;
    }
}

