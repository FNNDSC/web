//
//
//  Description:
//      Definition of MRI info box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "MRIInfoBox.h"
#include "ConfigOptions.h"
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WTextArea>
#include <Wt/WStandardItem>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WMessageBox>
#include <Wt/WFont>
#include <Wt/WCssDecorationStyle>
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
MRIInfoBox::MRIInfoBox(WContainerWidget *parent) :
    WGroupBox(parent)
{
    setTitle("MRI Info");

    // Create the patient info box
    WGridLayout *mriInfoLayout = new WGridLayout();
    mMRIInfoTextArea = new WTextArea("");
    mMRIInfoTextArea->setStyleClass("mriinfodiv");
    mriInfoLayout->addWidget(mMRIInfoTextArea, 0, 0);
    mriInfoLayout->setRowStretch(0, -1);
    setLayout(mriInfoLayout);

    mMRIInfoTextArea->decorationStyle().font().setFamily(WFont::Monospace);

    resetAll();
}

///
//  Destructor
//
MRIInfoBox::~MRIInfoBox()
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
void MRIInfoBox::resetAll()
{
    mMRIInfoTextArea->setText("");
}

///
//  Set the DICOM file name
//
void MRIInfoBox::setDicomFileName(std::string dcmFileName)
{
    string mriFileStd = dcmFileName + ".std";
    string mriFileErr = dcmFileName + ".err";

    std::ifstream mriFile(mriFileStd.c_str(), ios::in);
    if (mriFile.is_open())
    {
        ostringstream oss;
        oss << mriFile.rdbuf();

        if (oss.str().length() > 1)
        {
            mMRIInfoTextArea->setText(oss.str());
        }
        else
        {
            std::ifstream errFile(mriFileErr.c_str(), ios::in);

            if (errFile.is_open())
            {
                oss << errFile.rdbuf();

                if (oss.str().length() > 1)
                {
                    mMRIInfoTextArea->setText(oss.str());
                }
                errFile.close();
            }
        }

        mriFile.close();
    }
    else
    {
        mMRIInfoTextArea->setText("No MRI info file found.");
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//
