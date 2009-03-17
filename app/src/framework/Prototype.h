/** @file Prototype.h
 *  @brief Implementation of JEMRIS Prototype
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#include     "TPOI.h"
#include     "Debug.h"
#include     "StrX.h"
#include     "World.h"

#include     <stdexcept>
#include     <map>
#include     <vector>
#include     <typeinfo>
#include     <cmath>

#include     <ginac/ginac.h>
#include     <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE


enum PrepareMode {
	PREP_INIT,
	PREP_VERBOSE,
	PREP_UPDATE
};

enum Type {
	MOD_PULSE,
	MOD_ATOM,
	MOD_CONCAT,
	MOD_VOID,
	COIL
};

#define ATTRIBUTE(THEKEY,THEADDR) \
     if (mode!=PREP_UPDATE) m_attrib_type.insert(pair<string,string>(THEKEY,typeid(THEADDR).name()));  \
     if (mode!=PREP_UPDATE) m_attrib_addr.insert(pair<string,void*>(THEKEY,THEADDR)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_xml.insert(pair<string,bool>(THEKEY,true)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_dynamic.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_observable.insert(pair<string,bool>(THEKEY,true)) ;  \
     if (mode!=PREP_UPDATE) SetAttribSymbol(THEKEY) ;

#define HIDDEN_ATTRIBUTE(THEKEY,THEADDR) \
     if (mode!=PREP_UPDATE) m_attrib_type.insert(pair<string,string>(THEKEY,typeid(THEADDR).name()));  \
     if (mode!=PREP_UPDATE) m_attrib_addr.insert(pair<string,void*>(THEKEY,THEADDR)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_xml.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_dynamic.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_observable.insert(pair<string,bool>(THEKEY,true)) ;  \
     if (mode!=PREP_UPDATE) SetAttribSymbol(THEKEY) ;

#define UNOBSERVABLE_ATTRIBUTE(THEKEY,THEADDR) \
     if (mode!=PREP_UPDATE) m_attrib_type.insert(pair<string,string>(THEKEY,typeid(THEADDR).name()));  \
     if (mode!=PREP_UPDATE) m_attrib_addr.insert(pair<string,void*>(THEKEY,THEADDR)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_xml.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_dynamic.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) m_attrib_observable.insert(pair<string,bool>(THEKEY,false)) ;  \
     if (mode!=PREP_UPDATE) SetAttribSymbol(THEKEY) ;

using std::ofstream;
using std::string;
using std::map;

class Parameters;

/**
 * @brief Prototype super class.
 */
class Prototype {

 public:


    /**
     * @brief Pair of prototype and attribute name.
     *
     * Type definition of a Subject (another Module) which is observed
     * by the current module. The pair <Module*,string> denotes the
     * subject-module and its attribute which is observed.
     */
    typedef pair <Prototype*,string> ObsModAttr;

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
    virtual Prototype* GetParent   () {};

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
     * @brief Set the symbol of a member variable of an attribute.
     *
     * @param attrib     The name of the attribute.
     */
     void SetAttribSymbol (string attrib);

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
     * @brief A string tokenizer.
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
     * @brief A global sub-string replacer.
     *
     * @param str       The string to change.
     * @param s1	The sub-string to be replaced.
     * @param s2	The sub-string to be inserted instead.
     * @return          True, if replacing took place at least once (s1 was in str).
     */
    bool		ReplaceString (string& str, const string& s1, const string& s2);

    /**
     * @brief Set a GiNac expression of an attribute
     *
     * @param attrib  Attribute to be set.
     * @param sexpr   The expression as string.
	 * @param mode    Prepare mode.
     */
    bool   SetExpression (string attrib, string sexpr, PrepareMode mode) ;

    /**
     * @brief Evaluate GiNac expressions for an observing attribute.
     *
     * @param attrib Attribute to be evaluated.
     * @param record If true, keep track of triggered attribute changes in observers.
	 * @param mode   Prepare mode.
     */
    void   EvalExpressions (string attrib, PrepareMode mode, bool record=false) ;

    /**
     * @brief Check, if attribute exists in DOM node of this module
     *
     * @param attribute  The attribute of the DOMNode corresponding to this Module.
     * @return           True, if attribute exists in DOM (XML)
     */
    bool HasDOMattribute    (const string attribute);

    /**
     * @brief Check if an attribute exist.
     *
     * @param key        The name of the attribute
     * @return           Success/Failure of operation.
     */
    inline bool HasAttribute (string key) {return (m_attrib_addr.find(key) != m_attrib_addr.end());};

    /**
     * @brief Check if an attribute is dynamic.
     *
     * @param key        The name of the attribute
     * @return           True for dynamic attributes.
     */
    inline bool IsDynamic (string key) { return ( (m_attrib_dynamic.find(key))->second ); };

    /**
     * @brief Set an attribute to be dynamic.
     *
     * @param key        The name of the attribute
     */
    inline void SetDynamic (string key) { (m_attrib_dynamic.find(key))->second = true; };

    /**
     * @brief Get a module by value of an attribute
     *
     * @param  name   Attribute name
     * @param  attrib Attribute value
     * @return        Found module
     */
	inline virtual Prototype* GetModuleByAttributeValue (string name, string attrib) {return NULL;};

    /**
     * @brief Get attribute value from the DOMNode
     *
     * @param s          The value of the attribute
     * @param attribute  The attribute of the DOMNode corresponding to this Module.
     * @return           Success/Failure of operation.
     */
    bool GetAttribute    (string &s, const string attribute);

    /**
     * @brief Get the name of this module
     *
     * @return The value of the private member m_name
     */
    inline string GetName () { return m_name; };

