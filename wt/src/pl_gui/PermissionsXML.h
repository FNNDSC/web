//
//
//  Description:
//      Definition of class that loads an XML file that describes permissions (user/group)
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PERMISSIONSXML_H
#define PERMISSIONSXML_H

#include <mxml.h>
#include <map>
#include <list>
#include <string>

///
/// \class PermissionsXML
/// \brief Object for accessing permissions file
///
class PermissionsXML
{
public:
    ///
    /// Constructor
    ///
    PermissionsXML();

    ///
    /// Destructor
    ///
    virtual ~PermissionsXML();

    ///
    /// Load permissions from XML file
    /// \param permissionsPath Full path to project XML file
    /// \return Whether loading was successful
    ///
    bool loadFromFile(const std::string& permissionsPath);

    ///
    /// Return whether user is in the given group
    ///
    bool userInGroup(const std::string& userName, const std::string& groupName) const;

protected:

    ///
    /// Parse XML tree
    ///
    bool parseXMLTree(mxml_node_t *tree, const std::string& configPath);

    ///
    ///  Parse <Group> node
    ///
    bool parseGroupNode(mxml_node_t *groupNode, const std::string& configPath);

protected:

    /// Group map that contains list of users per group
    std::map<std::string, std::list<std::string> > mGroupMap;
};

#endif // PERMISSIONSXML_H
