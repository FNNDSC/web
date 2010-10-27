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
#include "ProjectXML.h"
#include "PermissionsXML.h"
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

///
//  Static constants
//

// This table maps model roles to whether they can be searched, what XML tag they are read from (and search on),
// and what description should be displayed.  This table is used heavily by the SearchTerm class.
const MRIBrowser::MRIField MRIBrowser::mFieldInfo[] =
{
    { DisplayRole,                          true,   "PatientID",            "Patient ID"            },  // PATIENT_ID
    { MRIBrowser::FIRST_SCAN_ROLE_INDEX,    true,   "Scan",                 "Scan Sequence Name"    },  // SCAN
    { MRIBrowser::DIR_ROLE,                 false,  "Directory",            "Directory"             },  // DIRECTORY
    { MRIBrowser::AGE_ROLE,                 true,   "PatientAge",           "Patient Age"           },  // AGE
    { MRIBrowser::NAME_ROLE,                true,   "PatientName",          "Patient Name"          },  // NAME
    { MRIBrowser::SEX_ROLE,                 true,   "PatientSex",           "Patient Sex"           },  // SEX
    { MRIBrowser::BIRTHDAY_ROLE,            true,   "PatientBirthday",      "Patient Birthday"      },  // BIRTHDAY
    { MRIBrowser::SCANDATE_ROLE,            true,   "ImageScanDate",        "Image Scan Date"       },  // SCANDATE
    { MRIBrowser::MANUFACTURER_ROLE,        true,   "ScannerManufacturer",  "Scanner Manufacturer"  },  // MANUFACTURER
    { MRIBrowser::MODEL_ROLE,               true,   "ScannerModel",         "Scanner Model"         },  // MODEL
    { MRIBrowser::SOFTWARE_VER_ROLE,        true,   "SoftwareVer",          "Software Version"      },  // SOFTWARE_VER
};

const MRIBrowser::MRISearchType MRIBrowser::mSearchType[] =
{
    { "contains",       "Contains"                      },  // CONTAINS
    { "equal",          "Is equal to"                   },  // EQUAL
    { "gtequal",        "Greater than or equal to"      },  // GEQUAL
    { "ltequal",        "Less than or equal to"         },  // LEQUAL
};

///////////////////////////////////////////////////////////////////////////////
//
//  MRIFilterProxyModel
//
//


///
//  Constructor
//
MRIFilterProxyModel::MRIFilterProxyModel(PermissionsXML *permissionsXML, WObject *parent) :
    WSortFilterProxyModel(parent),
    mNumSearchTerms(0),
    mPermissionsXML(permissionsXML)
{
}

///
//  Destructor
//
MRIFilterProxyModel::~MRIFilterProxyModel()
{
}

