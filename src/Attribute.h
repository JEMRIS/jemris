/** @file Attribute.h
 *  @brief Implementation of Attribute
 *
 * Author: tstoecker
 * Date  : Mar 18, 2009
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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


#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include "Declarations.h"

#include     <stdexcept>
#include     <map>
#include     <vector>
#include     <typeinfo>
#include     <cmath>

#include     "StrX.h"
#include     <ginac/ginac.h>
#include     <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

//declaration of the Prototype
class Prototype;

/**
 * @brief Attribute class. Attributes are private member variables of a Prototype which are
 * accessible through XML and, thus, they are subject to observation mechanism.
 */
class Attribute {

 public:

    /**
     * @brief Constructors: must provide name (XML), Prototype, public/observable status, and the member variable
     *
     */
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const    double& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const       int& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const      long& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const  unsigned& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const      bool& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const    string& val) { Initialize(name,proto,pub,obs,val); };
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs, const PulseAxis& val) { Initialize(name,proto,pub,obs,val); };

    /**
     * @brief Constructor for an unobservable Attribute which does not represent a member variable
     *
     */
    Attribute (const string& name, Prototype* proto, const bool& pub, const bool& obs) {
        m_public        = true;
        m_observable    = false;
        m_dynamic       = false;
        m_name  		= name;
        m_prototype     = proto;
        m_symbol_name   = "NA";
        m_formula	    = "NA";
        m_ginac_excomp  = false;
        m_num_fp		= -1;
        m_cur_fp		= -1;
        m_diff 			= -1;
        m_sym_diff      = "NA";
        m_complex       = false;
        m_imaginary		= 0.0;
        m_address       = NULL;
    	m_datatype      = "";
    	m_backup        = NULL;

    }

    /**
     * @brief Destructor deletes local copies of Prototype member variables.
     *
     */
    virtual ~Attribute ();

    /**
     * @brief Check, if this attribute is public for manipulation through XML.
     *
     * @return true/false
     */
    bool IsPublic() const { return m_public;  }

    /**
     * @brief Set this attribute to be public for manipulation through XML.
     *
     * @param val The state of being public.
     */
    void SetPublic(bool val)    { m_public = val; }

        /**
     * @brief Check, if this attribute is observable by other attributes.
     *
     * @return true/false
     */
    bool IsObservable() const    { return m_observable; }

    /**
     * @brief Set this attribute to be observable by other attributes.
     *
     * @param val The state of being observable.
     */
    void SetObservable(bool val)    { m_observable = val; }

    /**
     * @brief Check, if this attribute is dynamic. This is needed for exporting
     * static XML Files of the Sequence.
     *
     * @return true/false
     */
    bool IsDynamic() const    { return m_dynamic; }

    /**
     * @brief Set this attribute to be dynamic.
     *
     * @param val The state of being dynamic.
     */
    void SetDynamic(bool val)    { m_dynamic = val; }

    /**
     * @brief  Get the number of symbolic differentiations of the attribute's expression.
     *
     * @return The order of the derivative
     */
    int GetDiff()    { return m_diff; }

    /**
     * @brief  Get the imaginary part the attribute's evaluation.
     *
     * @param val The imaginary part
     */
    double GetImaginary()    { return m_imaginary; }

    /**
     * @brief Check, if this attribute's evaluation is complex.
     *
     * @return true/false
     */
    bool IsComplex() const    { return m_complex; }

    /**
     * @brief  Set the number of symbolic differentiations of the attribute's expression.
     *
     * @param val The order of the derivative
     */
    void SetDiff(int val=0, string sym="diff")    { m_diff = val; m_sym_diff = sym; }

    /**
     * @brief Get the name of the attribute.
     *
     * @return The name of the attribute.
     */
    string GetName() const    { return m_name; }

    /**
     * @brief Get the type ID of the attribute.
     *
     * @return The type ID of the attribute.
     */
    string GetTypeID() const   { return m_datatype; }

    /**
     * @brief Get the pointer to the value represented by this attribute.
     *
     * @return The pointer to the value of the attribute.
     */
    void* GetAddress() const    { return m_address; }

    /**
     * @brief Get the pointer to the Prototype to which this attribute belongs.
     *
     * @return The pointer to the Prototype.
     */
    Prototype*		GetPrototype() const    { return m_prototype; }

    /**
     * @brief Get the GiNaC symbol of this attribute.
     *
     * return the GiNaC symbol
     */
    string	GetSymbol() { return m_symbol_name; };

    /**
     * @brief Set the Prototype's private member represented by this attribute.
     * The function performs the following tasks:
     * - write a value to the Prototype's private member, in case of non-dynamic expressions
     * - add all observed attributes which are necessary for GiNaC evaluation
     * - set the GiNaC expression to calculate the value for the Prototype's private member
     * - test once, if evaluation is possible
     *
     * @param expr			the attribute value as it is read from XML
     * @param obs_attribs	list of attributes observed by the Prototype
     * @param verbose		if true, warnings will be dumped to stdout
     * @return				success/failure of operation
     */
    bool SetMember (string expr, const vector<Attribute*>& obs_attribs, bool verbose = false);

    /**
     * @brief Evaluate the GiNaC expression of this attribute
     *
     * The evaluation depends on the current state of all observed attributes.
     * It is written to the Prototype's private member represented by this attribute.
     * Afterwards, attributes observing this attribute are notified.
     */
    void EvalExpression ();

    /**
     * @brief Evaluate the compiled GiNaC expression of this attribute
     *
     * At first call, performs runtime compilation of the GiNaC expression.
     * Then, compiled function evaluation is returned. Runtime compilation
     * is repeated, if the expression changes according to notification of
     * observed attributes in the expression.
     * Attention:
     *   1) Only to be used for attributes of type double.
     *   2) The evaluation is NOT written to the Prototype's private member
     *      represented by this attribute.
     *   3) Attributes observing this attribute are NOT notified.
     *   4) Falls back to slow analytic evaluation, if external compilation fails.
     *
     * @param val		function input value
     * @param attrib	attribute representing the function input value
     * @return			expression evaluation
     */
    double EvalCompiledExpression (double const val, string const attrib );

    /**
     * @brief True, if GiNaC external compiler is available on this system.
     */
    bool	HasGinacExCompiler(){return	m_ginac_excomp;};

    /**
     * @brief  Return the total number of function pointers.
     */
    int		GetNumberFunctionPointers(){return m_num_fp;};

    /**
     * @brief Return the counter to the current function pointer.
     */
    int		GetCurrentFunctionPointer  (){ 	return m_cur_fp; };

    /**
     * @brief Set the counter to the current function pointer to zero.
     */
    void	ResetCurrentFunctionPointer(){  m_cur_fp=0; };

    /**
     * @brief Increase the counter to the current function pointer by one.
     */
    void	StepCurrentFunctionPointer (){	m_cur_fp = (m_cur_fp+1>m_num_fp)?m_num_fp:m_cur_fp+1; };


    /* pure template Functions need header implementation! */

    /**
     * @brief Notify all observers
     *
     * @return true, if notification took place.
     */
    template <typename T> bool Notify (const T& val) {

    	if ( !NewState(val) ) return false;

    	//initiate re-evaluation and preparation of the observers
        for (unsigned int i=0; i<m_observers.size(); i++) {

        	//increase the counter for the observer's function pointer
        	if (m_observers.at(i)->GetNumberFunctionPointers()>0) {
        		m_observers.at(i)->StepCurrentFunctionPointer(  );
        	}

        	m_observers.at(i)->EvalExpression();
            UpdatePrototype( m_observers.at(i)->GetPrototype() );
            //cout << "DEBUG " << GetName() << " notified " << m_observers.at(i)->GetName() << endl;
        }

    	return true;
    };

    /**
     * @brief Append a new observer of this attribute.
     *
     * @param attr   The observing attribute.
     */
    void AttachObserver (Attribute* attrib);

    /**
     * @brief Append a new subject observed by this this attribute.
     *
     * @param attr   The observed attribute.
     */
    void AttachSubject (Attribute* attrib);


 private:

    /**
     * @brief Update a Prototype which holds an observing Attribute
     *
     * @param prot the Prototype to update
     */
    void UpdatePrototype (Prototype* prot);


    /* pure template Functions need header implementation */

    /**
     * @brief Check the state of the value represented by this attribute.
     *
     * @param the value to ckeck
     * return true,  if the value is new
     */
    template <typename T> bool NewState(const T& val) {
    	if ( *((T*) m_backup) == val ) return false;
    	*((T*) m_backup) = val;
    	return true;
    };

   /**
     * @brief write the member variable of this attribute
     *
     * @param val the new value
     */
    template <typename T> void WriteMember (const T& val) {
    	*((T*) m_address) = val;
    	Notify(val);
    };

    /**
     * @brief get the member variable of this attribute
     *
     * @return the value of the member variable
     */
    template <typename T> T GetMember (){
    	T d = *((T*) m_address);
    	if (isnan(d)) return 0;
    	return d;
    };

    /**
     * @brief Initialize private members.
     *
     * @param name the XML name
     * @param proto the Prototype which holds this Attribute
     * @param pub public status
     * @param obs observable status
     * @param val the member variable
     */
	template <typename T> void Initialize(const string& name, Prototype* proto, const bool& pub, const bool& obs, const T& val){
        m_public        = pub;
        m_observable    = obs;
        m_dynamic       = false;
        m_name  		= name;
        m_prototype     = proto;
        m_symbol_name   = "x";
        m_formula	    = "";
        m_ginac_excomp  = true;
        m_compiled.push_back(false);
        m_num_fp		= 0;
        m_cur_fp		= 0;
        m_diff 			= 0;
        m_sym_diff      = "diff";
        m_complex       = false;
        m_imaginary		= 0.0;
        m_address       = ((void*) &val);
    	m_datatype      = typeid(T*).name();
    	T* p            = new T(val);
    	m_backup        = (void*) p;
};

	bool       		m_public;		/**< @brief Indicating whether the attribute is accessible through XML. */
	bool       		m_observable;	/**< @brief Indicating whether the attribute is observable. */
	bool       		m_dynamic;		/**< @brief Indicating whether the attribute dynamically changes its value in runtime. */
	string     		m_name;			/**< @brief Name of the attribute (in XML).*/
	void*      		m_address;		/**< @brief Pointer to the Prototype member variable, which is represented by this attribute.  */
	string     		m_datatype;		/**< @brief Type of the Prototype member variable, which is represented by this attribute.  */
	void*      		m_backup;		/**< @brief Backup value of the Prototype member variable, which is represented by this attribute.  */
    Prototype*		m_prototype;	/**< @brief Pointer to the Prototype object, which instantiated this attribute.  */
    string			m_symbol_name;	/**< @brief GiNaC symbol name of the attribute.*/
    string			m_sym_diff;		/**< @brief GiNaC symbol name for symbolic derivative.*/
	string     		m_formula;		/**< @brief Mathematical formula of the attribute (in XML) for GiNaC evaluation.*/
	GiNaC::ex       m_expression;	/**< @brief GiNaC Mathematical expression of the attribute */
	GiNaC::lst		m_symlist;		/**< @brief GiNaC list of all symbols involved in the calculation.*/
	bool			m_ginac_excomp;	/**< @brief True, if GiNaC external compiler is available on this system.*/
	int				m_num_fp;		/**< @brief Number of GiNaC expression function pointers owned by this attribute.*/
	int				m_cur_fp;		/**< @brief Current GiNaC expression function pointer.*/
	vector<bool>	m_compiled;		/**< @brief True, if GiNaC expression is compiled successfully in run time.*/
	vector<GiNaC::FUNCP_1P> m_fp;	/**< @brief Function pointers to GiNaC expression evaluation.*/
	vector<GiNaC::FUNCP_1P> m_fpi;	/**< @brief Function pointers to GiNaC expression evaluation of imaginary part.*/
	int				m_diff;			/**< @brief Number of symbolic differentiations of the attribute's expression.*/
	bool            m_complex;      /**< @brief If symbolic expressions are complex, the imaginary part is considered */
    double          m_imaginary;    /**< @brief The imaginary part of complex expression evaluation.*/
    vector<Attribute*> m_subjects;  /**< @brief Vector of attributes under observation by this attribute */
    vector<Attribute*> m_observers; /**< @brief Vector of attributes observing this attribute  */
};

#endif /* ATTRIBUTE_H_ */
