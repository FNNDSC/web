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
#include "PipelineApp.h"
#include "ScanBrowser.h"
#include "PatientInfoBox.h"
#include "MRIInfoBox.h"
//#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WMessageBox>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WDialog>
#include <Wt/WLogger>
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
    WContainerWidget(parent),
    mPipelineDialog(NULL),
    mPipelineDialogGroup(NULL)
{
    //mPatientInfoBox = new PatientInfoBox();
    mMRIInfoBox = new MRIInfoBox();
    mScansToProcessBox = new WGroupBox("Scans to Process");

    mMRIInfoBox->setStyleClass("groupdiv");
    //mPatientInfoBox->setStyleClass("verysmallgroupdiv");
    mScansToProcessBox->setStyleClass("groupdiv");

    mScansSelectionBox = new WSelectionBox();
    mScansSelectionBox->setStyleClass("groupdiv");
    mScansSelectionBox->setSelectionMode(Wt::ExtendedSelection);
    mScansToProcessList = new WSelectionBox();
    mScansToProcessList->setStyleClass("groupdiv");
    mScansToProcessList->setSelectionMode(Wt::ExtendedSelection);

    mPipelineModeLabel = new WLabel("");
    mPipelineOverrideButton = new WPushButton("Override");

    mAddScanButton = new WPushButton("Add");
    mRemoveScanButton = new WPushButton("Remove");

    WGridLayout *scanLayout = new WGridLayout();
    scanLayout->addWidget(mScansSelectionBox, 0, 0);
    scanLayout->addWidget(mAddScanButton, 1, 0, Wt::AlignCenter);
    scanLayout->setRowStretch(0, 1);

    WGridLayout *scansToProcessLayout = new WGridLayout();
    scansToProcessLayout->addWidget(mScansToProcessList, 0, 0);
    scansToProcessLayout->addWidget(mRemoveScanButton, 1, 0, Wt::AlignCenter);
    scansToProcessLayout->setRowStretch(0, 1);


    WHBoxLayout *hBox = new WHBoxLayout();
    hBox->addLayout(scanLayout);
    hBox->addLayout(scansToProcessLayout);

    WHBoxLayout *pipelineTypeLayout = new WHBoxLayout();
    pipelineTypeLayout->addWidget(mPipelineModeLabel);
    pipelineTypeLayout->addWidget(mPipelineOverrideButton);

    WGridLayout *scansToProcessBoxLayout = new WGridLayout();
    scansToProcessBoxLayout->addLayout(hBox, 0, 0);
    scansToProcessBoxLayout->addLayout(pipelineTypeLayout, 1, 0, Wt::AlignCenter);
    scansToProcessBoxLayout->setRowStretch(0, 1);
    mScansToProcessBox->setLayout(scansToProcessBoxLayout);


    WGridLayout *topLayout = new WGridLayout();
    //topLayout->addWidget(mPatientInfoBox, 0, 0);
    topLayout->addWidget(mScansToProcessBox, 0, 1);
    topLayout->setColumnStretch(1, 1);


    WGridLayout *layout = new WGridLayout();
    layout->addLayout(topLayout, 0, 0);
    layout->addWidget(mMRIInfoBox, 1, 0);
    layout->setRowStretch(0, -1);
    layout->setRowStretch(1, -1);

    mMessageBox = new WMessageBox();
    mMessageBox->buttonClicked().connect(SLOT(this, ScanBrowser::handleMessageBoxFinished));
    mAddScanMessageBox = new WMessageBox;
    mAddScanMessageBox->buttonClicked().connect(SLOT(this, ScanBrowser::handleAddScanFinished));


    // Connect signals to slots
    mScansSelectionBox->changed().connect(SLOT(this, ScanBrowser::scanSelectionChanged));
    mAddScanButton->clicked().connect(SLOT(this, ScanBrowser::addScanClicked));
    mRemoveScanButton->clicked().connect(SLOT(this, ScanBrowser::removeScanClicked));
    mPipelineOverrideButton->clicked().connect(SLOT(this, ScanBrowser::pipelineOverrideClicked));

    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
ScanBrowser::~ScanBrowser()
{
    delete mMessageBox;
    delete mAddScanMessageBox;
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

    //mPatientInfoBox->resetAll();
    mMRIInfoBox->resetAll();
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

    // Load the patient info box
    //mPatientInfoBox->setScanDir(scanDir);

    // Clear the MRI info
    mMRIInfoBox->resetAll();

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

                if (scanName == "" || scanName == " ")
                {
                    scanName = "unnamed";
                }

                mScansSelectionBox->addItem(scanName);
                mScansDicomFiles.push_back(dicomFile);
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
                }
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

        mNewScanData.mMRID = mCurMRID;
        mNewScanData.mDicomFile = mScansDicomFiles[*iter];
        mNewScanData.mScanDate = mScansDate;
        mNewScanData.mScanName = curScanText.toUTF8();
        mNewScanData.mScanDir = mCurScanDir;
        mNewScanData.mAge = mCurAge;

        for (int i = 0; i < mScansToProcessData.size(); i++)
        {
            // Check to see if it has already been added, and if so display
            // a message to the user
            if(mNewScanData.mMRID == mScansToProcessData[i].mMRID &&
               mNewScanData.mDicomFile == mScansToProcessData[i].mDicomFile &&
               mNewScanData.mScanName == mScansToProcessData[i].mScanName)
            {
                mMessageBox->setWindowTitle("Scan Already Selected");
                mMessageBox->setText("Can not add scan: [MRID:] " + mNewScanData.mMRID +
                                     " [Scan:] " + mNewScanData.mScanName + "\nIt has already been selected for processing.");
                mMessageBox->setButtons(Wt::Ok);
                mMessageBox->show();
                return;
            }
        }

        Enums::PipelineType pipelineType = Enums::PIPELINE_UNKNOWN;

        /*if (findSeriesMatch(getConfigOptionsPtr()->GetSeriesListTract(), curScanText.toUTF8()))
        {
            pipelineType = Enums::PIPELINE_TYPE_TRACT;
        }
        else if(findSeriesMatch(getConfigOptionsPtr()->GetSeriesListFS(), curScanText.toUTF8()))
        {
            pipelineType = Enums::PIPELINE_TYPE_FS;
        }
        else if(findSeriesMatch(getConfigOptionsPtr()->GetSeriesListFetal(), curScanText.toUTF8()))
        {
            pipelineType = Enums::PIPELINE_TYPE_FETAL;
        }*/

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
                    // Used to display a message here, but do nothing.  It does not require user interaction.
                }
            }
        }

        // Force the scan to be added as if the user clicked "Yes"
        handleAddScanFinished(Wt::Yes);

        iter++;
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

    // Emit a signal with whether a scan is selected
    mScanAdded.emit(!mScansToProcessData.empty());

    if (mScansToProcessData.empty())
    {
        setCurrentPipeline(Enums::PIPELINE_UNKNOWN);
    }
}

