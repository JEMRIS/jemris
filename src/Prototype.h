/** @file Prototype.h
 *  @brief Implementation of JEMRIS Prototype
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

#include     "StrX.h"
#include     "Attribute.h"
#include     "TPOI.h"
#include     "Debug.h"
#include     "World.h"

#include     <stdexcept>
#include     <map>
#include     <vector>
#include     <typeinfo>
#include     <cmath>
#include     <sstream>
#include     <fstream>

#include     <ginac/ginac.h>
#include     <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE


enum PrepareMode{	PREP_INIT,	PREP_VERBOSE,	PREP_UPDATE					};
enum Type		{	MOD_PULSE,		MOD_ATOM,	MOD_CONCAT,	MOD_CONTAINER, MOD_VOID,	COIL};

//parameter class declaration
class Parameters;

//Macros for attribute creation in Prototype::Prepare()
#define ATTRIBUTE(KEY,VAL)  		if (mode!=PREP_UPDATE && m_attributes.find(KEY)==m_attributes.end() ) \
										m_attributes.insert(pair<string,Attribute*>(KEY,new Attribute(KEY,this, true, true,VAL)));
#define HIDDEN_ATTRIBUTE(KEY,VAL)   if (mode!=PREP_UPDATE && m_attributes.find(KEY)==m_attributes.end() ) \
										m_attributes.insert(pair<string,Attribute*>(KEY,new Attribute(KEY,this,false, true,VAL)));
#define UNOBSERVABLE_ATTRIBUTE(KEY) if (mode!=PREP_UPDATE && m_attributes.find(KEY)==m_attributes.end() ) \
										m_attributes.insert(pair<string,Attribute*>(KEY,new Attribute(KEY,this,false,false)));


using std::ofstream;
using std::string;
using std::map;

/**
 * @brief Prototype super class.
 */
class Prototype {

 public:

    /**
     * @brief Constructor
     *
     * Default constructor
     */
    Prototype () { m_prepared=false; };

    /**
     * @brief Default destructor.
     *
     * Default destrcutor
     */
    virtual ~Prototype () {};

    /**
     * @brief Default copy constructor.
     *
     * Default copy constructor.
     */
    Prototype (const Prototype&) {};

    /**
     * @brief Get Parent
     *
     * @return Parent module.
     */
    virtual Prototype* GetParent   () {return NULL;};

    /**
     * @brief Clone a prototype.
     *
     * Create and recieve a Clone of an unprepared prototype.
     */
    virtual Prototype* Clone      () const           = 0;

    /**
     * @brief Prepare this pulse.
     *
     * Called many times when root node prepares. First time not verbose.
     * Successors must overload this method.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return Success
     */
    virtual bool       Prepare    (PrepareMode mode) = 0;

    /**
     * @brief Check if the Prototype is prepared.
     *
     * @return True, if prepared.
     */
    inline bool IsPrepared () {return m_prepared;};


    /**
     * @brief Initialise this prototype
     *
     * @param conf  DOMNode with configuration
     */
    virtual void       Initialize (DOMNode* conf)    = 0;

    /**
     * @brief Set the name of this module
     *
     * @param name New name of this module
     */
    void               SetName    (string name);

    /**
     * @brief Get the DOMNode of this module
     *
     * @return The value of the private member m_node
     */
    inline DOMNode* GetNode () {return m_node;};

    /**
     * @brief Set the DOMNode of this module
     *
     * @param node The new DOMNode
     */
    void SetNode (DOMNode* node) {m_node=node;};

    /**
     * @brief A global sub-string replacer.
     *
     * @param str       The string to change.
     * @param s1	The sub-string to be replaced.
     * @param s2	The sub-string to be inserted instead.
     * @return          True, if replacing took place at least once (s1 was in str).
     */
    static bool ReplaceString (string& str, const string& s1, const string& s2);

    /**
     * @brief A global string tokenizer.
     *
     * @param str        The string to tokenize.
     * @param delimiters The delimiters to split the string.
     * @return           The vector of tokenized sub-strings.
     */
    static vector<string>  Tokenize (const string& str, const string& delimiters = ",");

    /**
     * @brief Get the class type of this prototype.
     *
     * @return The tag name of the DOMNode.
     */
    inline string GetClassType () {return XMLString::transcode(GetNode()->getNodeName());};

    /**
     * @brief Get the module type of this module.
     *
     * @return The module type: one of MOD_PULSE, MOD_ATOM, MOD_CONCAT
     */
    inline Type GetType () {return m_type;};


