//
//
//  Description:
//      Definition of a resource object that can archive a folder and transmit
//      it via HTTP
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef ARCHIVEFILERESOURCE_H
#define ARCHIVEFILERESOURCE_H

#include <Wt/WResource>
#include <string>

using namespace Wt;

///
/// \class ArchiveFileResource
/// \brief Provides a file resource which can archive a folder
///
class ArchiveFileResource : public WResource
{
public:
    ///
    /// Constructor
    ///
    ArchiveFileResource(const std::string& directoryPath, WObject *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ArchiveFileResource();

    ///
    /// Set directory path
    ///
    void setDirPath(const std::string& dirPath) {   mDirPath = dirPath; }

protected:

    ///
    /// Handle HTTP request.  This callback is made when the file is requested
    ///
    virtual void handleRequest(const Http::Request& request, Http::Response& response);

private:

    /// Directory path
    std::string mDirPath;
};

#endif // ARCHIVEFILERESOURCE_H

