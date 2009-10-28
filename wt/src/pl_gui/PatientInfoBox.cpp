//
//
//  Description:
//      Implementation of patient info box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PatientInfoBox.h"
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
PatientInfoBox::PatientInfoBox(WContainerWidget *parent) :
    WGroupBox(parent)
{
    setTitle("Patient / Scanner");

    // Create the patient info box
    WGridLayout *patientInfoLayout = new WGridLayout();
    patientInfoLayout->addWidget(new WLabel("Patient ID:"), 0, 0);
    patientInfoLayout->addWidget(mPatientID = new WLabel(""), 0, 1);
    patientInfoLayout->addWidget(new WLabel("Patient Name:"), 1, 0);
    patientInfoLayout->addWidget(mPatientName = new WLabel(""), 1, 1);
    patientInfoLayout->addWidget(new WLabel("Patient Age:"), 2, 0);
    patientInfoLayout->addWidget(mPatientAge = new WLabel(""), 2, 1);
    patientInfoLayout->addWidget(new WLabel("Patient Sex:"), 3, 0);
    patientInfoLayout->addWidget(mPatientSex = new WLabel(""), 3, 1);
    patientInfoLayout->addWidget(new WLabel("Patient Birthday:"), 4, 0);
    patientInfoLayout->addWidget(mPatientBirthday = new WLabel(""), 4, 1);
    patientInfoLayout->addWidget(new WLabel("Image Scan-Date:"), 5, 0);
    patientInfoLayout->addWidget(mImageScanDate = new WLabel(""), 5, 1);
    patientInfoLayout->addWidget(new WLabel("Scanner Manufacturer:"), 6, 0);
    patientInfoLayout->addWidget(mScannerManufacturer = new WLabel(""), 6, 1);
    patientInfoLayout->addWidget(new WLabel("Scanner Model:"), 7, 0);
    patientInfoLayout->addWidget(mScannerModel = new WLabel(""), 7, 1);
    patientInfoLayout->addWidget(new WLabel("Software Version:"), 8, 0);
    patientInfoLayout->addWidget(mSoftwareVer = new WLabel(""), 8, 1);
    setLayout(patientInfoLayout);

    resetAll();
}

///
//  Destructor
//
PatientInfoBox::~PatientInfoBox()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset all widgets to the default state
//
void PatientInfoBox::resetAll()
{
    mPatientID->setText("");
    mPatientName->setText("");
    mPatientAge->setText("");
    mPatientSex->setText("");
    mPatientBirthday->setText("");
    mImageScanDate->setText("");
    mScannerManufacturer->setText("");
    mScannerModel->setText("");
    mSoftwareVer->setText("");
}

///
//  Set the scan directory
//
void PatientInfoBox::setScanDir(std::string scanDir)
{
    stringstream tocURL;

    tocURL << scanDir << "/toc.txt";

    // Parse table-of-contents file
    std::ifstream tocFile(tocURL.str().c_str());
    ostringstream oss;

    if (tocFile.is_open())
    {
        while (!tocFile.eof())
        {
            char buf[1024];
            tocFile.getline( buf, sizeof(buf));

            istringstream istr(string(buf), ios_base::out);
            string firstToken;
            istr >> firstToken;

            if (firstToken == "Patient")
            {
                string secondToken;
                string str;
                istr >> secondToken;

                while (!istr.eof())
                {
                    string tmp;
                    istr >> tmp;
                    str += tmp + " ";
                }

                if (secondToken == "ID")
                {
                    mPatientID->setText(str);
                }
                else if (secondToken == "Name")
                {
                    mPatientName->setText(str);
                }
                else if (secondToken == "Age")
                {
                    mPatientAge->setText(str);
                }
                else if (secondToken == "Sex")
                {
                    mPatientSex->setText(str);
                }
                else if (secondToken == "Birthday")
                {
                    mPatientBirthday->setText(str);
                }
            }
            else if (firstToken == "Image")
            {
                string secondToken;
                istr >> secondToken;

                if (secondToken == "Scan-Date")
                {
                    string scanDate;

                    istr >> scanDate;
                    mImageScanDate->setText(scanDate);
                }
            }
            else if (firstToken == "Scanner")
            {
                string secondToken;
                string str;
                istr >> secondToken;

                while (!istr.eof())
                {
                    string tmp;
                    istr >> tmp;
                    str += tmp + " ";
                }

                if (secondToken == "Manufacturer")
                {
                    mScannerManufacturer->setText(str);
                }
                else if (secondToken == "Model")
                {
                    mScannerModel->setText(str);
                }
            }
            else if (firstToken == "Software")
            {
                string secondToken;
                string str;
                istr >> secondToken;

                while (!istr.eof())
                {
                    string tmp;
                    istr >> tmp;
                    str += tmp + " ";
                }

                mSoftwareVer->setText(str);
            }
        }
        tocFile.close();
   }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//
