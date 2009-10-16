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

    const std::string& GetDicomDir()        const { return mDicomDir; }
    const std::string& GetSeriesListTract() const { return mSeriesListTract; }
    const std::string& GetSeriesListFS()    const { return mSeriesListFS; }
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
};

#endif // CONFIGOPTIONS_H
