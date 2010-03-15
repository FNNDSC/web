//
//
//  Description:
//      Definition of class that loads an XML file that describes a project
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PROJECTXML_H
#define PROJECTXML_H

#include <mxml.h>

///
/// \class ProjectXML
/// \brief Object for accessing project file
///
class ProjectXML
{
public:

    /// Search term node
    typedef struct
    {
        /// Field to search
        std::string mField;

        /// Type of search
        std::string mType;

        /// Search expression
        std::string mExpr;

    } SearchTermNode;

    ///
    /// Constructor
    ///
    ProjectXML();

    ///
    /// Destructor
    ///
    virtual ~ProjectXML();

    ///
    /// Load [rpkect from XML file
    /// \param configFile Full path to project XML file
    /// \return Whether loading was successful
    ///
    bool loadFromFile(const std::string& configFile);

    ///
    /// Get search match type
    ///
    std::string getSearchMatchType() const      {   return mSearchMatchType;    }

    ///
    /// Get search term nodes
    ///
    std::list<SearchTermNode>& getSearchTerms() {   return mSearchTerms;    }

protected:

    ///
    /// Parse XML tree
    ///
    bool parseXMLTree(mxml_node_t *tree, const std::string& configPath);

    ///
    ///  Parse <SearchTerm> node
    ///
    bool parseSearchTermNode(mxml_node_t *searchTermNode, const std::string& configPath);

    /// List of search terms
    std::list<SearchTermNode> mSearchTerms;

    /// Search match type
    std::string mSearchMatchType;
};

#endif // PROJECTXML_H
