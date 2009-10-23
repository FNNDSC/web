//
//
//  Description:
//      Definition of singleton class that loads configuration options from a file
//      using boost::program_options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef CONFIGOPTIONS_H
#define CONFIGOPTIONS_H

#define BOOST_PROCESS_HEADER_ONLY
#include <boost/program_options.hpp>
#include <string>

///
/// \class ConfigOptions
/// \brief Singleton object for accessing configuration options
///
class ConfigOptions
{
public:
    ///
    /// Constructor
    ///
    ConfigOptions();

    ///
    /// Destructor
    ///
    virtual ~ConfigOptions();

    ///
    /// Get a pointer to the singleton instance
    ///
    static ConfigOptions* GetPtr();

    ///
    /// Load configuration from file
    /// \param configFile Full path to configuration file
    /// \return Whether loading was successful
    ///
    bool LoadFromFile(const std::string& configFile);


    // Accessors
    const std::string& GetDicomDir()        const { return mDicomDir; }
    const std::string& GetOutDir()          const { return mOutDir; }
    const std::string& GetSeriesListTract() const { return mSeriesListTract; }
    const std::string& GetSeriesListFS()    const { return mSeriesListFS; }
    const std::string& GetScriptDir()       const { return mScriptDir; }
    const std::string& GetClusterName()     const { return mClusterName; }
    const std::string& GetPackageDir()      const { return mPackageDir; }
private:

    // Singleton instance
    static ConfigOptions *mInstance;

    // Allowable options
    boost::program_options::options_description *mOptionDesc;

    /// Dicom directory
    std::string mDicomDir;

    /// Series list (tract)
    std::string mSeriesListTract;

    /// Series list (FS)
    std::string mSeriesListFS;

    /// Script dir
    std::string mScriptDir;

    /// Output directory
    std::string mOutDir;

    /// Cluster name
    std::string mClusterName;

    /// Package dir
    std::string mPackageDir;
};

#endif // CONFIGOPTIONS_H
