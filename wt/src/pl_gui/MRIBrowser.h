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

#include <Wt/WTreeView>
#include <Wt/WStandardItemModel>
#include <string>

using namespace Wt;

///
/// \class MRIBrowser
/// \brief Provides a browser for all of the MRIDs
///
class MRIBrowser : public WTreeView
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
    //  Create an MRI item.
    //
    WStandardItem *createMRIItem(const std::string& MRID,
                                 const std::string& scanDir);

    /// MRID Model
    WStandardItemModel *mMRIModel;
};

#endif // MRIBROWSER_H

