/** @file ContainerSequence.cpp
 *  @brief Implementation of JEMRIS ContainerSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#include "ContainerSequence.h"
#include "Container.h"

/***********************************************************/
ContainerSequence::ContainerSequence  (const ContainerSequence& as) {

	m_container = NULL;
	for (int i=0;i<10;i++)	{
		m_import[i]  = 0.0;
		m_export[i]  = 0.0;
	}

}

/***********************************************************/
bool ContainerSequence::Prepare (const PrepareMode mode) {

    bool b=true;

    //the ContainerSequence must not loop
    m_repetitions = 1;
    m_counter = 0;

    //hidden attributes to link with import-attributes (Imp1, Imp2, ...) of the Container
    for (int i=0;i<9;i++)	{
		stringstream a; a << "Imp" << i+1;
	    HIDDEN_ATTRIBUTE(a.str(), m_import[i]);
		stringstream b; b << "Info_Imp" << i+1;
	    UNOBSERVABLE_ATTRIBUTE(b.str());
	}


    // attributes for export (Exp1, Exp2, ...) into the Container
	for (int i=0;i<3;i++)	{
		stringstream a; a << "Exp" << i+1;
	    ATTRIBUTE(a.str(), m_export[i]);
		stringstream b; b << "Info_Exp" << i+1;
		UNOBSERVABLE_ATTRIBUTE(b.str());
	}

 	//observe the import-attributes (Imp1, Imp2, ...) of the Container
    if ( m_container != NULL ) {
    	unsigned counter=0;
     	map<string,Attribute*>::iterator iter;
    	for(iter = m_container->m_attributes.begin(); iter != m_container->m_attributes.end(); iter++) {
    	    string     keyword   = iter->first;
    	    Attribute* attribute = iter->second;

    	    if ( attribute->IsObservable() && keyword.substr(0,3)=="Imp" &&
    	    	!attribute->GetFormula().empty() &&
    	    	 attribute->GetTypeID()!=typeid(std::string*).name() ) {

    	    	map<string,Attribute*>::iterator imp = m_attributes.find(keyword);
    	    	if (imp != m_attributes.end() ) {
    	    		counter++;
    	    		stringstream Imp; Imp << "Imp" << counter;
    	    		if (Imp.str() != keyword ) { //&& mode==PREP_VERBOSE) {
    	    			cout << GetName() << " Warning: import attributes not in correct order." << endl;
    	    			b = false;
    	    		}
    	            for(unsigned int i = 0; i < m_obs_attribs.size(); i++)
    	                if ( m_obs_attribs.at(i) == attribute ) m_obs_attribs.erase(m_obs_attribs.begin()+i);
    	    		m_obs_attribs.push_back(attribute);
    	    		string formula=keyword;
    	            for(unsigned int i = 0; i < m_obs_attrib_keyword.size(); i++)
    	                if ( m_obs_attrib_keyword.at(i) == formula ) m_obs_attrib_keyword.erase(m_obs_attrib_keyword.begin()+i);
    	            m_obs_attrib_keyword.push_back(formula);
    	            if (m_obs_attribs.size() != m_obs_attrib_keyword.size())
    	            cout << "!!! " << GetName() << ": " << m_obs_attribs.size() << " , " << m_obs_attrib_keyword.size() << endl;
    	    		b = ( imp->second->SetMember(formula, m_obs_attribs, m_obs_attrib_keyword, mode == PREP_VERBOSE) && b);
    	    	}
    	    }
    	}
    }



    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of ContainerSequence '" << GetName() << "' not successful. " << endl;

    b = ( ConcatSequence::Prepare(mode) && b);

    HideAttribute ("Repetitions",false);
    HideAttribute ("Counter",false);
    HideAttribute ("Vector",false);
    HideAttribute ("Aux1",false);
    HideAttribute ("Aux2",false);
    HideAttribute ("Aux3",false);

    return b;

}

/***********************************************************/
string          ContainerSequence::GetInfo () {

	string ret;

	return ret;

}
