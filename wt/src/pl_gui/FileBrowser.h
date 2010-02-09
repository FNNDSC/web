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
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <string>
#include <vector>

///
//  Classes
//
class QtFileSystemWatcher;
namespace Wt
{
    class WApplication;
};


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

    ///
    /// Reset to default state
    ///
    virtual void resetAll();

    ///
    /// Create Qt objects
    ///
    void createQt();

    ///
    /// Destroy Qt objects()
    ///
    void destroyQt();

    ///
    ///  Do an update on the browser
    ///
    void doUpdate();

    ///
    /// Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Handle async changes to directory
    ///
    virtual void directoryChanged() = 0;


protected:

    ///
    /// Add directory to be watched
    ///
    void addWatchPath(const std::string& path);

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

private:

    ///
    ///  Update thread for checking directory changes
    ///
    void updateBrowser();

protected:

    /// Tree view
    WTreeView *mTreeView;

    /// Model
    WStandardItemModel *mModel;

    /// File system watcher (using Qt)
    QtFileSystemWatcher *mQtFileSystemWatcher;

    /// Thread for background processing
    boost::thread  *mThread;

    /// Stop updating thread
    volatile bool mStopUpdateThread;

    /// Do update
    volatile bool mUpdateBrowser;

    /// Wait condition for do update
    boost::condition mDoUpdateCondition;

    /// Mutex
    boost::mutex mDoUpdateMutex;

    /// Application instance
    WApplication *mApp;

};

#endif // FILEBROWSER_H

