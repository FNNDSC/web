//
//
//  Description:
//      Implementation of the Scan browser widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "ScanBrowser.h"
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WMessageBox>
#include <fstream>
#include <iostream>
#include <string>
#include <set>

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
ScanBrowser::ScanBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    mPatientInfoBox = new WGroupBox("Patient / Scanner");
    mScanBox = new WGroupBox("Scans");
    mScansToProcessBox = new WGroupBox("Scans to Process");

    mPatientInfoBox->setStyleClass("groupdiv");
    mScanBox->setStyleClass("groupdiv");
    mScansToProcessBox->setStyleClass("groupdiv");

    mPatientInfoLabel = new WLabel("");
    mPatientInfoLabel->setStyleClass("whitelabeldiv");
    mScansSelectionBox = new WSelectionBox();
    mScansSelectionBox->setStyleClass("groupdiv");
    mScansSelectionBox->setSelectionMode(Wt::ExtendedSelection);
    mScansToProcessList = new WSelectionBox();
    mScansToProcessList->setStyleClass("groupdiv");
    mScansToProcessList->setSelectionMode(Wt::ExtendedSelection);

    mAddScanButton = new WPushButton("Add");
    mRemoveScanButton = new WPushButton("Remove");

    WVBoxLayout *patientInfoLayout = new WVBoxLayout();
    patientInfoLayout->addWidget(mPatientInfoLabel);
    mPatientInfoBox->setLayout(patientInfoLayout);

    WGridLayout *scanLayout = new WGridLayout();
    scanLayout->addWidget(mScansSelectionBox, 0, 0);
    scanLayout->addWidget(mAddScanButton, 1, 0, Wt::AlignCenter);
    scanLayout->setRowStretch(0, 1);
    mScanBox->setLayout(scanLayout);

    WGridLayout *scansToProcessLayout = new WGridLayout();
    scansToProcessLayout->addWidget(mScansToProcessList, 0, 0);
    scansToProcessLayout->addWidget(mRemoveScanButton, 1, 0, Wt::AlignCenter);
    scansToProcessLayout->setRowStretch(0, 1);
    mScansToProcessBox->setLayout(scansToProcessLayout);

    WGridLayout *layout = new WGridLayout();

    WVBoxLayout *leftLayout = new WVBoxLayout();
    leftLayout->addWidget(mPatientInfoBox);
    leftLayout->addWidget(mScanBox);

    WVBoxLayout *rightLayout = new WVBoxLayout();
    rightLayout->addWidget(mScansToProcessBox);

    layout->addLayout(leftLayout, 0, 0);
    layout->addLayout(rightLayout, 0, 1);
    layout->setColumnStretch(1, 1);

    // Connect signals to slots
    mAddScanButton->clicked().connect(SLOT(this, ScanBrowser::addScanClicked));
    mRemoveScanButton->clicked().connect(SLOT(this, ScanBrowser::removeScanClicked));


    setLayout(layout);

}

///
//  Destructor
//
ScanBrowser::~ScanBrowser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Set the scan directory
//
void ScanBrowser::setScanDir(std::string scanDir)
{
    stringstream tocURL;

    // Store the scan directory for future processing
    mCurScanDir = scanDir;

    tocURL << scanDir << "/toc.txt";

    // Parse table-of-contents file
    std::ifstream tocFile(tocURL.str().c_str());
    ostringstream oss;


    mScansSelectionBox->clear();
    mScansDicomFiles.clear();
    mScansSelectionBox->setCurrentIndex(0);

    if (tocFile.is_open())
    {
        while (!tocFile.eof())
        {
            char buf[1024];
            tocFile.getline( buf, sizeof(buf));

            istringstream istr(string(buf), ios_base::out);
            string firstToken;
            istr >> firstToken;

            if (firstToken == "Scan")
            {
                string dicomFile,
                       scanName;

                istr >> dicomFile;
                while (!istr.eof())
                {
                    string tmp;

                    istr >> tmp;
                    scanName += tmp + " ";
                }

                mScansSelectionBox->addItem(scanName);
                mScansDicomFiles.push_back(dicomFile);

            }
            else
            {
               oss << string(buf) << "<br/>";
            }
        }

        mPatientInfoLabel->setText(oss.str());

        tocFile.close();
    }
}


///
//  Set the current MRID
//
void ScanBrowser::setCurMRID(std::string mrid)
{
    mCurMRID = mrid;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Add scan clicked [slot]
//
void ScanBrowser::addScanClicked()
{

    const set<int>& selectedSet = mScansSelectionBox->selectedIndexes();
    set<int>::const_iterator iter = selectedSet.begin();

    while (iter != selectedSet.end())
    {
        WString curScanText = mScansSelectionBox->itemText(*iter);
        ScanData newScanData;

        newScanData.mMRID = mCurMRID;
        newScanData.mDicomFile = mScansDicomFiles[*iter];
        newScanData.mScanName = curScanText.toUTF8();
        newScanData.mScanDir = mCurScanDir;

        bool addScan = true;
        for (int i = 0; i < mScansToProcessData.size(); i++)
        {
            // Check to see if it has already been added, and if so display
            // a message to the user
            if(newScanData.mMRID == mScansToProcessData[i].mMRID &&
               newScanData.mDicomFile == mScansToProcessData[i].mDicomFile &&
               newScanData.mScanName == mScansToProcessData[i].mScanName)
            {
                StandardButton result = WMessageBox::show("Scan Already Selected",
                                                          "Can not add scan: [MRID:] " + newScanData.mMRID +
                                                          " [Scan:] " + newScanData.mScanName + "\nIt has already been selected for processing.",
                                                          Wt::Ok);
                addScan = false;
                break;
            }
        }

        if (addScan)
        {
            mScansToProcessList->addItem("[MRID:] " + mCurMRID + " [Scan:] " + curScanText);
            mScansToProcessData.push_back(newScanData);
        }

        iter++;
    }

    // Emit a signal with whehter a scan is selected
    mScanAdded.emit(!mScansToProcessData.empty());
}


///
//  Remove scan clicked [slot]
//
void ScanBrowser::removeScanClicked()
{
    const set<int>& selectedSet = mScansToProcessList->selectedIndexes();
    set<int>::const_iterator iter = selectedSet.begin();

    while (iter != selectedSet.end())
    {
        int index = *iter;

        mScansToProcessData.erase(mScansToProcessData.begin() + index);
        mScansToProcessList->removeItem(index);

        iter++;
    }

    // Emit a signal with whehter a scan is selected
    mScanAdded.emit(!mScansToProcessData.empty());
}

