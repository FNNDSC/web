//
//
//  Description:
//      Implementation of class that loads an XML file that describes a project
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
#include "ProjectXML.h"
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
ProjectXML::ProjectXML() :
    mSearchMatchType("any")
{
}

///
//  Destructor
//
ProjectXML::~ProjectXML()
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
bool ProjectXML::loadFromFile(const std::string& projectPath)
{
    mxml_node_t *tree;
    FILE *fp = fopen(projectPath.c_str(), "r");
    if (fp == NULL)
    {
        WApplication::instance()->log("error") << "Opening Project XML configuration file: " << projectPath;
        return false;
    }

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);

    return (parseXMLTree(tree, projectPath));
}


///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Parse XML tree
//
bool ProjectXML::parseXMLTree(mxml_node_t *tree, const std::string& configPath)
{
    if (tree == NULL)
    {
        return false;
    }

    mxml_node_t *searchMatchNode = mxmlFindElement(tree, tree, "SearchMatch", NULL, NULL, MXML_DESCEND);
    if (searchMatchNode != NULL)
    {
        const char* matchType = mxmlElementGetAttr(searchMatchNode, "type");
        if (matchType != NULL)
        {
            mSearchMatchType = matchType;
        }
    }

    // Iterate over <SearchTerm> nodes
    bool result = true;
    mxml_node_t *searchTermNode;
    for (searchTermNode = mxmlFindElement(tree, tree,
                                              "SearchTerm",
                                              NULL, NULL,
                                              MXML_DESCEND);
        searchTermNode != NULL;
        searchTermNode = mxmlFindElement(searchTermNode, tree,
                                             "SearchTerm",
                                             NULL, NULL,
                                             MXML_NO_DESCEND))
    {
        result = result && parseSearchTermNode(searchTermNode, configPath);
    }

    mxmlRelease(tree);
    return result;
}

///
//  Parse <SearchTerm> node
//
bool ProjectXML::parseSearchTermNode(mxml_node_t *searchTermNode, const std::string& configPath)
{
    if (searchTermNode == NULL)
    {
        return false;
    }


    SearchTermNode newNode;

    newNode.mField = mxmlElementGetAttr(searchTermNode, "field");
    newNode.mType = mxmlElementGetAttr(searchTermNode, "type");
    newNode.mExpr = mxmlElementGetAttr(searchTermNode, "expr");

    mSearchTerms.push_back(newNode);

    return true;
}