    /**
     * @brief Get attribute value from the DOMNode
     *
     * @param attribute  The attribute of the DOMNode corresponding to this Module.
     * @return            The value of the attribute
     */
    string GetDOMattribute    (const string attribute){ return StrX(((DOMElement*) m_node)->getAttribute (StrX(attribute).XMLchar())).std_str() ;  }

    /**
     * @brief Check, if attribute exists in DOM node of this module
     *
     * @param attribute  The attribute of the DOMNode corresponding to this Module.
     * @return           True, if attribute exists in DOM (XML)
     */
    bool HasDOMattribute    (const string attribute) {	if (GetDOMattribute(attribute).empty()) return false; else return true;  }


    /**
     * @brief Get an Attribute
     *
     * @param name       The name of the attribute.
     * @return           The Attribute.
     */
    Attribute*	GetAttribute(string name);

    /**
     * @brief Copy observers from one attribute to another
     *
     * @param a1       source attribute (copy from here)
     * @param a1       target attribute (copy to here)
     */
    void	CopyObservers(Attribute* a1, Attribute* a2);

    /**
     * @brief Check if an attribute exist.
     *
     * @param name       The name of the attribute
     * @return           Success/Failure of operation.
     */
    inline bool HasAttribute (string name) {if (GetAttribute(name)==NULL) return false; else return true;};

    /**
     * @brief Hide an attribute.
     *
     * Attributes, which were declared with the ATTRIBUTE command in a base class
     * will be declared as invisible, i.e. they can not be set through XML.
     * This method has to called in the particular Prepare function of a method, however,
     * after the base class Prepare call.
     *
     * @param attrib       Name of the attribute to hide
     * @param observable   Keep this attribute still observable? (default: true)
     */
      void  HideAttribute (string attrib, bool observable = true);

    /**
     * @brief Get a Prototype by value of an attribute
     *
     * @param  name   Attribute name
     * @param  attrib Attribute value
     * @return        Found Prototype
     */
	inline virtual Prototype* GetPrototypeByAttributeValue (string name, string attrib) {return this;};

    /**
     * @brief Get the name of this module
     *
     * @return The value of the private member m_name
     */
    inline string GetName () { return m_name; };

    /**
     * @brief Set up the list of observations
     *
     * This function creates an Attribute list for all observations from
     * this Prototype. It further attaches the current attribute to the
     * observer list of the observed attributes.
     *
     * @param attrib		The observer (An Attribute of this Prototype)
     * @param prot_name		Name of the Prototype which holds the Attribute to observe
     * @param attrib_name	Name of the Attribute to observe
     * @param verbose		If true, dump possible warnings to stdout
     * @return success/failure of operation
     */
    bool Observe (Attribute* attrib, string prot_name, string attrib_name, bool verbose);

    /**
     * @brief Notify all observers of an attribute.
     *
     * @param val member variable.
     * @return true, if member has changed
     */
    template <typename T> bool Notify (const T& val) {

    	map<string,Attribute*>::iterator iter;
    	for(iter = m_attributes.begin(); iter != m_attributes.end(); iter++)
    	    if (iter->second->GetAddress() == ((void*) &val) ) return iter->second->Notify(val);

    	return false;
    }

    /**
     * @brief Check the state of an attribute.
     *
     * @param private member variable.
     * @return true, if member has changed
     */
    template <typename T> bool NewState (const T& val) {

    	map<string,Attribute*>::iterator iter;
    	for(iter = m_attributes.begin(); iter != m_attributes.end(); iter++)
    	    if (iter->second->GetAddress() == ((void*) &val) ) return iter->second->NewState(val);

    	return false;
    };
    
    /**
     * @brief Each Prototype has a double vector as a private member, which values
     * can be filled through XML and accessed from attributes within the same Prototype.
     *
     * @return Pointer to the double vector of this Prototype
     */
    vector<double>*	        GetVector()              {return &m_vector;  };


 protected:

    bool                        m_aux;         /**< @brief auxiliary helper variable for debugging purposes*/
    bool                        m_prepared;    /**< @brief True, after the first call to Prepare */
    string                      m_name;        /**< @brief Name of this Prototype */
    DOMNode*                    m_node;        /**< @brief The node configuring this Module */
    Type                        m_type;        /**< @brief The type of the module: one of MOD_PULSE, MOD_ATOM, MOD_CONCAT*/
    vector<double>              m_vector;      /**< @brief A vector which elements are accessible through loop counters.*/
    map<string,Attribute*>		m_attributes;  /**< @brief Map to connect a keyword with an Attribute*/
    vector<Attribute*>			m_obs_attribs; /**< @brief Vector of observed Attributes */

};

#endif /*_PROTOTYPE_H_*/
