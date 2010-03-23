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
#include "PipelineApp.h"
#include "QtFileSystemWatcherThread.h"
#include "QtFileSystemWatcher.h"
#include <Wt/WLogger>
#include <QStringList>

///
//  Namespaces
//

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
QtFileSystemWatcherThread::QtFileSystemWatcherThread(QtFileSystemWatcherListener *listener,
                                                     WApplication *app) :
    mListener(listener),
    mApp(app),
    mQtFileSystemWatcher(NULL),
    mStopUpdateThread(false),
    mUpdate(false)
{

    mThread = new boost::thread(boost::bind(&QtFileSystemWatcherThread::updateThread, this));

}

///
//  Destructor
//
QtFileSystemWatcherThread::~QtFileSystemWatcherThread()
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Callback from QtFileSystemWatcher when file/directory is updated
//
void QtFileSystemWatcherThread::doUpdate(std::string path)
{
    boost::mutex::scoped_lock lock(mDoUpdateMutex);
    mUpdatePath = path;
    mUpdate = true;
    mDoUpdateCondition.notify_one();
}

///
//  Finalize the widget (pre-destruction)
//
void QtFileSystemWatcherThread::finalize()
{
    if (mThread != NULL)
    {
        mStopUpdateThread = true;
        mUpdate = false;
        mDoUpdateCondition.notify_one();
        mThread->join();
        delete mThread;
        mThread = NULL;
    }
}

///
//  Create Qt objects
//
void QtFileSystemWatcherThread::createQt()
{
    mQtFileSystemWatcher = new QtFileSystemWatcher(this);
}

///
//  Destroy Qt objects
//
void QtFileSystemWatcherThread::destroyQt()
{
    delete mQtFileSystemWatcher;
}

///
//  Reset to default state
//
void QtFileSystemWatcherThread::resetAll()
{
    if (mQtFileSystemWatcher != NULL)
    {
        QStringList dirList = mQtFileSystemWatcher->directories();
        QStringList fileList = mQtFileSystemWatcher->files();

        if (dirList.size() > 0)
        {
            mQtFileSystemWatcher->removePaths(dirList);
        }

        if (fileList.size() > 0 )
        {
            mQtFileSystemWatcher->removePaths(fileList);
        }
    }
}

///
// Add path to watch
///
void QtFileSystemWatcherThread::addPath(const std::string &path)
{
    mQtFileSystemWatcher->addPath(QString(path.c_str()));
}

///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Function that executes the update thread
//
void QtFileSystemWatcherThread::updateThread()
{
    boost::mutex::scoped_lock lock(mDoUpdateMutex);

    while (!mStopUpdateThread)
    {
        mDoUpdateCondition.wait(lock);

        if (mUpdate)
        {

            // First, take the lock to safely manipulate the UI outside of the
            // normal event loop, by having exclusive access to the session.
            WApplication::UpdateLock lock = mApp->getUpdateLock();

            mApp->log("debug") << "UPDATE: " << mUpdatePath;

            mListener->fileUpdated(mUpdatePath);

            mApp->triggerUpdate();
        }
    }
}
