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
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include "ScansToProcessTable.h"
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
    mPatientInfoBox = new PatientInfoBox();
    mMRIInfoBox = new MRIInfoBox();
    mScansToProcessBox = new WGroupBox("Scans to Process");

    mMRIInfoBox->setStyleClass("groupdiv");
    mPatientInfoBox->setStyleClass("verysmallgroupdiv");
    mScansToProcessBox->setStyleClass("groupdiv");

    mScansSelectionBox = new WSelectionBox();
    mScansSelectionBox->setStyleClass("groupdiv");
    mScansSelectionBox->setSelectionMode(Wt::ExtendedSelection);


    mPipelineModeLabel = new WLabel("");
    mPipelineOverrideButton = new WPushButton("Choose Pipeline...");

    // Determine the maximum number of inputs that any pipeline
    // uses and setup the GUI according to this
    int maxInputs = getConfigXMLPtr()->getMaxPipelineInputs();

    WHBoxLayout *addScanButtonLayout = new WHBoxLayout();
    for (int i = 0; i < maxInputs; i++ )
    {
        mAddScanButtonList.push_back(new WPushButton("Add"));
        addScanButtonLayout->addWidget(mAddScanButtonList[i]);
        if (i == 0)
        {
            mAddScanButtonList[i]->disable();
        }
        else
        {
            mAddScanButtonList[i]->hide();
        }

        mAddScanButtonList[i]->clicked().connect(SLOT(this, ScanBrowser::addScanClicked));
    }

    WGridLayout *scanLayout = new WGridLayout();
    scanLayout->addWidget(mScansSelectionBox, 0, 0);
    scanLayout->addLayout(addScanButtonLayout, 1, 0, Wt::AlignCenter);
    scanLayout->setRowStretch(0, 1);

    WGridLayout *scansToProcessLayout = new WGridLayout();
    scansToProcessLayout->setRowStretch(0, 1);
    mScansToProcessTable = new ScansToProcessTable();
    mRemoveScanButton = new WPushButton("Remove");
    scansToProcessLayout->addWidget(mScansToProcessTable, 0, 0, Wt::AlignLeft);
    scansToProcessLayout->addWidget(mRemoveScanButton, 1, 0, Wt::AlignCenter);

    WHBoxLayout *hBox = new WHBoxLayout();
    hBox->addLayout(scanLayout);
    hBox->addLayout(scansToProcessLayout);

    WHBoxLayout *pipelineTypeLayout = new WHBoxLayout();
    pipelineTypeLayout->addWidget(mPipelineOverrideButton);
    pipelineTypeLayout->addWidget(mPipelineModeLabel);

    WGridLayout *scansToProcessBoxLayout = new WGridLayout();
    scansToProcessBoxLayout->addLayout(hBox, 0, 0);
    scansToProcessBoxLayout->addLayout(pipelineTypeLayout, 1, 0, Wt::AlignCenter);
    scansToProcessBoxLayout->setRowStretch(0, 1);
    mScansToProcessBox->setLayout(scansToProcessBoxLayout);


    WGridLayout *topLayout = new WGridLayout();
    topLayout->addWidget(mPatientInfoBox, 0, 0);
    topLayout->addWidget(mMRIInfoBox, 0, 1);
    topLayout->setColumnStretch(1, 1);


    WGridLayout *layout = new WGridLayout();
    layout->addWidget(mScansToProcessBox, 0, 0);
    layout->addLayout(topLayout, 1, 0);
    layout->setRowStretch(0, -1);
    layout->setRowStretch(1, -1);

    mMessageBox = new WMessageBox();
    mMessageBox->buttonClicked().connect(SLOT(this, ScanBrowser::handleMessageBoxFinished));


    // Connect signals to slots
    mScansSelectionBox->changed().connect(SLOT(this, ScanBrowser::scanSelectionChanged));
    mPipelineOverrideButton->clicked().connect(SLOT(this, ScanBrowser::pipelineOverrideClicked));
    mRemoveScanButton->clicked().connect(SLOT(this, ScanBrowser::removeScanClicked));
    mScansToProcessTable->scanSelected().connect(SLOT(this, ScanBrowser::scanSelectedForRemove));

    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
