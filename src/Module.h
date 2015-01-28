/** @file Module.h
 *  @brief Implementation of JEMRIS Module
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

#ifndef MODULE_H_
#define MODULE_H_


#include "Prototype.h"

// Declaration to avoid inclusion of the header resulting in circular referentation
class SequenceTree;
class Parameters;
class ConcatSequence;


//! Module super class! ABC for all Objects in the sequence framework

class Module : public Prototype {

 public:

    /**
     * @brief Constructor
     *
     * Default constructor
     */
    Module ();

    /**
     * @brief Destructor.
     *
     * Default destrcutor
     */
    virtual ~Module () {};

    /**
     * @brief Copy constructor.
     *
     * Default copy constructor.
     */
    Module (const Module&);

    /**
     * @brief Clone a module.
     *
     * Create and recieve a Clone of an unprepared module.
     */
    virtual Module* Clone() const = 0;

    /**
     * @brief Get the value of this element.
     *
     * Calculate the value of the particular pulse at a given time in a given
     * cycle and put the values put the values in dAllVal.
     * Successors must implements this method.
     *
     * @param dAllVal is an array of 5 doubles. {amplitude, phase, Gx, Gy, Gz}
     *                Gx, Gy, Gz equal to 1.0 for the direction of the Axis. 0.0 else.
     * @param time    constant double value of the time of invocation.
     */
    virtual void GetValue (double * dAllVal,   double const time) {};

    /*
     * @brief Get values for available Transmit coils for simulation only.
     *
     * Get the values for each Transmit channel in this atomic sequence. And
     * return the in dAllVal.
     *
     * @param dAllVal represents the pointer to array where results are returned.
     * @param time    represents the time of invocation.
     * @param pos     represents the position in x, y, z.
     */
    //virtual void GetValue  (double * dAllVal, double const time, double * pos[3]) {};

    /**
     * @brief Prepare this pulse.
     *
     * Called twice when root node prepares. First time not verbose.
     * Successors must overload this method.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool Prepare  (const PrepareMode mode);


    /**
     * @brief Initialize this module.
     *
     * The first step after cloning!
     * The method sets the Name of the Module, and pointers to
     * the referring node and the (unique) SequenceTree.
     *
     * @param node The DOMNode referring to this module.
     */
    void Initialize  (DOMNode* node );

    /**
     * @brief Get Parent
     *
     * @return Parent module.
     */
    Module*         GetParent   ();

    /**
     * Get the duration of this module.
     *
     * @return Duration in ms
     */
    virtual double GetDuration () = 0;

    /**
     * @brief Get the Number of TPOIs of this module.
     *
     * @return The size of the member m_tpoi
     */
    virtual int     GetNumOfTPOIs () { return m_tpoi.GetSize(); };

    /**
     * @brief Get Children
     *
     * @return Vector of child modules.
     */
    vector<Module*> GetChildren () const;

    /**
     * @brief Get Child
     *
     * @param position The position in the list of children.
     */
    Module*         GetChild    (unsigned int position) const;

    /**
     * @brief Get number of Children
     *
     * @return Number of child modules.
     */
    int GetNumberOfChildren () const;

    /**
     * @brief Insert Child
     *
     * @param name The name of the module from the ModulePrototypeFactory
     * @return Success
     */
    bool InsertChild (const string& name);

	virtual Module* GetPrototypeByAttributeValue (string name, string attrib);

    /**
     * @brief Add attribute and value to the DOM node of this module
     *
     * @param attribute  The new attribute of the DOMNode
     * @param value      The corresponding value
     * @return           False, if attribute already exists in DOM (XML)
     */
    bool AddDOMattribute    (const string attribute, const string value);

	/**
	 * More elaborate desciption here please
 	 */
    void AddAllDOMattributes (bool show_hidden = true);

    /**
     * @brief get the TPOIs of this module
     *
     * @return      Pointer to the private member TPOI.
     */
     virtual TPOI* GetTPOIs () { return &m_tpoi; }

    /**
     * @brief Dump the sequence tree.
     *
     * @param file   Optional output file. If, NULL stdout is used.
     * @param mod    The starting module from which the tree is dumped.
     * @param ichild More elaborate description
     * @param level  More elaborate description
     */
      void  DumpTree (const string& file="", Module* mod=NULL, int ichild = 0, int level = 0) ;

      /**
       * @brief Return tree depth from this module (including depth of Containers)
       *
       * @param depth  the depth so far
       */
        int  GetDepth (int depth = 0) ;

    /**
     * @brief Rewrite XML-tree where all expressions are evaluated (for IDEA)
     *
     * @param xml_file name of XML file
     * @return success or failure
     */
      bool WriteStaticXML(const string& xml_file);

    /**
     * @brief Create a DOM tree where all expressions are evaluated (for IDEA)
     *
     * @param doc    Document
     * @param node   Node
     * @param append Append to node?
     *
     * @return success or failure
     */
     bool StaticDOM(DOMDocument* doc, DOMNode* node, bool append = true);


     void			  	SetSeqTree (SequenceTree* pST)	{ m_seq_tree= pST;	};
     SequenceTree*  	GetSeqTree (				 )	{ return m_seq_tree;};


 protected:

    /**
     * @brief Get informations on this module
     *
     * @return Some useful information of a specific module
     */
    virtual string GetInfo () { return ""; };

    SequenceTree*  m_seq_tree;    /**< @brief Reference to the sequence tree. */
    Parameters*    m_parameters;  /**< @brief Pointer to the sole instance of the Parameters*/
    TPOI           m_tpoi;        /**< @brief Time points of interest are stored the referred repository.*/

    string         m_info;        /**< @brief Information string for this module */
    double         m_duration;    /**< @brief The duration of this module*/
    int            m_calls;       /**< @brief Number of calls of this module*/
 private:


};

#endif