///
//  Set filter file - a file that specifies a set of patterns to filter the list of MRIs
//
void MRIFilterProxyModel::setFilterFile(const std::string& path)
{
    ProjectXML projectXML;

    for(int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
    {
        mSearchItems[i].clear();
    }
    mNumSearchTerms = 0;

    // No file specified, just return
    if (path == "")
        return;

    if (!projectXML.loadFromFile(path))
    {
        WApplication::instance()->log("error") << "Cound not open/parse project XML file: " << path;
        return;
    }

    mSearchMatchType = MRIBrowser::ANY;
    if (projectXML.getSearchMatchType() == "all")
    {
        mSearchMatchType = MRIBrowser::ALL;
    }

    std::list<ProjectXML::SearchTermNode> searchTerms = projectXML.getSearchTerms();
    std::list<ProjectXML::SearchTermNode>::iterator iter = searchTerms.begin();

    while(iter != searchTerms.end())
    {
        ProjectXML::SearchTermNode* node = &(*iter);

        for(int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
        {
            const MRIBrowser::MRIField *field = MRIBrowser::getMRIField(i);
            if (field != NULL && field->mTagName == node->mField)
            {
                for (int j = 0; j < MRIBrowser::NUM_TYPES; j++)
                {
                    const MRIBrowser::MRISearchType *search = MRIBrowser::getMRISearchType(j);
                    if (search != NULL & search->mXMLValue == node->mType)
                    {
                        SearchItem newItem;
                        newItem.mRole = field->mRole;
                        newItem.mSearchType = (MRIBrowser::MRISearchTypeEnum)j;
                        newItem.mExpr = node->mExpr;
                        mSearchItems[i].push_back(newItem);
                        mNumSearchTerms++;
                    }
                }
            }
        }

        iter++;
    }
}

///
// Custom filter, override base class implementation
//
bool MRIFilterProxyModel::filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const
{
    int col = 0;
    boost::any data = sourceModel()->index(sourceRow, col, sourceParent).data(filterRole());

    if (!data.empty())
    {
        WString s = asString(data);
        std::string searchTarget = s.toUTF8();

        // First, see if it matches on the user/group access
        if (filterByUserGroup(sourceRow, sourceParent) == false)
            return false;

        // Next, see if it matches anything in the project filter search criteria
        if (filterByProjectFile(sourceRow, sourceParent) == false)
            return false;

        // If no regexp specified, we are done, row should be accepted.
        if (filterRegExp().empty())
            return true;

        //
        // Finally, if the user provided a filter in the UI, match on final
        // user specified filter
        //
        // Break search pattern into multiple tokens separated by spaces
        //
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

    return false;
}

///
// Check to see whether the row matches the project file search criteria
//
bool MRIFilterProxyModel::filterByProjectFile(int sourceRow, const WModelIndex& sourceParent) const
{
    int col = 0;

    bool passesFilterFile;
    if (mSearchMatchType == MRIBrowser::ANY)
    {
        passesFilterFile = false;
    }
    else
    {
        passesFilterFile = true;
    }

    if (mNumSearchTerms == 0)
    {
        passesFilterFile = true;
    }
    else
    {
        for(int i = 0; i < MRIBrowser::NUM_FIELDS; i++)
        {
            std::list<SearchItem>::const_iterator iter = mSearchItems[i].begin();
            while(iter != mSearchItems[i].end())
            {
                bool curSearchResult = true;
                const SearchItem *searchItem = &(*iter);

                // Special handling for scan which has multiple entries
                if (i == MRIBrowser::SCAN)
                {
                    // Determine the number of scans
                    boost::any numScansData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::NUM_SCANS_ROLE);
                    boost::any firstScansIndexData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::FIRST_SCAN_ROLE_INDEX);
                    if (!numScansData.empty() && !firstScansIndexData.empty())
                    {
                        int startScansIndex = boost::any_cast<int>(firstScansIndexData);
                        int numScans = boost::any_cast<int>(numScansData);
                        if (numScans == 0)
                        {
                            curSearchResult = false;
                        }
                        else
                        {
                            for (int scan = 0; scan < numScans; scan++)
                            {
                                int scanRole = MRIBrowser::VARIABLE_DATA_START_ROLE + startScansIndex + scan;

                                boost::any scanData = sourceModel()->index(sourceRow, col, sourceParent).data(scanRole);
                                if (!scanData.empty())
                                {
                                    WString curStr = asString(scanData);
                                    std::string searchStr = curStr.toUTF8();

                                    curSearchResult = compareSearchTerm(searchStr, searchItem->mExpr, searchItem->mSearchType);

                                    if (curSearchResult)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                // General case
                else
                {
                    boost::any curData = sourceModel()->index(sourceRow, col, sourceParent).data(searchItem->mRole);
                    if (!curData.empty())
                    {
                        WString curStr = asString(curData);
                        std::string searchStr = curStr.toUTF8();

                        curSearchResult = compareSearchTerm(searchStr, searchItem->mExpr, searchItem->mSearchType);
                    }
                }

                // ANY
                if (mSearchMatchType == MRIBrowser::ANY)
                {
                    if (curSearchResult)
                    {
                        passesFilterFile = true;
                        break;
                    }
                }
                // ALL
                else
                {
                    passesFilterFile = passesFilterFile && curSearchResult;
                    if (passesFilterFile == false)
                        break;
                }
                iter++;
            }

            // ANY
            if (mSearchMatchType == MRIBrowser::ANY)
            {
                if (passesFilterFile == true)
                    break;
            }
            // ALL
            else
            {
                if (passesFilterFile == false)
                    break;
            }
        }

    }

    return passesFilterFile;
}

///
// Check to see whether the row is accessible to current user/group
//
bool MRIFilterProxyModel::filterByUserGroup(int sourceRow, const WModelIndex& sourceParent) const
{
    int col = 0;

    // Determine whether there is a match on the user
    boost::any numUsersData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::NUM_USERS_ROLE);
    boost::any firstUserIndexData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::FIRST_USER_ROLE_INDEX);
    if (!numUsersData.empty() && !firstUserIndexData.empty())
    {
        int startUserIndex = boost::any_cast<int>(firstUserIndexData);
        int numUsers = boost::any_cast<int>(numUsersData);
        if (numUsers > 0)
        {
            for (int user = 0; user < numUsers; user++)
            {
                int userRole = MRIBrowser::VARIABLE_DATA_START_ROLE + startUserIndex + user;

                boost::any userData = sourceModel()->index(sourceRow, col, sourceParent).data(userRole);
                if (!userData.empty())
                {
                    WString curStr = asString(userData);
                    std::string userName = curStr.toUTF8();

                    if (userName == getCurrentUserName())
                    {
                        // Found a match on exact user name, row should be accepted
                        return true;
                    }
                }
            }
        }
    }

    // Check whether the user is in the admin group
    if (mPermissionsXML->userInGroup(getCurrentUserName(), getConfigOptionsPtr()->GetAdminGroup()))
    {
        return true;
    }

    // Determine whether there is a match on the current user's groups
    boost::any numGroupsData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::NUM_GROUPS_ROLE);
    boost::any firstGroupIndexData = sourceModel()->index(sourceRow, col, sourceParent).data(MRIBrowser::FIRST_GROUP_ROLE_INDEX);
    if (!numGroupsData.empty() && !firstGroupIndexData.empty())
    {
        int startGroupIndex = boost::any_cast<int>(firstGroupIndexData);
        int numGroups = boost::any_cast<int>(numGroupsData);
        if (numGroups > 0)
        {
            for (int group = 0; group < numGroups; group++)
            {
                int groupRole =  MRIBrowser::VARIABLE_DATA_START_ROLE + startGroupIndex + group;

                boost::any groupData = sourceModel()->index(sourceRow, col, sourceParent).data(groupRole);
                if (!groupData.empty())
                {
                    WString curStr = asString(groupData);
                    std::string groupName = curStr.toUTF8();

                    // Check whether this user is in this group as stored in the permissions XML file
                    if (mPermissionsXML->userInGroup(getCurrentUserName(), groupName))
                    {
                        return true;
                    }
                }
            }
        }
    }

    // Did not match user or group, reject row
    return false;
}

///
//   Compare search term
//
bool MRIFilterProxyModel::compareSearchTerm(std::string searchStr, std::string expr, MRIBrowser::MRISearchTypeEnum searchType) const
{
    if (searchStr == "")
        return false;

    switch(searchType)
    {
    case MRIBrowser::CONTAINS:
        {
            WRegExp regExp(expr);
            bool searchResult = regExp.exactMatch(searchStr) || boost::algorithm::icontains(searchStr, expr);
            if (searchResult)
            {
                return true;
            }
        }
        break;
    case MRIBrowser::EQUAL:
        if (searchStr == expr)
        {
            return true;
        }
        break;
    case MRIBrowser::GEQUAL:
        if (searchStr >= expr)
        {
            return true;
        }
        break;
    case MRIBrowser::LEQUAL:
        if (searchStr <= expr)
        {
            return true;
        }
        break;
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
    populateMRIDs(getConfigOptionsPtr()->GetDicomDir() + + "/dcm_MRID.xml");

    // Create permissions XML file reader and read in the permissions file
    mPermissionsXML = new PermissionsXML();
    mPermissionsXML->loadFromFile(getConfigOptionsPtr()->GetPermissionsFile());

    mSearchButton = new WPushButton("Search");

    WContainerWidget *searchContainer = new WContainerWidget();
    mSearchLineEdit = new WLineEdit("", searchContainer);
    mSearchLineEdit->setToolTip("Enter a string or regular expression to filter MRIDs.  Multiple expressions can be separated by spaces.");
    mSearchLineEdit->resize(190, WLength::Auto);

    mSortFilterProxyModel = new MRIFilterProxyModel(mPermissionsXML, this);
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
             "</span>",                // highlightEndTag
             ' ',                      // listSeparator      (for multiple addresses)
             " ",                      // whitespace
             " ",                      // wordSeparators     (within an address)
             " "                       // appendReplacedText (prepare next email address)
       };

    mPopup = new Wt::WSuggestionPopup(Wt::WSuggestionPopup::generateMatcherJS(searchOptions),
                                      Wt::WSuggestionPopup::generateReplacerJS(searchOptions),
                                      searchContainer);
    //mPopup->setStyleClass("suggest");
    mPopup->forEdit(mSearchLineEdit);
    mPopup->setModel(mSortFilterProxyModel);
    mPopup->setModelColumn(0);
    mPopup->setPopup(true);

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

    // File system watcher
    mQtFileSystemWatcherThread = new QtFileSystemWatcherThread(this, WApplication::instance());

    resetAll();
}

///
//  Destructor
//
MRIBrowser::~MRIBrowser()
{
    delete mPermissionsXML;
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

    //mPermissionsXML.loadFromFile(/* TODO */);

    // Select the first item in the list
    if (mSortFilterProxyModel->rowCount() > 0)
    {
        mMRITreeView->select(mSortFilterProxyModel->index(0, 0));
        mSortFilterProxyModel->sort(0, AscendingOrder);
    }
}

///
//  Create Qt objects
//
void MRIBrowser::createQt()
{
    mQtFileSystemWatcherThread->createQt();
    mQtFileSystemWatcherThread->addPath(getConfigOptionsPtr()->GetDicomDir() + + "/dcm_MRID.xml");
}

///
//  Destroy Qt objects
//
void MRIBrowser::destroyQt()
{
    mQtFileSystemWatcherThread->destroyQt();
}


///
//  Finalize the widget (pre-destruction)
//
void MRIBrowser::finalize()
{
    mQtFileSystemWatcherThread->finalize();
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
            boost::any d = item->data(DIR_ROLE);
            if (!displayData.empty() && !d.empty())
            {
                WString curScanDir = boost::any_cast<WString>(d);
                path curScanPath(curScanDir.toUTF8());

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

///
// Return the number of fields in the MRI field table
//
const MRIBrowser::MRIField* MRIBrowser::getMRIField(int role)
{
    return &mFieldInfo[role];
}

///
// Return the search term of the MRI search term table
//
const MRIBrowser::MRISearchType* MRIBrowser::getMRISearchType(int index)
{
    return &mSearchType[index];
}

///
//  Set filter file - a file that specifies a set of patterns to filter the list of MRIs
//
void MRIBrowser::setFilterFile(const std::string& path)
{
    mFilterFilePath = path;
    mSortFilterProxyModel->setFilterFile(path);
    mSortFilterProxyModel->setFilterRegExp("");
    mSearchButton->setText("Filter");
    mSearchLineEdit->setEnabled(true);

    if (mSortFilterProxyModel->rowCount() > 0)
    {
        mMRITreeView->select(mSortFilterProxyModel->index(0, 0));
    }
}

///
//  Method which gets called with a path name whenever there is a file updated, must be
//  implemented by the QtFileSystemWatcherListener
//
void MRIBrowser::fileUpdated(std::string path)
{
    mMRIListUpdated.emit();
}

///
//  Refresh MRI list from disk
//
void MRIBrowser::refreshMRIList()
{
    if (mSortFilterProxyModel != NULL)
    {
        delete mSortFilterProxyModel;
    }

    mMRIModel->removeRows(0, mMRIModel->rowCount());

    populateMRIDs(getConfigOptionsPtr()->GetDicomDir() + + "/dcm_MRID.xml");


    mSortFilterProxyModel = new MRIFilterProxyModel(mPermissionsXML, this);
    mSortFilterProxyModel->setSourceModel(mMRIModel);
    mSortFilterProxyModel->setDynamicSortFilter(true);
    mSortFilterProxyModel->setFilterKeyColumn(0);
    mSortFilterProxyModel->setFilterRole(DisplayRole);


    mMRITreeView->setModel(mSortFilterProxyModel);
    mMRITreeView->setColumnWidth(0, WLength(125, WLength::Pixel));

    mPopup->setModel(mSortFilterProxyModel);
    resetAll();

    setFilterFile(mFilterFilePath);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//


///
//  Populate the MRIDs model by reading the dcm_MRID.xml file.
//
void MRIBrowser::populateMRIDs(const std::string& mridXMLFile)
{
    FILE *fp = fopen(mridXMLFile.c_str(), "r");
    if (fp == NULL)
    {
       WApplication::instance()->log("error") << "Failed opening MRID log file for reading: " << mridXMLFile;
       return;
    }

    mxml_node_t *tree;
    tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);

    bool result = true;
    mxml_node_t *patientRecordNode;


    for (patientRecordNode = mxmlFindElement(tree, tree,
                                             "PatientRecord",
                                             NULL, NULL,
                                             MXML_DESCEND);
        patientRecordNode != NULL;
        patientRecordNode = mxmlFindElement(patientRecordNode, tree,
                                            "PatientRecord",
                                            NULL, NULL,
                                            MXML_NO_DESCEND))
    {
        int row = mMRIModel->rowCount();
        mMRIModel->insertRows(row, 1);

        // Set extended (not for display) data
        setDataColumn(patientRecordNode, "PatientID", row, 0, DisplayRole);
        setDataColumn(patientRecordNode, "Directory", row, 0, DIR_ROLE, "", getConfigOptionsPtr()->GetDicomDir() + std::string("/"));
        setDataColumn(patientRecordNode, "PatientAge", row, 0, AGE_ROLE, "UNKNOWN_AGE");
        setDataColumn(patientRecordNode, "PatientName", row, 0, NAME_ROLE);
        setDataColumn(patientRecordNode, "PatientSex", row, 0, SEX_ROLE);
        setDataColumn(patientRecordNode, "PatientBirthday", row, 0, BIRTHDAY_ROLE);
        setDataColumn(patientRecordNode, "ImageScanDate", row, 0, SCANDATE_ROLE);
        setDataColumn(patientRecordNode, "ScannerManufacturer", row, 0, MANUFACTURER_ROLE);
        setDataColumn(patientRecordNode, "ScannerModel", row, 0, MODEL_ROLE);
        setDataColumn(patientRecordNode, "SoftwareVer", row, 0, SOFTWARE_VER_ROLE);

        int numVariableRoles = 0;
        // Set multi-entry data
        setMultiDataColumn(patientRecordNode, "Scan", row, 0, FIRST_SCAN_ROLE_INDEX, NUM_SCANS_ROLE, &numVariableRoles);
        setMultiDataColumn(patientRecordNode, "User", row, 0, FIRST_USER_ROLE_INDEX, NUM_USERS_ROLE, &numVariableRoles);
        setMultiDataColumn(patientRecordNode, "Group", row, 0, FIRST_GROUP_ROLE_INDEX, NUM_GROUPS_ROLE, &numVariableRoles);

        mMRIModel->item(row)->setIcon("icons/folder.gif");
    }


    fclose(fp);
    mxmlRelease(tree);
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
    boost::any d = selected.data(DIR_ROLE);
    boost::any d1 = selected.data(AGE_ROLE);

    if (!displayData.empty() && !d.empty() )
    {
        WString mrid = boost::any_cast<WString>(displayData);
        WString scanDir = boost::any_cast<WString>(d);
        WString age;
        if (!d1.empty())
        {
            age = boost::any_cast<WString>(d1);
        }
        else
        {
            age = "UNKNOWN_AGE";
        }

        mMRISelected.emit(mrid.toUTF8(), scanDir.toUTF8(), age.toUTF8());
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


///
//  Set data in the model from an XML node
//
void MRIBrowser::setDataColumn(mxml_node_t *node, const char* name, int row, int col, int role, std::string errorReplaceStr, std::string prependStr)
{

    boost::any data;
    mxml_node_t *dataNode = mxmlFindElement(node, node, name, NULL, NULL, MXML_DESCEND);
    if (dataNode != NULL && dataNode->child != NULL)
    {
        std::string dataStr = std::string(dataNode->child->value.opaque);

        if (dataStr != "ERROR:")
        {
            data = boost::any(WString::fromUTF8(prependStr + dataStr));
            mMRIModel->setData(row, col, data, role);
        }
        else
        {
            if (errorReplaceStr != "")
            {
                data = boost::any(WString::fromUTF8(errorReplaceStr));
                mMRIModel->setData(row, col, data, role);
            }
        }
    }
}

///
//  Set multiple data entries in the model from a series of like-named XML nodes
//
void MRIBrowser::setMultiDataColumn(mxml_node_t *node, const char* name, int row, int col, int firstRoleIndex, int numberRole,
                                    int *numVariableRoles)
{
    mxml_node_t *curNode;
    int count = 0;

    // Set the first index of the data
    boost::any startIndex = boost::any(*numVariableRoles);
    mMRIModel->setData(row, 0, startIndex, firstRoleIndex);

    for (curNode = mxmlFindElement(node, node,
                                   name,
                                   NULL, NULL,
                                   MXML_DESCEND);
        curNode != NULL;
        curNode = mxmlFindElement(curNode, node,
                                  name,
                                  NULL, NULL,
                                  MXML_NO_DESCEND))
    {
        if (curNode != NULL && curNode->child != NULL)
        {
            std::string dataStr = std::string(curNode->child->value.opaque);

            if (dataStr == "")
            {
                dataStr = "unnamed";
            }

            boost::any data = boost::any(WString::fromUTF8(dataStr));
            mMRIModel->setData(row, 0, data, VARIABLE_DATA_START_ROLE + *numVariableRoles);
            (*numVariableRoles)++;
            count++;
        }
    }

   // Set the number of items in the data
   boost::any numData = boost::any(count);
   mMRIModel->setData(row, 0, numData, numberRole);
}


