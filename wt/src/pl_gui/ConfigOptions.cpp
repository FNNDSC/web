//
//
//  Description:
//      Implementation of singleton class that loads configuration options from a file
//      using boost::program_options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include "ConfigOptions.h"
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <iostream>
#include <fstream>
#include <string>

///
//  Namespaces
//
using namespace Wt;
using namespace std;
using namespace boost::program_options;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ConfigOptions::ConfigOptions() :
    // Default options
    mDicomDir("/chb/osx1927/1/users/dicom/files/")
{
    mOptionDesc = new options_description("Allowable options");
    mOptionDesc->add_options()
        ("dicomDir",        value<string>(), "Base DICOM directory")
        ("outDir",          value<string>(), "Base output directory")
        ("analysisDir",     value<string>(), "Base analysis directory")
        ("outGradientDir",  value<string>(), "Gradient output directory")
        ("projectDir",      value<string>(), "Project directory")
        ("scriptDir",       value<string>(), "Base scripts directory")
        ("clusterDir",      value<string>(), "Cluster schedule directory")
        ("clusterHeadNode", value<string>(), "Cluster head node")
        ("clusterType",     value<string>(), "Cluster type")
        ("packageDir",      value<string>(), "Package directory")
        ("configXML",       value<string>(), "Config XML file")
        ("cpuUsageFile",    value<string>(), "CPU Usage File")
        ("topLogFile",      value<string>(), "Top Log file")
        ("remoteMatLab",    value<string>(), "Remote MatLAB hostname")
        ("mridFilterFile",  value<string>(), "MRI Filter file")
        ("permissionsFile", value<string>(), "Permissions file (XML) for users/groups")
        ("adminGroup",      value<string>(), "Admin group")
        ("anonCertificate", value<string>(), "Anonymizing certificate")
        ("jobIDPrefix",     value<string>(), "Job ID Prefix")
        ;
}

///
//  Destructor
//
ConfigOptions::~ConfigOptions()
{
    delete mOptionDesc;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Load configuration from file
//
bool ConfigOptions::LoadFromFile(const std::string& configPath)
{
    if (mOptionDesc == NULL)
    {
        WApplication::instance()->log("error") << "Missing options description.";
        return false;
    }

    fstream configFile(configPath.c_str(), ios::in);
    if(!configFile.is_open())
    {
        WApplication::instance()->log("error") << "Opening file for reading: " << configPath;
        return false;
    }

    try
    {
        variables_map vm;
        store(parse_config_file(configFile, *mOptionDesc), vm);
        notify(vm);

        if (vm.count("dicomDir"))
        {
            mDicomDir = vm["dicomDir"].as<string>();
        }

        if (vm.count("outDir"))
        {
            mOutDir = vm["outDir"].as<string>();
        }

        if (vm.count("analysisDir"))
        {
            mAnalysisDir = vm["analysisDir"].as<string>();
        }

        if (vm.count("outGradientDir"))
        {
            mOutGradientDir = vm["outGradientDir"].as<string>();
        }

        if (vm.count("projectDir"))
        {
            mProjectDir = vm["projectDir"].as<string>();
        }

        if (vm.count("scriptDir"))
        {
            mScriptDir = vm["scriptDir"].as<string>();
        }

        if (vm.count("clusterDir"))
        {
            mClusterDir = vm["clusterDir"].as<string>();
        }

        if (vm.count("clusterHeadNode"))
        {
            mClusterHeadNode = vm["clusterHeadNode"].as<string>();
        }

        if (vm.count("clusterType"))
        {
            mClusterType = vm["clusterType"].as<string>();
        }

        if (vm.count("packageDir"))
        {
            mPackageDir = vm["packageDir"].as<string>();
        }

        if (vm.count("configXML"))
        {
            mConfigXML = vm["configXML"].as<string>();
        }

        if (vm.count("cpuUsageFile"))
        {
            mCPUUsageFile = vm["cpuUsageFile"].as<string>();
        }

        if (vm.count("topLogFile"))
        {
            mTopLogFile = vm["topLogFile"].as<string>();
        }

        if (vm.count("remoteMatLab"))
        {
            mRemoteMatLab = vm["remoteMatLab"].as<string>();
        }

        if (vm.count("mridFilterFile"))
        {
            mMRIDFilterFile = vm["mridFilterFile"].as<string>();
        }

        if (vm.count("permissionsFile"))
        {
            mPermissionsFile = vm["permissionsFile"].as<string>();
        }

        if (vm.count("adminGroup"))
        {
            mAdminGroup = vm["adminGroup"].as<string>();
        }

        if (vm.count("anonCertificate"))
        {
            mAnonCertificate = vm["anonCertificate"].as<string>();
        }

        if (vm.count("jobIDPrefix"))
        {
            mJobIDPrefix = vm["jobIDPrefix"].as<string>();
        }

        WApplication::instance()->log("info") << "[DICOM Dir:] " << mDicomDir;
        WApplication::instance()->log("info") << "[Output Dir:] " << mOutDir;
        WApplication::instance()->log("info") << "[Analysis Dir:] " << mAnalysisDir;
        WApplication::instance()->log("info") << "[Output Gradient Dir:] " << mOutGradientDir;
        WApplication::instance()->log("info") << "[Project Dir:] " << mProjectDir;
        WApplication::instance()->log("info") << "[Script Dir:] " << mScriptDir;
        WApplication::instance()->log("info") << "[Cluster Dir:] " << mClusterDir;
        WApplication::instance()->log("info") << "[Cluster Type:] " << mClusterType;
        WApplication::instance()->log("info") << "[Cluster Head Node:] " << mClusterHeadNode;
        WApplication::instance()->log("info") << "[Package Dir:] " << mPackageDir;
        WApplication::instance()->log("info") << "[Results Config XML:] " << mConfigXML;
        WApplication::instance()->log("info") << "[CPU Usage file:] " << mCPUUsageFile;
        WApplication::instance()->log("info") << "[Top log file:] " << mTopLogFile;
        WApplication::instance()->log("info") << "[Remote MatLAB:] " << mRemoteMatLab;
        WApplication::instance()->log("info") << "[MRID Filter File:] " << mMRIDFilterFile;
        WApplication::instance()->log("info") << "[Permissions File:] " << mPermissionsFile;
        WApplication::instance()->log("info") << "[Admin Group:]" << mAdminGroup;
        WApplication::instance()->log("info") << "[Anonymizing Certificate:]" << mAnonCertificate;
        WApplication::instance()->log("info") << "[Job ID Prefix:]" << mJobIDPrefix;

        configFile.close();
    }
    catch(boost::program_options::error& e)
    {
        WApplication::instance()->log("error") << "Error parsing config file: " << configPath;
        WApplication::instance()->log("error") << e.what();
        return false;
    }
    catch(...)
    {
        WApplication::instance()->log("error") << "Error parsing config file: " << configPath;
        return false;
    }

    return true;
}



