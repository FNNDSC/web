//
//
//  Description:
//      Definition of file preview box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef FILEPREVIEWBOX_H
#define FILEPREVIEWBOX_H

#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include <vector>
#include <string>
#include "GlobalEnums.h"

namespace Wt
{
    class WAnchor;
    class WLabel;
    class WImage;
    class WFileResource;
    class WPushButton;
}

using namespace Wt;

///
/// \class FilePreviewBox
/// \brief Provides a group box that displays a preview of a file
///
class FilePreviewBox : public WGroupBox
{
public:

    ///
    /// Constructor
    ///
    FilePreviewBox(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~FilePreviewBox();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// \brief Set the file to preview
    ///
    void setFilePath(std::string filePath);

private:

    ///
    /// Handle download button clicked [slot]
    ///
    void downloadClicked();

    ///
    ///  Return the image extension of the file (if it is an image file)
    ///
    std::string imageExtension(const std::string& fileName);

private:

    /// File Name
    WLabel *mFileName;

    /// File Directory
    WLabel *mFileDir;

    /// File Size
    WLabel *mFileSize;

    /// Image preview (for image files)
    WImage *mImagePreview;

    /// Image resource
    WFileResource *mImageResource;

    /// Download file resource
    WFileResource *mDownloadFileResource;

    /// Download button
    WPushButton *mDownloadButton;

    /// Download anchor
    WAnchor *mDownloadAnchor;
};

#endif // FILEPREVIEWBOX_H

