//
//
//  Description:
//      Implementation of file preview box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "PipelineApp.h"
#include "FilePreviewBox.h"
#include "ConfigOptions.h"
#include "ConfigXML.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WLabel>
#include <Wt/WStandardItem>
#include <Wt/WGroupBox>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WMessageBox>
#include <Wt/WFileResource>
#include <Wt/WAnchor>
#include <Wt/WFileResource>
#include <Wt/WStackedWidget>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
FilePreviewBox::FilePreviewBox(WContainerWidget *parent) :
    WGroupBox(parent),
    mImageResource(NULL)
{
    setTitle("File Info");

    mImagePreview = new WImage();
    mTextPreview = new WTextArea();
    mTextPreview->setStyleClass("logdiv");
    mTextPreview->decorationStyle().font().setFamily(WFont::Monospace);

    mPreviewStack = new WStackedWidget();
    mPreviewStack->addWidget(mImagePreview);
    mPreviewStack->addWidget(mTextPreview);


    // Create an anchor that references a URL
    mDownloadFileResource = new WFileResource("application/octet-stream", "");
    mDownloadAnchor = new WAnchor(mDownloadFileResource);
    mDownloadAnchor->setTarget(TargetThisWindow);
    mDownloadButton = new WPushButton("Download", mDownloadAnchor);

    WGridLayout *layout = new WGridLayout();

    // Create the patient info box
    WGridLayout *fileInfoLayout = new WGridLayout();
    fileInfoLayout->addWidget(new WLabel("Name:"), 0, 0);
    fileInfoLayout->addWidget(mFileName = new WLabel(""), 0, 1);
    fileInfoLayout->addWidget(new WLabel("Directory:"), 1, 0);
    fileInfoLayout->addWidget(mFileDir = new WLabel(""), 1, 1);
    fileInfoLayout->addWidget(new WLabel("Size:"), 2, 0);
    fileInfoLayout->addWidget(mFileSize = new WLabel(""), 2, 1);
    fileInfoLayout->setColumnStretch(1, 1);

    WHBoxLayout *hbox = new WHBoxLayout();
    hbox->addStretch(10);
    hbox->addWidget(mDownloadAnchor);
    hbox->addStretch(10);

    layout->addLayout(fileInfoLayout, 0, 0);
    layout->addLayout(hbox, 1, 0);
    layout->addWidget(mPreviewStack, 2, 0, Wt::AlignCenter);
    layout->setRowStretch(2, 1);
    setLayout(layout);

    resetAll();
}

///
//  Destructor
//
FilePreviewBox::~FilePreviewBox()
{
    delete mDownloadFileResource;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Reset all widgets to the default state
//
void FilePreviewBox::resetAll()
{
    mFileName->setText("");
    mFileDir->setText("");
    mFileSize->setText("");
    mPreviewStack->hide();
}

///
//  Set the file
//
void FilePreviewBox::setFilePath(std::string filePathStr)
{
    try
    {
        path filePath = path(filePathStr);

        mFileName->setText(filePath.leaf());
        mFileDir->setText(filePath.branch_path().string());
        mDownloadFileResource->setFileName(filePathStr);
        mDownloadFileResource->suggestFileName(filePath.leaf());

        mFileSize->setText(WString("{1} Bytes").arg((int)file_size(filePath)));

        if (fileMatchesExpression(filePathStr, getConfigXMLPtr()->getImageFilePattern()))
        {
            if (mImageResource == NULL)
            {
                mImageResource = new WFileResource("image/" + filePath.extension(), filePathStr);
            }
            else
            {
                mImageResource->setFileName(filePathStr);
            }
            mImagePreview->setResource(mImageResource);
            mImagePreview->setMaximumSize(650, 532);
            mPreviewStack->setCurrentIndex(0);
            mPreviewStack->show();
        }
        else if(fileMatchesExpression(filePathStr, getConfigXMLPtr()->getTextFilePattern()))
        {
            std::ifstream inFile(filePathStr.c_str(), ios::in);
            if (inFile.is_open())
            {
                ostringstream oss;
                oss << inFile.rdbuf();

                mTextPreview->setText(oss.str().c_str());
                mTextPreview->setMinimumSize(650, 532);

                mPreviewStack->setCurrentIndex(1);
                mPreviewStack->show();
            }
            else
            {
                mPreviewStack->hide();
            }
        }
        else
        {
            mPreviewStack->hide();
        }
    }
    catch (...)
    {
        // If any exceptions occur, the file may no longer exist, so reset box
        resetAll();
    }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Check whether file matches regular expression
//
bool FilePreviewBox::fileMatchesExpression(const std::string& filePathStr, const std::string &filePatternExpr) const
{
    regex regEx;
    try
    {
        regEx = regex(filePatternExpr);
    }
    catch(...)
    {
        WApplication::instance()->log("error") << "Invalid regular expression '" << filePatternExpr << "' in config XML file";
        return false;
    }

    boost::smatch what;
    return (boost::regex_match( filePathStr, what, regEx ));
}


