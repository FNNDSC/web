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
#include <Wt/WScrollArea>
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
    mTextPreview = new WText();
    WScrollArea *scrollArea = new WScrollArea();
    scrollArea->setMaximumSize(1000, 820);
    scrollArea->setMinimumSize(500, WLength::Auto);
    scrollArea->setStyleClass("textpreviewscroll");

    scrollArea->setWidget(mTextPreview);
    mTextPreview->setStyleClass("textpreviewdiv");
    mTextPreview->setTextFormat(PlainText);
    mTextPreview->decorationStyle().font().setFamily(WFont::Monospace);

    mPreviewStack = new WStackedWidget();
    mPreviewStack->addWidget(mImagePreview);
    mPreviewStack->addWidget(scrollArea);


    // Create an anchor that references a URL
    mDownloadFileResource = new WFileResource("application/octet-stream", "");
    mDownloadAnchor = new WAnchor(mDownloadFileResource);
    mDownloadAnchor->setTarget(TargetThisWindow);
    mDownloadButton = new WPushButton("Download", mDownloadAnchor);

    mViewerAnchor = new WAnchor("http://durban.tch.harvard.edu/webgl-test/index.html");
    mViewerAnchor->setTarget(TargetNewWindow);
    mViewerButton = new WPushButton("Preview", mViewerAnchor);


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
    hbox->addWidget(mViewerAnchor);
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
    if (mImageResource != NULL)
    {
        delete mImageResource;
        mImageResource = NULL;
    }
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
        path fileDir = filePath.branch_path();

        mFileName->setText(filePath.leaf().string());
        mFileDir->setText(fileDir.string());
        mDownloadFileResource->setFileName(filePathStr);
        mDownloadFileResource->suggestFileName(filePath.leaf().string());

        mFileSize->setText(WString("{1} Bytes").arg((int)file_size(filePath)));

        if (fileMatchesExpression(filePathStr, getConfigXMLPtr()->getImageFilePattern()))
        {
            if (mImageResource == NULL)
            {
                mImageResource = new WFileResource("image/" + filePath.extension().string(), filePathStr);
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

                if (oss.str().size() < WApplication::instance()->maximumRequestSize())
                {
                    mTextPreview->setText(oss.str().c_str());
                }
                else
                {
                    mTextPreview->setText("The file is too large to display, please click Download to view it.");
                }

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
            bool previewFound = false;
            const std::list<ConfigXML::PreviewPatternNode> &previewPatternList = getConfigXMLPtr()->getPreviewPatterns();
            std::list<ConfigXML::PreviewPatternNode>::const_iterator iter = previewPatternList.begin();

            while (iter != previewPatternList.end() && !previewFound)
            {
                if (fileMatchesExpression(filePathStr, (*iter).mExpression))
                {
                    // See if a file matching the preview expression exists
                    for(directory_iterator dirIter(fileDir); dirIter != directory_iterator(); ++dirIter)
                    {
                        const string& fileName = dirIter->path().filename().string();

                        // Skip if no match
                        if( !fileMatchesExpression( fileName, (*iter).mPreviewExpression ) )
                            continue;

                        if (!is_directory(dirIter->path()))
                        {
                            if (mImageResource == NULL)
                            {
                                mImageResource = new WFileResource("image/" + dirIter->path().extension().string(), dirIter->path().string());
                            }
                            else
                            {
                                mImageResource->setFileName(dirIter->path().string());
                            }
                            mImagePreview->setResource(mImageResource);
                            mImagePreview->setMaximumSize(1000, 820);
                            mPreviewStack->setCurrentIndex(0);
                            mPreviewStack->show();
                            previewFound = true;
                            break;
                        }
                    }
                }

                iter++;
            }

            // No preview available
            if (!previewFound)
            {
                mPreviewStack->hide();
            }

            bool viewerFound = false;

            const std::list<ConfigXML::ViewerPatternNode> &viewerPatternList = getConfigXMLPtr()->getViewerPatterns();
            std::list<ConfigXML::ViewerPatternNode>::const_iterator viewerIter = viewerPatternList.begin();

            while (viewerIter != viewerPatternList.end() && !viewerFound)
            {
                if (fileMatchesExpression(filePathStr, (*viewerIter).mExpression))
                {

                    mViewerAnchor->setRef((*viewerIter).mViewerURL + filePathStr);
                    mViewerAnchor->show();
                    viewerFound = true;
                    break;
                }

                viewerIter++;
            }

            if (!viewerFound)
            {
                mViewerAnchor->hide();
            }
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


;
