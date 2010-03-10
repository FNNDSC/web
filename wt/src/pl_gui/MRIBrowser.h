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

#include <string>
#include <list>

namespace Wt
{
    class WPushButton;
    class WLineEdit;
}
using namespace Wt;

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
    /// Custom filter, override base class implementation
    ///
    virtual bool filterAcceptRow(int sourceRow, const WModelIndex& sourceParent) const;

    /// List of filters specified from file
    std::list<std::string> mFilterFileList;

};

///
/// \class MRIBrowser
/// \brief Provides a browser for all of the MRIDs
///
class MRIBrowser : public WContainerWidget
{
public:
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

private:

    ///
    /// Populate the MRIDs model by reading the dcmMRID.log file.
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

#endif // MRIBROWSER_H

