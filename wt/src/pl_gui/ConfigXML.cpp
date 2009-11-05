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

    return (mPipelineMap[pipelineName]);
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

    mxml_node_t *pipelineNode;

    // Iterate over the <Pipeline> nodes
    bool result = true;
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
        mPipelineMap[pipelineName] = new WStandardItemModel();
    }

    mxml_node_t *filePatternsNode = mxmlFindElement(pipelineNode, pipelineNode, "FilePatterns", NULL, NULL, MXML_DESCEND);
    if (filePatternsNode == false)
    {
        WApplication::instance()->log("error") << "No patterns specified for " << pipelineName << ". Ignoring.";
        return false;
    }

    mxml_node_t *filePatternNode;

    // Iterate over the <Pipeline> nodes
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
        result = result && parseFilePatternNode(mPipelineMap[pipelineName]->invisibleRootItem(), filePatternNode, configPath, 0);
    }

    return result;
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
