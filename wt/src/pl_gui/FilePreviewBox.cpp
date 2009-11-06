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
#include "FilePreviewBox.h"
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
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
#include <Wt/WFileResource>
#include <Wt/WAnchor>
#include <Wt/WFileResource>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
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
    layout->addWidget(mImagePreview, 2, 0, Wt::AlignCenter);
    layout->setRowStretch(2, 1);
    setLayout(layout);

    // Make connections
    mDownloadButton->clicked().connect(SLOT(this, FilePreviewBox::downloadClicked));

    resetAll();
}

///
//  Destructor
//
FilePreviewBox::~FilePreviewBox()
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
void FilePreviewBox::resetAll()
{
    mFileName->setText("");
    mFileDir->setText("");
    mFileSize->setText("");
    mImagePreview->hide();
}

///
//  Set the file
//
void FilePreviewBox::setFilePath(std::string filePathStr)
{
    path filePath = path(filePathStr);

    mFileName->setText(filePath.leaf());
    mFileDir->setText(filePath.branch_path().string());
    mDownloadFileResource->setFileName(filePathStr);
    mDownloadFileResource->suggestFileName(filePath.leaf());

    mFileSize->setText(WString("{1} Bytes").arg((int)file_size(filePath)));

    if (!imageExtension(filePathStr).empty())
    {
        if (mImageResource == NULL)
        {
            mImageResource = new WFileResource("mime/" + imageExtension(filePathStr),
                                               filePathStr);
        }
        else
        {
            mImageResource->setFileName(filePathStr);
        }
        mImagePreview->setResource(mImageResource);
        mImagePreview->setMaximumSize(650, 532);
        mImagePreview->show();
    }
    else
    {
        mImagePreview->hide();
    }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Private Members
//
//

///
//  Return the image extension of the file (if it is an image file)
//
std::string FilePreviewBox::imageExtension(const std::string& fileName)
{
  static const char *imageExtensions[] =
  {
    ".png", ".gif", ".jpg", "jpeg", ".ico", 0
  };

  path p(fileName);
  std::string extension = boost::filesystem::extension(p);

  for (const char **s = imageExtensions; *s != 0; ++s)
    if (*s == extension)
      return extension.substr(1);

  return std::string();
}

///
//   Handle download button clicked [slot]
//
void FilePreviewBox::downloadClicked()
{

}

