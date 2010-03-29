//
//
//  Description:
//      Implementation of class that loads an XML file that describes permissions (user/group)
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#include <Wt/WApplication>
#include <Wt/WLogger>
#include "PermissionsXML.h"
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
PermissionsXML::PermissionsXML()
{
}

///
//  Destructor
//
PermissionsXML::~PermissionsXML()
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
bool PermissionsXML::loadFromFile(const std::string& permissionsPath)
{
    mGroupMap.clear();

    mxml_node_t *tree;
    FILE *fp = fopen(permissionsPath.c_str(), "r");
    if (fp == NULL)
    {
        WApplication::instance()->log("error") << "Opening Permissions XML configuration file: " << permissionsPath;
        return false;
    }

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    fclose(fp);

    return (parseXMLTree(tree, permissionsPath));
}

///
//  Return whether user is in the given group
//
bool PermissionsXML::userInGroup(const std::string& userName, const std::string& groupName) const
{
    std::map<std::string, std::list<std::string> >::const_iterator mapIter = mGroupMap.find(groupName);
    if (mapIter == mGroupMap.end())
    {
        return false;
    }

    std::list<std::string>::const_iterator iter = mapIter->second.begin();
    while (iter != mapIter->second.end())
    {
        if (userName == (*iter))
        {
            return true;
        }
        iter++;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Protected Members
//
//

///
//  Parse XML tree
//
bool PermissionsXML::parseXMLTree(mxml_node_t *tree, const std::string& configPath)
{
    if (tree == NULL)
    {
        return false;
    }

    // Iterate over <Group> nodes
    bool result = true;
    mxml_node_t *groupNode;
    for (groupNode = mxmlFindElement(tree, tree,
                                     "Group",
                                     NULL, NULL,
                                     MXML_DESCEND);
        groupNode != NULL;
        groupNode = mxmlFindElement(groupNode, tree,
                                     "Group",
                                     NULL, NULL,
                                     MXML_NO_DESCEND))
    {
        result = result && parseGroupNode(groupNode, configPath);
    }

    mxmlRelease(tree);
    return result;
}

///
//  Parse <Group> node
//
bool PermissionsXML::parseGroupNode(mxml_node_t *groupNode, const std::string& configPath)
{
    if (groupNode == NULL)
    {
        return false;
    }

    const char *groupName = mxmlElementGetAttr(groupNode, "name");
    if (groupName == NULL)
    {
        WApplication::instance()->log("error") << "No name specified for <Group>, ignoring: " << configPath;
        return false;
    }

    std::string groupStr(groupName);

    // Add users to group
    mxml_node_t *userNode;
    for (userNode = mxmlFindElement(groupNode, groupNode,
                                     "User",
                                     NULL, NULL,
                                     MXML_DESCEND);
        userNode != NULL;
        userNode = mxmlFindElement(userNode, groupNode,
                                     "User",
                                     NULL, NULL,
                                     MXML_NO_DESCEND))
    {
        const char *userName = mxmlElementGetAttr(userNode, "name");
        if (userName == NULL)
        {
            WApplication::instance()->log("error") << "No name specified for <User>, ignoring: " << configPath;
            continue;
        }

        mGroupMap[groupStr].push_back(userName);
    }

    return true;
}