///
//  Pipeline override clicked [slot]
//
void ScanBrowser::pipelineOverrideClicked()
{
    if (mPipelineDialog != NULL)
    {
        delete mPipelineDialog;
        delete mPipelineDialogGroup;
    }

    mPipelineDialog = new WDialog("Select Pipeline:");
    mPipelineDialogGroup = new WButtonGroup(mPipelineDialog->contents());

    WText *text = new WText(
                            "The pipeline type is automatically detected based on the name of the sequence.<BR/>"
                                "  If you need to override the automatically detected pipeline, select it below:",
                                mPipelineDialog->contents());

    new WBreak(mPipelineDialog->contents());
    new WBreak(mPipelineDialog->contents());

    // Use a button group to logically group the 3 options
    WButtonGroup *group = mPipelineDialogGroup;
    WRadioButton *structButton = new WRadioButton("Structural Reconstruction",
            mPipelineDialog->contents());
    group->addButton(structButton, Enums::PIPELINE_TYPE_FS + 1);
    new WBreak(mPipelineDialog->contents());

    WRadioButton *tractButton = new WRadioButton("Tractography",
            mPipelineDialog->contents());
    group->addButton(tractButton, Enums::PIPELINE_TYPE_TRACT + 1);
    new WBreak(mPipelineDialog->contents());

    WRadioButton *fetalButton = new WRadioButton("Fetal Extraction",
            mPipelineDialog->contents());
    group->addButton(fetalButton, Enums::PIPELINE_TYPE_FETAL + 1);
    new WBreak(mPipelineDialog->contents());

    WRadioButton *dcmSendButton = new WRadioButton("Send to Remote PACS",
                mPipelineDialog->contents());
    group->addButton(dcmSendButton, Enums::PIPELINE_TYPE_DCMSEND + 1);
    new WBreak(mPipelineDialog->contents());

    WRadioButton *unknownButton = new WRadioButton("Unknown",
            mPipelineDialog->contents());
    group->addButton(unknownButton, Enums::PIPELINE_UNKNOWN + 1);
    new WBreak(mPipelineDialog->contents());

    group->setCheckedButton(group->button(mPipelineType + 1));

    new WBreak(mPipelineDialog->contents());

    WPushButton *ok = new WPushButton("OK", mPipelineDialog->contents());
    WPushButton *cancel = new WPushButton("Cancel", mPipelineDialog->contents());

    ok->clicked().connect(SLOT(mPipelineDialog, WDialog::accept));
    cancel->clicked().connect(SLOT(mPipelineDialog, WDialog::reject));

    mPipelineDialog->finished().connect(SLOT(this, ScanBrowser::handlePipelineDialogClosed));

    mPipelineDialog->show();
}

