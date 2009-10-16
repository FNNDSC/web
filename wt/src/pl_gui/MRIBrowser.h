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

#include <string>

using namespace Wt;

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
    /// Signal accessor for MRI selection
    ///
    Wt::Signal<std::string, std::string>& mriSelected() { return mMRISelected; }

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
    ///  Create an MRI item.
    ///
    WStandardItem *createMRIItem(const std::string& MRID,
                                 const std::string& scanDir);

private:

    /// Signal for when an MRI is selected
    Wt::Signal<std::string, std::string> mMRISelected;

    /// MRID Tree view
    WTreeView *mMRITreeView;

    /// MRID Model
    WStandardItemModel *mMRIModel;
};

#endif // MRIBROWSER_H

