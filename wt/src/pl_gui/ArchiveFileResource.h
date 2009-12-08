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
    ArchiveFileResource(WObject *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~ArchiveFileResource();

    ///
    /// Set data pointer.  Note this pointer is owned by the caller and will not
    /// be freed or copied by this class.
    ///
    void setData(const unsigned char *data, int size);

protected:

    ///
    /// Handle HTTP request.  This callback is made when the file is requested
    ///
    virtual void handleRequest(const Http::Request& request, Http::Response& response);

private:

    /// Pointer to data
    const unsigned char *mData;

    /// Data size
    int mSize;
};

#endif // ARCHIVEFILERESOURCE_H

