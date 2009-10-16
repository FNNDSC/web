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
ScanBrowser::ScanBrowser(WContainerWidget *parent) :
    WContainerWidget(parent)
{
    mPatientInfoBox = new WGroupBox("Patient / Scanner");
    mScanBox = new WGroupBox("Scans");


    mPatientInfoBox->setStyleClass("groupdiv");
    mScanBox->setStyleClass("groupdiv");

    mPatientInfoLabel = new WLabel("");
    mPatientInfoLabel->setStyleClass("whitelabeldiv");
    mScansSelectionBox = new WSelectionBox();
    mScansSelectionBox->setStyleClass("groupdiv");
    mScansSelectionBox->setVerticalSize(15);

    WVBoxLayout *patientInfoLayout = new WVBoxLayout();
    patientInfoLayout->addWidget(mPatientInfoLabel);
    mPatientInfoBox->setLayout(patientInfoLayout);

    WVBoxLayout *scanLayout = new WVBoxLayout();
    scanLayout->addWidget(mScansSelectionBox);
    mScanBox->setLayout(scanLayout);


    WVBoxLayout *layout = new WVBoxLayout();
    layout->addWidget(mPatientInfoBox);
    layout->addWidget(mScanBox);

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

