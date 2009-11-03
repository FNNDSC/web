//
//
//  Description:
//      Definition of base class for File browser widget.  This widget provides
//      common functionality that can be inherited to browse a tree of files.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <Wt/WContainerWidget>
#include <Wt/WTreeView>
#include <Wt/WStandardItemModel>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>

using namespace Wt;


///
/// \class FileBrowser
/// \brief Base class for a file browser widget
///
class FileBrowser : public WContainerWidget
{
public:

    ///
    /// Constructor
    ///
    FileBrowser(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~FileBrowser();


protected:

    ///
    /// Add an entry to the browser
    /// \param rootDir Whether the item is at the root of the tree
    /// \param entryDepth Depth of the entry in the tree
    /// \param baseDir Base directory of entry
    /// \param baseName Base name of file (or display name for folder)
    /// \param index This is the index of the item that can be used by the subclass
    ///              to pass an index into its own data structure for the entry
    /// \return True if item was added succesfully, false otherwise
    //
    bool addEntry(bool rootDir, int entryDepth,
                  const std::string &baseDir, const std::string &baseName,
                  int index);

    ///
    //  Create a file entry
    //
    WStandardItem* createEntry(const std::string& baseName, int index);

protected:

    /// Tree view
    WTreeView *mTreeView;

    /// Model
    WStandardItemModel *mModel;
};

#endif // LOGFILEBROWSER_H

