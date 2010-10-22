//
//
//  Description:
//      Implementation of class that loads an XML file that configures
//      how the pipeline results are accumulated.
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include "ConfigXML.h"
#include <stdlib.h>
#include <stdio.h>

///
//  Namespaces
//
using namespace Wt;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
//

///
//  Constructor
//
ConfigXML::ConfigXML()
{
}

///
//  Destructor
//
ConfigXML::~ConfigXML()
{
    std::map<std::string, PipelineConf*>::iterator mapIter = mPipelineMap.begin();

    while (mapIter != mPipelineMap.end())
    {
        std::map<std::string, OptionNode*>::iterator optionIter = (mapIter->second)->mOptionMap.begin();
        while (optionIter != (mapIter->second)->mOptionMap.end())
        {
            delete optionIter->second;
            optionIter++;
        }
        delete mapIter->second;
        mapIter++;
    }


}

///////////////////////////////////////////////////////////////////////////////
//
//  Public Members
//
//

///
//  Load configuration from XML file
//
bool ConfigXML::loadFromFile(const std::string& configPath)
{
    mxml_node_t *tree;
    FILE *fp = fopen(configPath.c_str(), "r");
    if (fp == NULL)
    {
        WApplication::instance()->log("error") << "Opening XML configuration file: " << configPath;
        return false;
    }

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);

    return (parseXMLTree(tree, configPath));
}

///
//  Get the results entry list for a given pipeline by name
//
WStandardItemModel* ConfigXML::getResultsPipelineTree(const std::string& pipelineName)
{
    if (mPipelineMap.find(pipelineName) == mPipelineMap.end())
    {
        return NULL;
    }

    return (mPipelineMap[pipelineName]->mModel);
}

///
// Get the <Input> list for the pipeline by name
//
const std::vector<ConfigXML::InputNode>* ConfigXML::getPipelineInputs(const std::string& pipelineName)
{
    if (mPipelineMap.find(pipelineName) == mPipelineMap.end())
    {
        return NULL;
    }

    return (&mPipelineMap[pipelineName]->mInputList);
}

///
//  Get the maximum number of inputs used by any pipeline
//
int ConfigXML::getMaxPipelineInputs() const
{
    int maxInputs = 0;

    std::map<std::string, PipelineConf*>::const_iterator mapIter = mPipelineMap.begin();
    while(mapIter != mPipelineMap.end())
    {
        int curSize = mapIter->second->mInputList.size();
        if (curSize > maxInputs)
            maxInputs = curSize;
        mapIter++;
    }

    return maxInputs;
}

///
/// Get the <Options> map for the pipeline by name
/// \param pipelineName Name of pipeline to get tree for
/// \return Pointer option node tree or NULL if the pipeline is not found.
///
std::map<std::string, ConfigXML::OptionNode*>* ConfigXML::getOptionMap(const std::string& pipelineName)
{
    if (mPipelineMap.find(pipelineName) == mPipelineMap.end())
    {
        return NULL;
    }

    return (&mPipelineMap[pipelineName]->mOptionMap);
}

///
//  Get the pattern to use to match files as text files
//
std::string ConfigXML::getTextFilePattern() const
{
    return mTextFilePattern;
}

///
//  Get the pattern to use to match files as image files
//
std::string ConfigXML::getImageFilePattern() const
{
    return mImageFilePattern;
}


///
/// Get the list of preview patterns which specify patterns
/// of files to preview another file (for example, .png images
/// as preview of a .trk file
///
const std::list<ConfigXML::PreviewPatternNode>& ConfigXML::getPreviewPatterns() const
{
    return mPreviewPatterns;
}

