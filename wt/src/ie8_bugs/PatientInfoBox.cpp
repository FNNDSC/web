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
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTreeView>
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

    mModel = new WStandardItemModel(this);
    mModel->insertColumns(0, 2);
    mModel->invisibleRootItem()->setRowCount(0);

    mModel->insertRows(0, NUM_ROWS);
    mModel->setData(PATIENT_ID, 0, boost::any(std::string("Patient ID")));
    mModel->setData(PATIENT_NAME, 0, boost::any(std::string("Patient Name")));
    mModel->setData(PATIENT_AGE, 0, boost::any(std::string("Patient Age")));
    mModel->setData(PATIENT_SEX, 0, boost::any(std::string("Patient Sex")));
    mModel->setData(PATIENT_BIRTHDAY, 0, boost::any(std::string("Patient Birthday")));
    mModel->setData(IMAGE_SCAN_DATE, 0, boost::any(std::string("Image Scan Date")));
    mModel->setData(SCANNER_MANUFACTURER, 0, boost::any(std::string("Scanner Manufacturer")));
    mModel->setData(SCANNER_MODEL, 0, boost::any(std::string("Scanner Model")));
    mModel->setData(SOFTWARE_VERSION, 0, boost::any(std::string("Software Version")));

    mTreeView = new WTreeView();
    mTreeView->setModel(mModel);
    mTreeView->setRootIsDecorated(false);
    mTreeView->setSelectionMode(NoSelection);
    mTreeView->setSortingEnabled(false);
    mTreeView->setAlternatingRowColors(true);
    mTreeView->setHeaderHeight(0);
    mTreeView->setMinimumSize(300, WLength::Auto);

    // Create the patient info box
    WGridLayout *patientInfoLayout = new WGridLayout();
    patientInfoLayout->addWidget(mTreeView, 0, 0);
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
    for(int row = 0; row < NUM_ROWS; row++)
    {
        mModel->setData(row, 1, boost::any(std::string("")));
    }
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
                    setInfoData(PATIENT_ID, str);
                }
                else if (secondToken == "Name")
                {
                    setInfoData(PATIENT_NAME, str);
                }
                else if (secondToken == "Age")
                {
                    setInfoData(PATIENT_AGE, str);
                }
                else if (secondToken == "Sex")
                {
                    setInfoData(PATIENT_SEX, str);
                }
                else if (secondToken == "Birthday")
                {
                    setInfoData(PATIENT_BIRTHDAY, str);
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
                    setInfoData(IMAGE_SCAN_DATE, scanDate);
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
                    setInfoData(SCANNER_MANUFACTURER, str);
                }
                else if (secondToken == "Model")
                {
                    setInfoData(SCANNER_MODEL, str);
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

                setInfoData(SOFTWARE_VERSION, str);
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

///
//	Set the patient information text, do not display "ERROR:" if that is what is present
//
void PatientInfoBox::setInfoData(PatientEnum patientEnum, const std::string& str)
{
    boost::any data;

	if (str.find("ERROR") == string::npos)
	{
		data = boost::any(str);
	}
	else
	{
		data = boost::any(std::string("Unknown"));
	}

	mModel->setData(patientEnum, 1, data);
}
