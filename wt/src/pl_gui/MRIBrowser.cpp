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
#include "MRIBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <fstream>
#include <iostream>
#include <string>

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
MRIBrowser::MRIBrowser(WContainerWidget *parent) :
    WTreeView(parent)
{
    // Populate the list of MRIDs
    mMRIModel = new WStandardItemModel();
    populateMRIDs(ConfigOptions::GetPtr()->GetDicomDir() + + "/dcm_MRID.log");

    setAttributeValue
            ("oncontextmenu",
             "event.cancelBubble = true; event.returnValue = false; return false;");
    setModel(mMRIModel);
    resize(200, WLength::Auto);
    setSelectionMode(SingleSelection);
    expandToDepth(1);
    selectionChanged().connect(SLOT(this, MRIBrowser::mriChanged));
}

///
//  Destructor
//
MRIBrowser::~MRIBrowser()
{
    delete mMRIModel;
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
        MRID;

        char buf[1024] = {0};
        inFile.getline( buf, sizeof(buf));

        istringstream istr( string(buf), ios_base::out);
        istr >> scanName >> MRID;

        stringstream scanDir;
        scanDir << ConfigOptions::GetPtr()->GetDicomDir() << "/" << scanName;

        stringstream tocURL;

        tocURL << scanDir.str() << "/toc.txt";
        cout << tocURL.str() << endl;

        std::ifstream tocFile(tocURL.str().c_str());
        if (tocFile.is_open())
        {
            mMRIModel->appendRow(createMRIItem(MRID, scanDir.str()));
        }
    }
}


///
//  Create an MRI item.
//
WStandardItem *MRIBrowser::createMRIItem(const std::string& MRID,
                                         const std::string& scanDir)
{
    WStandardItem *result = new WStandardItem(MRID);

    result->setData(scanDir);
    result->setIcon("icons/folder.gif");

    return result;
}



///
//  MRI selection changed by user
//
void MRIBrowser::mriChanged()
{
}


