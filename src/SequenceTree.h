/** @file SequenceTree.h
 *  @brief Implementation of JEMRIS SequenceTree
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SEQUENCETREE_H_
#define SEQUENCETREE_H_

#include "ModulePrototypeFactory.h"
#include "XMLIO.h"


using std::map;

using std::string;

// Class declarations to avoid including Module.h and thus a rereferenciation.
class Module;
class ConcatSequence;
class ContainerSequence;

//! Tree representation of the sequence

class SequenceTree {

 friend class Container;

 public:

    SequenceTree();			/**< default constructor */
   ~SequenceTree();			/**< public virtual default destructor */

    /**
     * Initialize the sole instance
     *
     * @param  seqFile uri to the xml file representing the sequence tree to
     *                 to Initialize from
     * @return error code (0: ok, 1: file io error, 2: dtd validation error)
     */
    void                  Initialize    (string  seqFile);

    /**
     * Get the child nodes of the given node
     *
     * @param node the node for which the child nodes are requested
     * @return pointer to DOMNodeList including the children of the node for which the
     *         children are requested
     */
    DOMNodeList*         GetChildNodes (DOMNode* node);

    /**
     * Get the Parent node of the given node
     *
     * @param node the node for which the parent node is requested
     * @return pointer to the requested parent node
     */
    DOMNode*             GetParentNode (DOMNode* node);

    /**
     * Get the attributes of the given node
     *
     * @param node the node for which the attributes are requested
     */
    DOMNamedNodeMap*     GetAttributes (DOMNode* node);

    /**
     * Get a Module based on its DOMNode
     *
     * @param node The DOMNode
     */
    Module*               GetModule    (DOMNode*   node);

    /**
     * Get a puls based on its name
     *
     * @param name Unique name of the Module to be added
     */
    unsigned int          AddModule    (string   name);

    /**
     * Run through the sequence tree starting with a given node and perform on its children
     *
     * @param node  The given DOM node
     * @param ptr   More elaborate description here please
     * @param fun   Pointer to the performing function
	 * @param depth Depth of run (default=0).
     *
     * @return The depth of the tree
     */
    int        RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*) , int depth = 0);

    /**
     * Populate sequence
     *
     * Populate sequence with the given XML by cloning all needed objects.
     */
    unsigned int          Populate      ();

    /**
     * A static function to create modules. Called by SequenceTree::Populate() as
     * function pointer argument to SequenceTree::RunTree() .
	 *
     * @param ptr    Pointer to the SequenceTree object.
	 * @param node   More elaborate description here please.
     * @return       Error code.
     */
    static unsigned int CreateModule(void* ptr, DOMNode* node);

    /**
     * Get the parent module
     *
     * @param node The node referred to.
     */
	Module*               GetParent    (DOMNode* node);

    /**
     * Get vector of chilren
     *
	 * @param node The node referred to.
     */
	vector<Module*>       GetChildren  (DOMNode* node);


    /**
     * Get child at position
     *
	 * @param node The node referred to.
	 * @param position The position in the vector of children
     */
	Module*               GetChild (DOMNode* node, unsigned int position);

    /**
     * Get the root ConcatSequence module.
     *
     * @return The root ConcatSequence module.
     */
	ConcatSequence*      GetRootConcatSequence    ();

    /**
     * Get the ContainerSequence module.
     *
     * @return The ContainerSequence module.
     */
	ContainerSequence*      GetContainerSequence    ();

    /**
     * Get first module in the tree, which has a matching
     * attribute (name,value) pair in its DOMNode.
     *
     * @param name       The name of the attribute.
     * @param value      The value of the attribute.
     * @return           The module.
     */
     Module*      GetModuleByAttributeValue  (string name, string value);

    /**
     * A static string tokenizer. Needed for setting module dependencies.
     *
     * @param str        The string to tokenize.
     * @param delimiters The delimiters to split the string.
     * @return           The vector of tokenized sub-strings.
     */
     static vector<string>  Tokenize(const string& str, const string& delimiters = ",");

     /**
      * Get the DOM document
      *
      * @return the requested DOM document
      */
     DOMDocument*   GetDOMDocument(){return m_dom_doc;};

     /**
      * Get the module prototype factory
      *
      * @return the requested prototype factory
      */
     ModulePrototypeFactory* GetMPF(){return m_mpf;};

     /**
      * Get the map of clonable modules
      *
      * @return Map of DOM nodes and modules
      */
     map<DOMNode*, Module*>*  GetModuleMap(){return &m_Modules;};

     /**
      * Get the depth of the tree
      *
      * @return the depth
      */
     int  GetDepth(){return m_depth;};

     /**
      * Get the directory of the sequence file
      *
      * @return the directory
      */
     string  GetSequenceDirectory();

     /**
      * Get the filename of the sequence file
      *
      * @return the filename
      */

     string GetSequenceFilename();

     /**
      * Write xml file containing all modules of the sequence framework.
      */
     void          SerializeModules(string xml_file);

     /**
      * Get pointer to the sole instance of the 'Parameters' module.
      */
     Parameters*      GetParameters(){return m_parameters;}

     /**
      * @brief Get my status of parsing XML
      */
     bool GetStatus           () { return m_state; }



 private:
    bool                 m_state;          /**< My status                       */
    int                  m_depth;          /**< The depth of the tree           */
    Parameters*      		m_parameters;   /**< Pointer to the sole instance of the Parameter Module */
    ConcatSequence*      	m_root_seq;   /**< Pointer to the root ConcatSequence */
    DOMDocument*     		m_dom_doc;   /**< The DOM document containing the whole sequence      */
    ModulePrototypeFactory* m_mpf;
    XMLIO*                  m_xio;
    map<DOMNode*, Module*>  m_Modules;
    string                  m_seq_file;  /**< path of sequence file  */

};

#endif /*SEQUENCETREE_H_*/
