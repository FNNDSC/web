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
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

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
MRIBrowser::MRIBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    // Populate the list of MRIDs
    mMRITreeView = new WTreeView();
    mMRIModel = new WStandardItemModel(this);
    populateMRIDs(getConfigOptionsPtr()->GetDicomDir() + + "/dcm_MRID_age.log");

    mMRITreeView->setModel(mMRIModel);
    mMRITreeView->resize(200, WLength::Auto);
    mMRITreeView->setSelectionMode(SingleSelection);
    mMRITreeView->expandToDepth(1);
    mMRITreeView->selectionChanged().connect(SLOT(this, MRIBrowser::mriChanged));
    mMRITreeView->setHeaderHeight(0);

    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mMRITreeView);
    setLayout(layout);

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
            boost::any d = item->data(UserRole);
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

    result->setData(scanDir, UserRole);
    result->setData(age, UserRole + 1);
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
    boost::any d = selected.data(UserRole);
    boost::any d1 = selected.data(UserRole + 1);
    if (!displayData.empty() && !d.empty() && !d1.empty())
    {
        WString mrid = boost::any_cast<WString>(displayData);
        std::string scanDir = boost::any_cast<std::string>(d);
        std::string age = boost::any_cast<std::string>(d1);

        mMRISelected.emit(mrid.toUTF8(), scanDir, age);
    }
}


