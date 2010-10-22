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
    /// Load configuration from file
    /// \param configFile Full path to configuration file
    /// \return Whether loading was successful
    ///
    bool LoadFromFile(const std::string& configFile);

    // Accessors
    const std::string& GetDicomDir()            const { return mDicomDir; }
    const std::string& GetOutDir()              const { return mOutDir; }
    const std::string& GetAnalysisDir()         const { return mAnalysisDir; }
    const std::string& GetOutGradientDir()      const { return mOutGradientDir; }
    const std::string& GetProjectDir()          const { return mProjectDir; }
    const std::string& GetScriptDir()           const { return mScriptDir; }
    const std::string& GetClusterName()         const { return mClusterName; }
    const std::string& GetClusterHeadNode()     const { return mClusterHeadNode; }
    const std::string& GetClusterType()         const { return mClusterType; }
    const std::string& GetPackageDir()          const { return mPackageDir; }
    const std::string& GetConfigXML()           const { return mConfigXML; }
    const std::string& GetCPUUsageFile()        const { return mCPUUsageFile; }
    const std::string& GetTopLogFile()          const { return mTopLogFile; }
    const std::string& GetRemoteMatLab()        const { return mRemoteMatLab; }
    const std::string& GetMRIDFilterFile()      const { return mMRIDFilterFile; }
    const std::string& GetPermissionsFile()     const { return mPermissionsFile; }
    const std::string& GetAdminGroup()          const { return mAdminGroup; }
    const std::string& GetAnonCertificate()     const { return mAnonCertificate; }
    const std::string& GetJobIDPrefix()         const { return mJobIDPrefix; }

private:

    // Allowable options
    boost::program_options::options_description *mOptionDesc;

    /// Dicom directory
    std::string mDicomDir;

    /// Script dir
    std::string mScriptDir;

    /// Output directory
    std::string mOutDir;

    /// Analysis directory
    std::string mAnalysisDir;

    /// Output gradient directory
    std::string mOutGradientDir;

    /// Project base directory
    std::string mProjectDir;

    /// Cluster name
    std::string mClusterName;

    /// Cluster head node
    std::string mClusterHeadNode;

    /// Cluster type
    std::string mClusterType;

    /// Package dir
    std::string mPackageDir;

    /// Results configuration XML file
    std::string mConfigXML;

    /// CPU Usage file
    std::string mCPUUsageFile;

    /// Top log file
    std::string mTopLogFile;

    /// Remote MatLAB host
    std::string mRemoteMatLab;

    /// MRI Filter file
    std::string mMRIDFilterFile;

    /// Permissions File (XML)
    std::string mPermissionsFile;

    /// Admin permissions group
    std::string mAdminGroup;

    /// Anonymizing certificate
    std::string mAnonCertificate;

    /// Job ID Prefix
    std::string mJobIDPrefix;
};

#endif // CONFIGOPTIONS_H
