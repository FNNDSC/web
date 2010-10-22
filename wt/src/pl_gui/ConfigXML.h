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
#include <vector>
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

    /// Preview pattern node
    typedef struct
    {
        /// Expression for matching files
        std::string mExpression;

        /// Expression for preview files
        std::string mPreviewExpression;

    } PreviewPatternNode;

    /// Input node
    typedef struct
    {
        /// Display name for input
        std::string mName;

        /// Command-line argument flag (if applicable)
        std::string mArg;

    } InputNode;

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

    ///
    /// Get the <Input> list for the pipeline by name
    /// \param pipelineName Name of pipeline to get inputs for
    /// \return Pointer input node list or NULL if the pipeline is not found.
    ///
    const std::vector<InputNode>* getPipelineInputs(const std::string& pipelineName);

    ///
    //  Get the maximum number of inputs used by any pipeline
    //
    int getMaxPipelineInputs() const;


    ///
    /// Translate arguments to script using pipeline options specification given in main
    /// configuration XML file
    /// \param rootItem Root item to append rows that contain entries for optoins
    /// \param metaScript Meta script name (tract_meta.bash, fs_meta.bash, etc.)
    /// \param arguments Arguments that were sent to script
    ///
    void translateScriptArgs(WStandardItemModel *model, const std::string& metaScript, const std::string& arguments);


    ///
    /// Get the pattern to use to match files as text files
    ///
    std::string getTextFilePattern() const;

    ///
    /// Get the pattern to use to match files as image files
    ///
    std::string getImageFilePattern() const;

    ///
    /// Get the list of preview patterns which specify patterns
    /// of files to preview another file (for example, .png images
    /// as preview of a .trk file
    ///
    const std::list<PreviewPatternNode>& getPreviewPatterns() const;

    ///

protected:

    /// Option Argument node
    typedef struct
    {
        // Option argument tag
        std::string mTag;

        // Option argument description
        std::string mDesc;

    } OptionArgNode;

    /// Option Node
    typedef struct
    {
        /// Option description
        std::string mDesc;

        /// Option not-given text
        std::string mNotGivenText;

        /// Option arguments
        std::list<OptionArgNode> mOptionArgs;

    } OptionNode;



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

        /// Option map
        std::map<std::string, OptionNode*> mOptionMap;

        /// List of inputs
        std::vector<InputNode> mInputList;
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
    ///  Parse <PreviewPattern> node
    ///
    bool parsePreviewPatternNode(mxml_node_t *previewPatternNode, const std::string& configPath);

    ///
    ///  Parse <FilePattern> node
    ///
    bool parseFilePatternNode(WStandardItem *item, mxml_node_t *filePatternNode,
                               const std::string& configPath, int indent);

    ///
    ///  Parse <Option> node
    ///
    bool parseOptionNode(std::map<std::string, OptionNode*>& optionMap, mxml_node_t *optionNode,
                         const std::string& configPath);

    ///
    ///  Parse <OptionArg> node
    ///
    bool parseOptionArgNode(std::list<OptionArgNode>& optionArgs, mxml_node_t *optionArgNode,
                             const std::string& configPath);

    ///
    ///  Parse <Input> node
    ///
    bool parseInputNode(std::vector<InputNode>& inputLIst, mxml_node_t *inputNode,
                         const std::string& configPath);


    ///
    ///  Parse <[Text|Image]FilePattern> node
    ///
    std::string parsePatternNode(mxml_node_t *baseNode, std::string nodeName) const;

    ///
    /// Get the <Options> map for the pipeline by name
    /// \param pipelineName Name of pipeline to get tree for
    /// \return Pointer option node tree or NULL if the pipeline is not found.
    ///
    std::map<std::string, OptionNode*>* getOptionMap(const std::string& pipelineName);


    /// Map relating pipeline by string to a tree of entries
    std::map<std::string, PipelineConf*> mPipelineMap;

    /// Text file pattern
    std::string mTextFilePattern;

    /// ImageFilePattern
    std::string mImageFilePattern;

    /// List of preview patterns
    std::list<PreviewPatternNode> mPreviewPatterns;
};

#endif // CONFIGXML_H
