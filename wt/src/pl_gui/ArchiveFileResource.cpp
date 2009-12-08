//
//
//  Description:
//      Implementation of a resource object that can archive a folder and transmit
//      it via HTTP
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "ConfigOptions.h"
#include "ArchiveFileResource.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/Http/Response>
#include <signal.h>
#include <boost/process/process.hpp>
#include <boost/process/child.hpp>
#include <boost/process/launch_shell.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>



///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::filesystem;
using namespace boost::processes;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//


///
//  Constructor
//
ArchiveFileResource::ArchiveFileResource(WObject *parent) :
    WResource(parent),
    mData(NULL),
    mSize(0)
{
}


///
//  Destructor
//
ArchiveFileResource::~ArchiveFileResource()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Set data pointer.  Note this pointer is owned by the caller and will not
//  be freed or copied by this class.
//
void ArchiveFileResource::setData(const unsigned char *data, int size)
{
    mData = data;
    mSize = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Handle HTTP request.  This callback is made when the file is requested
//
void ArchiveFileResource::handleRequest(const Http::Request& request,
                                        Http::Response& response)
{
    if (mData != NULL && mSize > 0)
    {
        response.setMimeType("application/x-compressed");
        response.out().write((const char*)mData, mSize);
    }
}
