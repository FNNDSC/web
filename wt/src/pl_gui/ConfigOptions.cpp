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
#include <iostream>
#include <fstream>
#include <string>

///
//  Namespaces
//
using namespace std;
using namespace boost::program_options;

///
//  Static Members
//
ConfigOptions* ConfigOptions::mInstance = NULL;

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
        ("dicomDir", value<string>(), "Base DICOM directory")
        ;
}

///
//  Destructor
//
ConfigOptions::~ConfigOptions()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Get a reference to the singleton instance
//
ConfigOptions* ConfigOptions::GetPtr()
{
    if (mInstance == NULL)
    {
        mInstance = new ConfigOptions();
    }

    return mInstance;
}

///
//  Load configuration from file
//
bool ConfigOptions::LoadFromFile(const std::string& configPath)
{
    if (mOptionDesc == NULL)
    {
        cerr << "Missing options description." << endl;
        return false;
    }

    fstream configFile(configPath.c_str());
    if(!configFile.is_open())
    {
        cerr << "ERROR: Opening file for reading: " << configPath << endl;
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

        configFile.close();
    }
    catch(...)
    {
        cerr << "Error parsing config file: " << configPath << endl;
        return false;
    }

    return true;
}



