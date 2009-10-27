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

    mScansSelectionBox = new WSelectionBox();
    mScansSelectionBox->setStyleClass("groupdiv");
    mScansSelectionBox->setSelectionMode(Wt::ExtendedSelection);
    mScansToProcessList = new WSelectionBox();
    mScansToProcessList->setStyleClass("groupdiv");
    mScansToProcessList->setSelectionMode(Wt::ExtendedSelection);

    mPipelineModeLabel = new WLabel("");

    mAddScanButton = new WPushButton("Add");
    mRemoveScanButton = new WPushButton("Remove");

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
    patientInfoLayout->addWidget(new WLabel("Scanner Manufacturer:"), 5, 0);
    patientInfoLayout->addWidget(mScannerManufacturer = new WLabel(""), 5, 1);
    patientInfoLayout->addWidget(new WLabel("Scanner Model:"), 6, 0);
    patientInfoLayout->addWidget(mScannerModel = new WLabel(""), 6, 1);
    patientInfoLayout->addWidget(new WLabel("Software Version:"), 7, 0);
    patientInfoLayout->addWidget(mSoftwareVer = new WLabel(""), 7, 1);

    mPatientInfoBox->setLayout(patientInfoLayout);

    WGridLayout *scanLayout = new WGridLayout();
    scanLayout->addWidget(mScansSelectionBox, 0, 0);
    scanLayout->addWidget(mAddScanButton, 1, 0, Wt::AlignCenter);
    scanLayout->setRowStretch(0, 1);
    mScanBox->setLayout(scanLayout);

    WGridLayout *scansToProcessLayout = new WGridLayout();
    scansToProcessLayout->addWidget(mScansToProcessList, 0, 0);
    scansToProcessLayout->addWidget(mRemoveScanButton, 1, 0, Wt::AlignCenter);
    scansToProcessLayout->addWidget(mPipelineModeLabel, 2, 0, Wt::AlignCenter);
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

    resetAll();
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
//  Reset all widgets to the default state
//
void ScanBrowser::resetAll()
{
    mPipelineType = Enums::PIPELINE_UNKNOWN;
    mScansSelectionBox->clear();
    mScansDicomFiles.clear();
    mScansDate = "";
    mScansSelectionBox->setCurrentIndex(0);
    setCurrentPipeline(Enums::PIPELINE_UNKNOWN);
    mScansToProcessList->clear();
    mScansToProcessData.clear();

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
    mScansDate = "";
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
            else if (firstToken == "Patient")
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
                    mScansDate = scanDate;
                    mImageScanDate->setText(mScansDate);
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


///
//  Set the current MRID
//
void ScanBrowser::setCurMRID(std::string mrid)
{
    mCurMRID = mrid;
}


///
//  Set the curreng age
//
void ScanBrowser::setCurAge(std::string age)
{
    mCurAge = age;
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
        newScanData.mScanDate = mScansDate;
        newScanData.mScanName = curScanText.toUTF8();
        newScanData.mScanDir = mCurScanDir;
        newScanData.mAge = mCurAge;

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
            Enums::PipelineType pipelineType = Enums::PIPELINE_UNKNOWN;

            if (findSeriesMatch(ConfigOptions::GetPtr()->GetSeriesListTract(), curScanText.toUTF8()))
            {
                pipelineType = Enums::PIPELINE_TYPE_TRACT;
            }
            else if(findSeriesMatch(ConfigOptions::GetPtr()->GetSeriesListFS(), curScanText.toUTF8()))
            {
                pipelineType = Enums::PIPELINE_TYPE_FS;
            }

            if (pipelineType != Enums::PIPELINE_UNKNOWN)
            {
                if (mPipelineType == Enums::PIPELINE_UNKNOWN)
                {
                    setCurrentPipeline(pipelineType);
                }
                else
                {
                    if (pipelineType != mPipelineType)
                    {
                        StandardButton result =
                                WMessageBox::show("Pipeline Mismatch",
                                                  "[MRID:] " + newScanData.mMRID +
                                                  " [Scan:] " + newScanData.mScanName + " does not match current pipeline type." +
                                                  "\nAre you sure you want to add it?",
                                                  Wt::Yes | Wt::No);

                        if (result == Wt::No)
                        {
                            addScan = false;
                        }
                    }
                }
            }

            if (addScan)
            {
                mScansToProcessList->addItem("[MRID:] " + mCurMRID + " [Scan:] " + curScanText);
                mScansToProcessData.push_back(newScanData);
            }
        }

        iter++;
    }

    // Emit a signal with whether a scan is selected
    mScanAdded.emit(!mScansToProcessData.empty());

    if (mScansToProcessData.empty())
    {
        setCurrentPipeline(Enums::PIPELINE_UNKNOWN);
    }
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

    if (mScansToProcessData.empty())
    {
        setCurrentPipeline(Enums::PIPELINE_UNKNOWN);
    }
}

///
// Set the current pipeline type
//
void ScanBrowser::setCurrentPipeline(Enums::PipelineType pipelineType)
{
    mPipelineType = pipelineType;
    switch(mPipelineType)
    {
    case Enums::PIPELINE_UNKNOWN:
        mPipelineModeLabel->setText("Pipeline Type: UNKNOWN");
        break;
    case Enums::PIPELINE_TYPE_TRACT:
        mPipelineModeLabel->setText("Pipeline Type: Tractography");
        break;
    case Enums::PIPELINE_TYPE_FS:
        mPipelineModeLabel->setText("Pipeline Type: Structural Reconstruction");
        break;
    }
}

///
//  Find a match between a string a comma separated list of strings
//
bool ScanBrowser::findSeriesMatch(const std::string& seriesList,
                                  const std::string& seriesName) const
{
    std::string token;
    std::istringstream iss(seriesList);

    while (getline(iss, token, ','))
    {
        if(seriesName.find(token) != string::npos)
        {
            return true;
        }
    }

    return false;
}

