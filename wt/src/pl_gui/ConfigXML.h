//
//
//  Description:
//      Definition of class that loads an XML file that configures
//      how the pipeline results are accumulated.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef CONFIGXML_H
#define CONFIGXML_H

#define BOOST_PROCESS_HEADER_ONLY
#include <Wt/WStandardItemModel>
#include <boost/program_options.hpp>
#include <string>
#include <map>
#include <list>
#include <mxml.h>


using namespace Wt;

///
/// \class ConfigXML
/// \brief Object for accessing configuration of the per-pipeline results
///
class ConfigXML
{
public:
    /// Results entry
    typedef struct
    {
        /// Expression for matching files
        std::string mExpression;

        /// Whether to match files or directories
        bool mDirectory;

        /// Whether to recurse the folder
        bool mRecurse;

    } FilePatternNode;

    ///
    /// Constructor
    ///
    ConfigXML();

    ///
    /// Destructor
    ///
    virtual ~ConfigXML();

    ///
    /// Load configuration from XML file
    /// \param configFile Full path to configuration XML file
    /// \return Whether loading was successful
    ///
    bool loadFromFile(const std::string& configFile);


    ///
    /// Get the results entry list for a given pipeline by name
    /// \param pipelineName Name of pipeline to get tree for
    /// \return Pointer to a standard item model that holdes the dir/file hierarchy
    ///         or NULL if the pipeline is not found.
    ///
    WStandardItemModel* getResultsPipelineTree(const std::string& pipelineName);

protected:

    /// Pipeline structure
    class PipelineConf
    {
    public:
        PipelineConf()  { mModel = new WStandardItemModel; }
        ~PipelineConf() { delete mModel; }

        /// Model of pipeline tree
        WStandardItemModel* mModel;

        /// Pipeline description
        std::string mDescription;

        /// Pipeline tag for meta script
        std::string mMetaTag;

    };

    ///
    /// Parse XML tree
    ///
    bool parseXMLTree(mxml_node_t *tree, const std::string& configPath);

    ///
    ///  Parse pipeline node
    ///
    bool parsePipelineNode(mxml_node_t *pipelineNode, const std::string& configPath);

    ///
    ///  Parse <FilePattern> node
    ///
    bool parseFilePatternNode(WStandardItem *item, mxml_node_t *filePatternNode,
                               const std::string& configPath, int indent);


    /// Map relating pipeline by string to a tree of entries
    std::map<std::string, PipelineConf*> mPipelineMap;
};

#endif // CONFIGXML_H