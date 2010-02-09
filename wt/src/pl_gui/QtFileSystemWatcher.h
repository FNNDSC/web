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
#ifndef QT_FILE_SYSTEM_WATCHER_H
#define QT_FILE_SYSTEM_WATCHER_H

#ifdef SLOT
# undef SLOT
# undef signals
# undef slots
#endif

#include <QFileSystemWatcher>

///
//  Classes
//
class FileBrowser;

///
/// \class QtFileSystemWatcher
/// \brief Qt wrapper object that monitors file system changes using QFileSystemWatcher
///
class QtFileSystemWatcher : public QFileSystemWatcher
{
    Q_OBJECT;

public:

    ///
    /// Constructor
    ///
    QtFileSystemWatcher(FileBrowser *fileBrowser, QObject *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~QtFileSystemWatcher();

signals:

public slots:

    ///
    /// Handle notification of directory changes [slot]
    ///
    void notifyDirectoryChanged(const QString& );

protected:

    /// File browser object
    FileBrowser *mFileBrowser;

};

#endif // QT_FILE_SYSTEM_WATCHER_H