ScanBrowser::~ScanBrowser()
{
    delete mMessageBox;
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

    mPatientInfoBox->resetAll();
    mMRIInfoBox->resetAll();
    mScansToProcessTable->resetAll();

    mRemoveScanButton->disable();

    for (int i = 0; i < mAddScanButtonList.size(); i++)
    {
        if (i == 0)
        {
            mAddScanButtonList[i]->setText("Add");
            mAddScanButtonList[i]->disable();
        }
        else
        {
            mAddScanButtonList[i]->hide();
        }
    }

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
    mPatientInfoBox->setScanDir(scanDir);

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

///
//  Get pipeline type as string (for command line arg)
//
const std::string ScanBrowser::getCurrentPipelineAsString() const
{
    if (getCurrentPipeline() == Enums::PIPELINE_TYPE_TRACT)
    {
        return std::string("tract");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_FS)
    {
        return std::string("fs");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_FETAL)
    {
        return std::string("fetal");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_CONNECTOME)
    {
        return std::string("connectome");
    }
    else if (getCurrentPipeline() == Enums::PIPELINE_TYPE_DCMSEND)
    {
        return std::string("dcmsend");
    }
    else
    {
        return std::string("unknown");
    }
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
    WObject *sender = WObject::sender();

    // Find which add button sent this signal
    int index = 0;
    while(index < mAddScanButtonList.size())
    {
        if ( (WObject*)mAddScanButtonList[index] == sender)
        {
            break;
        }
        index++;
    }
    if (index >= mAddScanButtonList.size())
    {
        WApplication::instance()->log("error") << "Unable to determine which button sent message.";
        return;
    }

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

        mScansToProcessTable->addEntry(index, mNewScanData);

        mScanAdded.emit(mScansToProcessTable->validateTable());
        iter++;
    }
}


///
/// Get the list of currently selected scans
///
std::vector<ScansToProcessTable::ScanData> ScanBrowser::getScansToProcess(int inputIndex) const
{
    return mScansToProcessTable->getScansToProcess(inputIndex);
}

///
//  Remove scan clicked [slot]
//
void ScanBrowser::removeScanClicked()
{
    mScansToProcessTable->removeSelected();
    mRemoveScanButton->disable();

    // Emit a signal with whether a scan is selected
    mScanAdded.emit(mScansToProcessTable->validateTable());
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

    WRadioButton *connectomeButton = new WRadioButton("Connectome",
                    mPipelineDialog->contents());
    group->addButton(connectomeButton, Enums::PIPELINE_TYPE_CONNECTOME + 1);
    new WBreak(mPipelineDialog->contents());

    if (mPipelineType != Enums::PIPELINE_UNKNOWN)
    {
        group->setCheckedButton(group->button(mPipelineType + 1));
    }
    else
    {
        group->setCheckedButton(group->button(Enums::PIPELINE_TYPE_FS + 1));
    }

    new WBreak(mPipelineDialog->contents());

    WPushButton *ok = new WPushButton("OK", mPipelineDialog->contents());
    WPushButton *cancel = new WPushButton("Cancel", mPipelineDialog->contents());

    ok->clicked().connect(SLOT(mPipelineDialog, WDialog::accept));
    cancel->clicked().connect(SLOT(mPipelineDialog, WDialog::reject));

    new WBreak(mPipelineDialog->contents());
    new WBreak(mPipelineDialog->contents());

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

        // Get the input list associated with the current pipeline
        std::string curPipeline = getCurrentPipelineAsString() + "_meta.bash";
        const std::vector<ConfigXML::InputNode>* inputList = getConfigXMLPtr()->getPipelineInputs(curPipeline);
        if (inputList != NULL)
        {
            mScansToProcessTable->setPipelineInputs(*inputList);

            int i = 0;
            for (i = 0; i < inputList->size(); i++)
            {
                mAddScanButtonList[i]->setText("Add " + (*inputList)[i].mName);
                mAddScanButtonList[i]->show();
                mAddScanButtonList[i]->enable();
            }

            for( ; i < mAddScanButtonList.size(); i++)
            {
                mAddScanButtonList[i]->hide();
            }
        }
        else
        {
            for(int i = 0 ; i < mAddScanButtonList.size(); i++)
            {
                if (i == 0)
                {
                    mAddScanButtonList[i]->setText("Add");
                    mAddScanButtonList[i]->disable();
                }
                else
                {
                    mAddScanButtonList[i]->hide();
                }
            }
        }

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
        mPipelineModeLabel->setText("Choose pipeline to continue");
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
        mPipelineModeLabel->setText("Pipeline Type: Send to Remote PACS");
        break;
    case Enums::PIPELINE_TYPE_CONNECTOME:
        mPipelineModeLabel->setText("Pipeline Type: Connectome");
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
/// Handle scan selected for remove
///
void ScanBrowser::scanSelectedForRemove(bool selected)
{
    if (selected)
    {
        mRemoveScanButton->enable();
    }
    else
    {
        mRemoveScanButton->disable();
    }
}

