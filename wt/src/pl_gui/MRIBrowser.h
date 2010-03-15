//
//
//  Description:
//      Definition of MRID browser widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef MRIBROWSER_H
#define MRIBROWSER_H

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>
#include <Wt/WStandardItemModel>
#include <Wt/WSortFilterProxyModel>
#include <mxml.h>

#include <string>
#include <list>

class MRIFilterProxyModel;
namespace Wt
{
    class WPushButton;
    class WLineEdit;
}
using namespace Wt;



///
/// \class MRIBrowser
/// \brief Provides a browser for all of the MRIDs
///
class MRIBrowser : public WContainerWidget
{
public:
    // Types of data stored per MRID
    typedef enum
    {
        PATIENT_ID = 0,
        SCAN,
        DIRECTORY,
        AGE,
        NAME,
        SEX,
        BIRTHDAY,
        SCANDATE,
        MANUFACTURER,
        MODEL,
        SOFTWARE_VER,

        NUM_FIELDS
    } MRIFieldEnum;

    // Types of searches that can be performed
    typedef enum
    {
        CONTAINS,
        EQUAL,
        GEQUAL,
        LEQUAL,
        NUM_TYPES

    } MRISearchTypeEnum;

    // Whether to match all or any of search items
    typedef enum
    {
        ANY = 0,
        ALL
    } MRISearchMatch;

    /// User role indices
    typedef enum
    {
        DIR_ROLE = Wt::UserRole + 1,
        AGE_ROLE,
        NAME_ROLE,
        SEX_ROLE,
        BIRTHDAY_ROLE,
        SCANDATE_ROLE,
        MANUFACTURER_ROLE,
        MODEL_ROLE,
        SOFTWARE_VER_ROLE,
        NUM_SCANS_ROLE,
        FIRST_SCAN_ROLE

    } UserRoleEnum;

    /// MRI Field
    typedef struct
    {
        /// Role where data is stored in model
        int mRole;

        /// Is a searchable field
        bool mSearchable;

        /// XML tag name
        std::string mTagName;

        /// Public description (for search)
        std::string mDescription;
    } MRIField;

    typedef struct
    {
        /// Value to store in XML
        std::string mXMLValue;

        /// Descriotion
        std::string mDescription;

    } MRISearchType;

    ///
    /// Constructor
    ///
    MRIBrowser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MRIBrowser();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Given a scan directory, determine the MRID
    ///
    std::string getMRIDFromScanDir(const std::string& scanDir) const;

    ///
    /// Signal accessor for MRI selection
    ///
    Wt::Signal<std::string, std::string, std::string>& mriSelected() { return mMRISelected; }

    ///
    /// Return the number of fields in the MRI field table
    ///
    static const MRIField* getMRIField(int role);

    ///
    // Return the search term of the MRI search term table
    //
    static const MRISearchType* getMRISearchType(int index);

    ///
    /// Set filter file - a file that specifies a set of patterns to filter the list of MRIs
    ///
    void setFilterFile(const std::string& path);

private:



    /// Static field info table
    static const MRIField mFieldInfo[];

    /// Static search type table
    static const MRISearchType mSearchType[];

    ///
    /// Populate the MRIDs model by reading the dcm_MRID.xml file.
    ///
    void populateMRIDs(const std::string& mridLogFile);

    ///
    ///  MRI selection changed by user
    ///
    void mriChanged();

    ///
    /// Search clicked [slot]
    ///
    void searchClicked();

    ///
    ///  Create an MRI item.
    ///
    WStandardItem *createMRIItem(const std::string& MRID,
                                 const std::string& scanDir,
                                 const std::string& age);

    ///
    ///  Set data in the model from an XML node
    ///
    void setDataColumn(mxml_node_t *node, const char* name, int row, int col, int role, std::string errorReplaceStr ="", std::string prependStr = "");

private:

    /// Signal for when an MRI is selected
    Wt::Signal<std::string, std::string, std::string> mMRISelected;

    /// MRID Tree view
    WTreeView *mMRITreeView;

    /// MRID Model
    WStandardItemModel *mMRIModel;

    /// Sort-filter proxy model
    MRIFilterProxyModel *mSortFilterProxyModel;

    /// Search button
    WPushButton *mSearchButton;

    /// Filter
    bool mFiltering;

    /// Search Line edit
    WLineEdit *mSearchLineEdit;
};

///
/// \class MRIFilterProxyModel
/// \brief Override WSortFilterProxyModel class to provide non-regexp searched
///
class MRIFilterProxyModel : public WSortFilterProxyModel
{
public:
    ///
    /// Constructor
    ///
    MRIFilterProxyModel(WObject *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MRIFilterProxyModel();

    ///
    /// Set filter file - a file that specifies a set of patterns to filter the list of MRIs
    ///
    void setFilterFile(const std::string& path);

protected:

    ///
    /// Compare search term
    ///
    bool compareSearchTerm(std::string searchTerm, std::string expr, MRIBrowser::MRISearchTypeEnum searchType) const;

    /// Search item
    typedef struct
    {
        /// Role
        int mRole;

        /// Search Type
        MRIBrowser::MRISearchTypeEnum mSearchType;

        /// Search expression
        std::string mExpr;

    } SearchItem;

    ///
    /// Custom filter, override base class implementation
    ///
    virtual bool filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const;

    /// List of search items
    std::list<SearchItem> mSearchItems[MRIBrowser::NUM_FIELDS];

    /// Number of search terms
    int mNumSearchTerms;

    /// Type to match (any or all)
    MRIBrowser::MRISearchMatch mSearchMatchType;

};

#endif // MRIBROWSER_H

