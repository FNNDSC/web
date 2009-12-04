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
ArchiveFileResource::ArchiveFileResource(const std::string& directoryPath, WObject *parent) :
    WResource(parent),
    mDirPath(directoryPath)
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
    char *tmpName = strdup("/tmp/archive_XXXXXX");

    if (mkstemp(tmpName) == -1)
    {
        WApplication::instance()->log("error") << "Error creating file on server: " << tmpName;
        return;
    }

    path dirPath = path(mDirPath);
    string cmdToExecute;
    cmdToExecute = string("tar cvzf ") + tmpName + " -C " + dirPath.branch_path().string() + " " + dirPath.leaf();

    context ctx;
    child c = launch_shell(cmdToExecute.c_str(), ctx);
    boost::processes::status s = c.wait();

    ifstream inFile(tmpName, ios::in);
    if (inFile.is_open())
    {
        ostringstream oss;
        oss << inFile.rdbuf();

        response.setMimeType("application/x-compressed");
        response.out() << oss.str();
        inFile.close();

        // Remove the temp file
        cmdToExecute = string("rm -rf ") + tmpName;
        context ctx;
        child c = launch_shell(cmdToExecute.c_str(), ctx);
        boost::processes::status s = c.wait();
    }
}
