//
//
//  Description:
//      This is Qt wrapper object that uses the QFileSystemWatcher to
//      watch for changes to the file system.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "FileBrowser.h"
#include "QtFileSystemWatcher.h"
#include <QtDebug>


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
QtFileSystemWatcher::QtFileSystemWatcher(FileBrowser *fileBrowser, QObject *parent)
  : QFileSystemWatcher(parent),
    mFileBrowser(fileBrowser)
{
    QObject::connect(this, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(notifyDirectoryChanged(const QString&)));

    // This is needed to force Qt to use a polling engine rather than inotify.
    // The problem with inotify is that it only picks up events for an NFS
    // share if they are made locally.  I found this out the hard way.  This
    // solution was suggested on one of the Qt forums.
    setObjectName(QLatin1String("_qt_autotest_force_engine_poller"));
}

///
//  Destructor
//
QtFileSystemWatcher::~QtFileSystemWatcher()
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Slots
//
//

///
//  Handle notification of directory changes [slot]
//
void QtFileSystemWatcher::notifyDirectoryChanged(const QString& path)
{
    mFileBrowser->doUpdate();
}