    /**
     * @brief Overwrite a member variable of an attribute.
     *
     * @param attribute The attribute
     * @param value     The string to convert to a number
     * @return          Success of the operation
     */
    bool WriteAttribAddress (string attribute, string value);

    /**
     * @brief Get the symbol of a member variable of an attribute.
     *
     * @param attrib     The name of the attribute.
     * @return           The GiNaC symbol
     */
    GiNaC::symbol GetAttribSymbol (string attrib);

    /**
     * @brief Get the typeid of a member variable of an attribute.
     *
     * @param attrib     The name of the attribute.
     * @return           The typeid string
     */
    string GetAttribTypeID (string attrib);

    /**
     * @brief Get adress of a private meber variable which connects to an attribute.
     *
     * @param key        The name of the attribute
     * @return           The adress to the member variable of this attribute.
     */
    inline void* GetAttribAddress (string key) {return m_attrib_addr.find(key)->second; };

    /**
     * @brief Get the typeid of a member variable of an attribute.
     *
     * @param p     The pointer to the member variable.
     * @return           The typeid string
     */
     string GetTypeID (void *p);

    /**
     * @brief Observe a module for an attribute.
     *
     * @param subj_attrib Name of the attribute to observe.
     * @param subj_mod    The Module to observe.
     * @param obs_attrib  Name of the local attribute which triggers the observation.
     */
    void Observe (string subj_attrib, Prototype* subj_mod, string obs_attrib);

    /**
     * @brief Notify all observers of an attribute.
     *
     * This function overwrites all members of all observers which
     * Observe the attribute.
     *
     * @param attrib The observed attribute.
     * @param record If true, keep track of triggered attribute changes in observers.
     */
    void Notify (string attrib, bool record=false);

    /**
     * @brief Notify all observers of an attribute.
     *
     * @param ppriv Pointer to a private member variable.
     * @param record If true, keep track of triggered attribute changes in observers.
     * @return true, if member has changed
     */
    bool Notify (void* ppriv,bool record=false);

    /**
     * @brief Get the module type of this module.
     *
     * @return The module type: one of MOD_PULSE, MOD_ATOM, MOD_CONCAT
     */
    inline Type GetType () {return m_type;};

    /**
     * @brief Check, if the state of an observed private member changed its state.
     *
     * @param member Pointer to a key in the map m_subject_state
     * @return true, if member has changed
     */
    bool Newstate (void* member);

    /**
     * @brief Append an new observer for attribute to the attribute-observer-multimap.
     *
     * @param attrib   The observed attribute.
     * @param observer The new observer.
     */
    void AttachObserver (string attrib, ObsModAttr observer);

    /**
     * Check if the parameter map is prepared.
     * @return True, if prepared.
     */
    inline bool IsPrepared () {return m_prepared;};

    /**
     * @brief Set attribute for which standard preparation is omitted.
     *
     * @param attrib Name of the attribute.
     */
    void SetExceptionalAttrib (string attrib);

 protected:

    bool                        m_has_imag_part;       /**< @brief If symbolic expressions are complex, the imaginary part is considered */
    bool                        m_aux;                 /**< @brief auxilliary helper variable for debbuging purposes*/
    bool                        m_prepared;            /**< @brief True, after the first call to Prepare */

    double                      m_imag_part;           /**< @brief If symbolic expressions are complex, the imaginary part is stored here*/

    string                      m_name;                /**< @brief Name of this module */

    World*                      m_world;
    DOMNode*                    m_node;                /**< @brief The node configuring this Module */
    Type                        m_type;                /**< @brief The type of the module: one of MOD_PULSE, MOD_ATOM, MOD_CONCAT*/

    vector<double>              m_vector;              /**< @brief A vector which elements are accesible through loop counters.*/

    map<string,void*>           m_attrib_addr;         /**< @brief Map to connect attribute with member-pointers */
    map<string,string>          m_attrib_type;         /**< @brief Map to connect attribute with member-datatypes */
    map<string,bool>            m_attrib_xml;          /**< @brief Map indicating whether the attribute is a valid XML attribute of the framework */
    map<string,bool>            m_attrib_observable;   /**< @brief Map indicating whether this attribute is observable */
    map<string,bool>            m_attrib_dynamic;      /**< @brief Map indicating whether this attribute dynamically changes its state*/

    map<string,GiNaC::symbol>   m_attrib_symbol;       /**< @brief Map to connect attribute with a its symbol */
    map<string,string>          m_attrib_expr_str;     /**< @brief Map to connect attribute with its expression for symbolic evaluation */
    map<string,int>             m_attrib_expr_dif;     /**< @brief Map to connect attribute with its order of derivative, in case symbolic differentiation*/

    vector<Prototype*>          m_observed_modules;    /**< @brief Vector of all modules to which this module link*/
    vector<string>              m_observed_attribs;    /**< @brief Vector of all attributes names to which this module link*/
    vector<string>              m_excpetional_attribs; /**< @brief Vector of attributes which should not proceed standard preparation*/

    multimap<string,double>     m_record_double;
    multimap<string,int>        m_record_int;

    multimap<string,ObsModAttr> m_attrib_subject;      /**< @brief Multimap to link attributes with subjects  */
    multimap<string,ObsModAttr> m_attrib_observer;     /**< @brief Multimap to link attributes with observers */
    map<void*,void*>            m_subject_state;       /**< @brief Map which holds copies of the observed attributes to determine state changes */
    map<void*,string>           m_subject_type;        /**< @brief Map which holds the datatypes corresponding to the map m_subject_state */

};

#endif /*_PROTOTYPE_H_*/