///
//  Handle pipeline dialog closed [slot]
//
void ScanBrowser::handlePipelineDialogClosed(WDialog::DialogCode dialogCode)
{
    if (dialogCode == WDialog::Accepted)
    {
        setCurrentPipeline((Enums::PipelineType)(mPipelineDialogGroup->checkedId() - 1));
    }

    delete mPipelineDialog;
    mPipelineDialog = NULL;
    mPipelineDialogGroup = NULL;
}


///
//  Scan selection changed
//
void ScanBrowser::scanSelectionChanged()
{
    const set<int>& selectedSet = mScansSelectionBox->selectedIndexes();
    set<int>::const_iterator iter = selectedSet.begin();

    // Use the first selected scan
    if (iter != selectedSet.end())
    {
        int index = *iter;

        if (index >= 0 && index < mScansDicomFiles.size())
        {
            string dicomFile = mScansDicomFiles[index];

            mMRIInfoBox->setDicomFileName(mCurScanDir + "/mri_info/" + dicomFile);
        }
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
    case Enums::PIPELINE_TYPE_FETAL:
        mPipelineModeLabel->setText("Pipeline Type: Fetal Extraction");
        break;
    case Enums::PIPELINE_TYPE_FS:
        mPipelineModeLabel->setText("Pipeline Type: Structural Reconstruction");
        break;
    case Enums::PIPELINE_TYPE_DCMSEND:
        mPipelineModeLabel->setText("Send to Remote PACS");
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

///
//  Handle message box finished [slot]
//
void ScanBrowser::handleMessageBoxFinished(StandardButton)
{
    mMessageBox->hide();
}

///
//  Handle message box finished [slot]
//
void ScanBrowser::handleAddScanFinished(StandardButton button)
{
    if (button == Wt::Yes)
    {
        mScansToProcessList->addItem("[MRID:] " + mNewScanData.mMRID + " [Scan:] " + mNewScanData.mScanName);
        mScansToProcessData.push_back(mNewScanData);
    }

    // Emit a signal with whether a scan is selected
    mScanAdded.emit(!mScansToProcessData.empty());

    if (mScansToProcessData.empty())
    {
        setCurrentPipeline(Enums::PIPELINE_UNKNOWN);
    }

    mAddScanMessageBox->hide();
}