///
//  Translate arguments to script using pipeline options specification given in main
//  configuration XML file
//
void ConfigXML::translateScriptArgs(WStandardItemModel *model, const std::string& metaScript, const std::string& arguments)
{
    std::map<std::string, OptionNode*> *optionMap = getOptionMap(metaScript);

    if (optionMap != NULL)
    {
        std::map<std::string, bool> argNotGivenMap;

        std::vector <WStandardItem*> options;
        std::vector <WStandardItem*> values;

        // Initialize all the "not givens" to true
        std::map<std::string, OptionNode*>::iterator mapIter = optionMap->begin();
        while(mapIter != optionMap->end())
        {
            argNotGivenMap[mapIter->first] = true;
            mapIter++;
        }

        istringstream iss(arguments);
        std::vector<string> tokens;
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));

        int index = 0;
        while(index < tokens.size())
        {
            string tag = tokens[index];
            index++;
            if (optionMap->find(tag) != optionMap->end())
            {
                argNotGivenMap[tag] = false;
                string arg="";
                // Check to see if the next token is an argument
                if (index < tokens.size() && tokens[index][0] != '-')
                {
                    arg=tokens[index];
                    index++;
                }

                std::list<OptionArgNode>::iterator iter = (*optionMap)[tag]->mOptionArgs.begin();
                while (iter != (*optionMap)[tag]->mOptionArgs.end())
                {
                    if (iter->mTag == arg)
                    {
                        options.push_back(new WStandardItem((*optionMap)[tag]->mDesc));
                        values.push_back(new WStandardItem(iter->mDesc));
                    }
                    else if (iter->mTag == "*")
                    {
                        options.push_back(new WStandardItem((*optionMap)[tag]->mDesc));
                        values.push_back(new WStandardItem(arg));
                    }

                    iter++;
                }
            }
        }

        mapIter = optionMap->begin();
        while(mapIter != optionMap->end())
        {
            if(argNotGivenMap[mapIter->first] == true && mapIter->second->mNotGivenText != "")
            {
                options.push_back(new WStandardItem(mapIter->second->mDesc));
                values.push_back(new WStandardItem(mapIter->second->mNotGivenText));
            }
            mapIter++;
        }

        for (int i = 0; i < options.size(); i++)
        {
            model->setItem(i, 0, options[i]);
            model->setItem(i, 1, values[i]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Parse XML tree
//
bool ConfigXML::parseXMLTree(mxml_node_t *tree, const std::string& configPath)
{
    if (tree == NULL)
    {
        return false;
    }

    mxml_node_t *resultsNode;
    resultsNode = mxmlFindElement(tree, tree, "Results", NULL, NULL, MXML_DESCEND);

    if (resultsNode == NULL)
    {
        WApplication::instance()->log("error") << "No <Results> node found in : " << configPath;
        return false;
    }

    mTextFilePattern = parsePatternNode(resultsNode, "TextFilePattern");
    mImageFilePattern = parsePatternNode(resultsNode, "ImageFilePattern");

    // Iterate over <PreviewPattern> nodes
    bool result = true;
    mxml_node_t *previewPatternNode;
    for (previewPatternNode = mxmlFindElement(resultsNode, resultsNode,
                                              "PreviewPattern",
                                              NULL, NULL,
                                              MXML_DESCEND);
        previewPatternNode != NULL;
        previewPatternNode = mxmlFindElement(previewPatternNode, resultsNode,
                                             "PreviewPattern",
                                             NULL, NULL,
                                             MXML_NO_DESCEND))
    {
        result = result && parsePreviewPatternNode(previewPatternNode, configPath);
    }


    mxml_node_t *pipelineNode;

    // Iterate over the <Pipeline> nodes
    for (pipelineNode = mxmlFindElement(resultsNode, resultsNode,
                                        "Pipeline",
                                        NULL, NULL,
                                        MXML_DESCEND);
        pipelineNode != NULL;
        pipelineNode = mxmlFindElement(pipelineNode, resultsNode,
                                       "Pipeline",
                                       NULL, NULL,
                                       MXML_NO_DESCEND))
    {
        result = result && parsePipelineNode(pipelineNode, configPath);
    }


    mxmlRelease(tree);
    return result;
}

///
//  Parse pipeline node
//
bool ConfigXML::parsePipelineNode(mxml_node_t *pipelineNode, const std::string& configPath)
{
    if (pipelineNode == NULL)
    {
        return false;
    }

    const char *pipelineName = mxmlElementGetAttr(pipelineNode, "name");
    if (pipelineName == NULL)
    {
        WApplication::instance()->log("error") << "No name specified for pipeline, ignoring: " << configPath;
        return false;
    }

    if (mPipelineMap.find(pipelineName) == mPipelineMap.end())
    {
        mPipelineMap[pipelineName] = new PipelineConf();

        const char *pipelineDesc = mxmlElementGetAttr(pipelineNode, "desc");
        if (pipelineDesc)
        {
            mPipelineMap[pipelineName]->mDescription = string(pipelineDesc);
        }

        const char *pipelineTag = mxmlElementGetAttr(pipelineNode, "tag");
        if (pipelineTag)
        {
            mPipelineMap[pipelineName]->mMetaTag = string(pipelineTag);
        }
    }

    mxml_node_t *filePatternsNode = mxmlFindElement(pipelineNode, pipelineNode, "FilePatterns", NULL, NULL, MXML_DESCEND);
    mxml_node_t *filePatternNode;

    // Iterate over the <FilePattern> nodes
    bool result = true;
    for (filePatternNode = mxmlFindElement(filePatternsNode, filePatternsNode,
                                            "FilePattern",
                                            NULL, NULL,
                                            MXML_DESCEND);
        filePatternNode != NULL;
        filePatternNode = mxmlFindElement(filePatternNode, filePatternsNode,
                                          "FilePattern",
                                          NULL, NULL,
                                          MXML_NO_DESCEND))
    {
        result = result && parseFilePatternNode(mPipelineMap[pipelineName]->mModel->invisibleRootItem(), filePatternNode, configPath, 0);
    }

    // Iterate over the <Option> nodes
    mxml_node_t *optionsNode = mxmlFindElement(pipelineNode, pipelineNode, "Options", NULL, NULL, MXML_DESCEND);
    mxml_node_t *optionNode;

    for (optionNode = mxmlFindElement(optionsNode, optionsNode,
                                        "Option",
                                        NULL, NULL,
                                        MXML_DESCEND);
        optionNode != NULL;
        optionNode = mxmlFindElement(optionNode, optionsNode,
                                      "Option",
                                      NULL, NULL,
                                      MXML_NO_DESCEND))
    {
        result = result && parseOptionNode(mPipelineMap[pipelineName]->mOptionMap, optionNode, configPath);
    }

    // Iterate over the <Input> nodes
    mxml_node_t *inputsNode = mxmlFindElement(pipelineNode, pipelineNode, "Inputs", NULL, NULL, MXML_DESCEND);
    mxml_node_t *inputNode;

    for (inputNode = mxmlFindElement(inputsNode, inputsNode,
                                     "Input",
                                     NULL, NULL,
                                     MXML_DESCEND);
        inputNode != NULL;
        inputNode = mxmlFindElement(inputNode, inputsNode,
                                    "Input",
                                    NULL, NULL,
                                    MXML_NO_DESCEND))
    {
        result = result && parseInputNode(mPipelineMap[pipelineName]->mInputList, inputNode, configPath);
    }


    return result;
}

///
//  Parse <PreviewPattern> node
//
bool ConfigXML::parsePreviewPatternNode(mxml_node_t *previewPatternNode, const std::string& configPath)
{
    if (previewPatternNode == NULL)
    {
        return false;
    }

    PreviewPatternNode newNode;

    newNode.mExpression = mxmlElementGetAttr(previewPatternNode, "pattern");
    newNode.mPreviewExpression = mxmlElementGetAttr(previewPatternNode, "files");

    mPreviewPatterns.push_back(newNode);

    return true;
}

///
//  Parse <FilePattern> node
//
bool ConfigXML::parseFilePatternNode(WStandardItem *item, mxml_node_t *filePatternNode,
                                     const std::string& configPath, int indent)
{
    if (filePatternNode == NULL)
    {
        return false;
    }

    FilePatternNode newNode;

    newNode.mExpression = mxmlElementGetAttr(filePatternNode, "name");
    const char* dir = mxmlElementGetAttr(filePatternNode, "dir");
    const char* recurse = mxmlElementGetAttr(filePatternNode, "recurse");

    if (dir == NULL)
    {
        newNode.mDirectory = false;
    }
    else
    {
        newNode.mDirectory = (string(dir) == "true" ? true : false);
    }

    if (recurse == NULL)
    {
        newNode.mRecurse = false;
    }
    else
    {
        newNode.mRecurse = (string(recurse) == "true" ? true : false);
    }

    WStandardItem *newItem = new WStandardItem();
    newItem->setData(newNode, UserRole);
    item->appendRow(newItem);

    string space;
    for(int i = 0; i < indent; i++)
        space += "  ";

    bool result = true;
    mxml_node_t *curNode;
    for(curNode = mxmlFindElement(filePatternNode, filePatternNode,
                                          "FilePattern",
                                          NULL, NULL,
                                          MXML_DESCEND);
        curNode != NULL;
        curNode = mxmlFindElement(curNode, filePatternNode,
                                        "FilePattern",
                                        NULL, NULL,
                                        MXML_NO_DESCEND))
    {
        result = result && parseFilePatternNode(newItem, curNode, configPath, indent + 1);
    }

    return result;
}

///
//  Parse <Option> node
//
bool ConfigXML::parseOptionNode(std::map<std::string, OptionNode*>& optionMap, mxml_node_t *optionNode,
                                const std::string& configPath)
{
    if (optionNode == NULL)
    {
        return false;
    }


    const char* tag = mxmlElementGetAttr(optionNode, "tag");
    if (tag == NULL)
    {
        WApplication::instance()->log("error") << "No tag specified for <Option> node. Ignoring.";
        return false;
    }

    // Create the node if it has not yet been found
    if (optionMap.find(tag) == optionMap.end())
    {
        optionMap[tag] = new OptionNode();
    }

    optionMap[tag]->mDesc = mxmlElementGetAttr(optionNode, "desc");
    optionMap[tag]->mNotGivenText = mxmlElementGetAttr(optionNode, "notgiven");

    mxml_node_t *optionArgNode;
    bool result = true;
    for (optionArgNode = mxmlFindElement(optionNode, optionNode,
                                            "OptionArg",
                                            NULL, NULL,
                                            MXML_DESCEND);
        optionArgNode != NULL;
        optionArgNode = mxmlFindElement(optionArgNode, optionNode,
                                          "OptionArg",
                                          NULL, NULL,
                                          MXML_NO_DESCEND))
    {
        result = result && parseOptionArgNode(optionMap[tag]->mOptionArgs, optionArgNode, configPath);
    }

    return result;
}

///
//  Parse <OptionArg> node
//
bool ConfigXML::parseOptionArgNode(std::list<OptionArgNode>& optionArgs, mxml_node_t *optionArgNode,
                                   const std::string& configPath)
{
    if (optionArgNode == NULL)
    {
        return false;
    }

    const char* tag = mxmlElementGetAttr(optionArgNode, "tag");
    const char* desc = mxmlElementGetAttr(optionArgNode, "desc");
    if (tag == NULL)
    {
        WApplication::instance()->log("error") << "No tag specified for <OptionArg> node. Ignoring.";
        return false;
    }

    OptionArgNode newNode;
    newNode.mTag = tag;
    newNode.mDesc = desc;
    optionArgs.push_back(newNode);

    return true;
}

///
//  Parse <Input> node
//
bool ConfigXML::parseInputNode(std::vector<InputNode>& inputList, mxml_node_t *inputNode,
                               const std::string& configPath)
{
    if (inputNode == NULL)
    {
        return false;
    }

    const char* name = mxmlElementGetAttr(inputNode, "name");
    if (name == NULL)
    {
        WApplication::instance()->log("error") << "No name specified for <Input> node. Ignoring.";
        return false;
    }

    const char* arg = mxmlElementGetAttr(inputNode, "arg");

    InputNode newNode;
    newNode.mName = name;
    newNode.mArg = "";
    if (arg != NULL)
    {
        newNode.mArg = arg;
    }
    inputList.push_back(newNode);

    return true;
}

///
//  Parse <[Text|Image]FilePattern> node
//
std::string ConfigXML::parsePatternNode(mxml_node_t *baseNode, std::string nodeName) const
{
    std::string result;
    mxml_node_t *node = mxmlFindElement(baseNode, baseNode, nodeName.c_str(),
                                        NULL, NULL, MXML_DESCEND);

    if (node != NULL)
    {
        const char *patternText = mxmlElementGetAttr(node, "pattern");

        if (patternText != NULL)
        {
            result = patternText;
        }
        else
        {
            WApplication::instance()->log("error") << "No pattern tag for " << nodeName << ". Ignoring.";
        }
    }

    return result;
}
