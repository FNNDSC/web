//
//
//  Description:
//      A generic thread to interact with the QtFileSystemWatcher and provide notification
//      back to the GUI.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef QT_FILE_SYSTEM_WATCHER_THREAD_H
#define QT_FILE_SYSTEM_WATCHER_THREAD_H

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <string>

class QtFileSystemWatcher;
namespace Wt
{
    class WApplication;
}

///
//  Classes
//
class FileBrowser;

///
/// \class QtFileSystemWatcherObject
/// \brief Base class for object which is going to be a listener to file system updates
///
class QtFileSystemWatcherListener
{
public:
    ///
    /// Constructor
    ///
    QtFileSystemWatcherListener()   {   ;   }

    ///
    /// Destructor
    ///
    virtual ~QtFileSystemWatcherListener()  {   ;   }

    ///
    /// Method which gets called with a path name whenever there is a file updated, must be
    /// implemented by the listener
    ///
    virtual void fileUpdated(std::string path) = 0;
};

///
/// \class QtFileSystemWatcherThread
/// \brief A generic thread to interact with the QtFileSystemWatcher
///
class QtFileSystemWatcherThread
{
public:

    ///
    /// Constructor
    ///
    QtFileSystemWatcherThread(QtFileSystemWatcherListener *listener,
                              Wt::WApplication *app);

    ///
    /// Destructor
    ///
    virtual ~QtFileSystemWatcherThread();

    ///
    ///  Callback from QtFileSystemWatcher when file/directory is updated
    ///
    void doUpdate(std::string path);

    ///
    ///  Finalize the widget (pre-destruction)
    ///
    void finalize();

    ///
    /// Create Qt objects
    ///
    void createQt();

    ///
    /// Destroy Qt objects
    ///
    void destroyQt();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Add path to watch
    ///
    void addPath(const std::string &path);


protected:

    ///
    /// Function that executes the update thread
    ///
    void updateThread();

protected:

    /// Qt File system watcher
    QtFileSystemWatcher *mQtFileSystemWatcher;

    /// Listener object
    QtFileSystemWatcherListener *mListener;

    /// Thread for background processing
    boost::thread  *mThread;

    /// Stop updating thread
    volatile bool mStopUpdateThread;

    /// Do update
    volatile bool mUpdate;

    /// Wait condition for do update
    boost::condition mDoUpdateCondition;

    /// Mutex
    boost::mutex mDoUpdateMutex;

    /// Update path
    std::string mUpdatePath;

    /// Pointer to application instance
    Wt::WApplication *mApp;

};

#endif // QT_FILE_SYSTEM_WATCHER_H
