/** @file Container.cpp
 *  @brief Implementation of JEMRIS Container
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

#include "Container.h"
#include "ContainerSequence.h"
#include "SequenceTree.h"

/***********************************************************/
Container::Container  (const Container& as) {
	m_container_seq_name = "";
	m_container_seq = NULL;
	m_container_seqtree = NULL;
	for (int i=0;i<5;i++)	{
		m_import[i]  = 0.0;
		m_export[i]  = 0.0;
	}
}

/***********************************************************/
Container::~Container  () {
	if (m_container_seqtree !=NULL) delete m_container_seqtree;
}

/***********************************************************/
bool Container::Prepare (const PrepareMode mode) {

	if (mode != PREP_UPDATE) m_type = MOD_CONTAINER;

    bool b=true;
    ATTRIBUTE("Filename", m_container_seq_name);

    // attributes for import (Imp1, Imp2, ...) into the ContainerSequence
	for (int i=0;i<5;i++)	{
		stringstream a; a << "Imp" << i+1;
	    ATTRIBUTE(a.str(), m_import[i]);
	}

    //hidden attributes to link with export-attributes (Exp1, Exp2, ...) of the ContainerSequence
    for (int i=0;i<5;i++)	{
		stringstream a; a << "Exp" << i+1;
	    HIDDEN_ATTRIBUTE(a.str(), m_export[i]);
	}

    //the number of export-attributes provided by the ContainerSequence (only for validation)
    UNOBSERVABLE_ATTRIBUTE("NumExpAttribs");
    int num_export_attribs = 0;
 	map<string,Attribute*>::iterator iter;
	for(iter = m_attributes.begin(); iter != m_attributes.end(); iter++) {
	    string     keyword   = iter->first;
	    Attribute* attribute = iter->second;
	    string val = GetDOMattribute(keyword);
	    if (!val.empty()  && keyword == "NumExpAttribs")
	    	num_export_attribs=atoi(val.c_str());
	}

    //read ContainerSequence and build up its sequence tree
    if (m_container_seq==NULL && !m_container_seq_name.empty() ) {
    	m_container_seqtree = new SequenceTree;
    	m_container_seqtree->Initialize(m_container_seq_name);
    	m_container_seqtree->Populate();
    	m_container_seq = m_container_seqtree->GetContainerSequence();
    	m_container_seq->SetContainer(this);
    	cout << "Info: Container '" << GetName() << "' inserted ContainerSequence '"
    		 << m_container_seq->GetName() << "' from file " << m_container_seq_name << endl << endl;
    }

    if (m_container_seq!=NULL) {
    	b = m_container_seq->Prepare(mode);

    	//observe the export-attributes (Exp1, Exp2, ...) of the ContainerSequence
    	unsigned counter=0;
    	for(iter = m_container_seq->m_attributes.begin(); iter != m_container_seq->m_attributes.end(); iter++) {
    	    string     keyword   = iter->first;
    	    Attribute* attribute = iter->second;
    	    if ( attribute->IsObservable() && keyword.substr(0,3)=="Exp" &&
    	    	!attribute->GetFormula().empty() &&
    	    	 attribute->GetTypeID()!=typeid(std::string*).name() ) {
    	    	map<string,Attribute*>::iterator exp = m_attributes.find(keyword);
    	    	if (exp != m_attributes.end() ) {
    	    		counter++;
    	    		stringstream Exp; Exp << "Exp" << counter;
    	    		if (Exp.str() != keyword && mode==PREP_VERBOSE) {
    	    			cout << GetName() << " Warning: export attributes not in correct order." << endl;
    	    			b = false;
    	    		}
    	            for(unsigned int i = 0; i < m_obs_attribs.size(); i++)
    	                if ( m_obs_attribs.at(i) == attribute ) m_obs_attribs.erase(m_obs_attribs.begin()+i);
    	    		m_obs_attribs.push_back(attribute);
    	    		stringstream formula; formula << "a" << m_obs_attribs.size();
    	    		b = ( exp->second->SetMember(formula.str(), m_obs_attribs, mode == PREP_VERBOSE) && b);
    	    	}
    	    }
    	}
    	if (counter != num_export_attribs && mode == PREP_VERBOSE ) {
    		b = false;
    		cout << "Warning in " << GetName() << ": Number of expected ("<< num_export_attribs
    			 << ") and actual (" << counter << ") export attributes differ" << endl;
    	}

    }

    b = ( Sequence::Prepare(mode) && b);

    HideAttribute ("Vector",false);
    HideAttribute ("Aux1",false);
    HideAttribute ("Aux2",false);
    HideAttribute ("Aux3",false);

    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of Container '" << GetName() << "' not successful. " << endl;

    return b;

}

/***********************************************************/
inline double Container::GetDuration () {

	if (m_container_seq==NULL) return 0.;

	m_duration = m_container_seq->GetDuration();

	Notify(m_duration);

	return m_duration;

}

/***********************************************************/
void  Container::GetValue (double * dAllVal, double const time) {

	if (m_container_seq==NULL) return;

	m_container_seq->GetValue(dAllVal,time);

}

/***********************************************************/
int  Container::GetNumOfTPOIs () {

	if (m_container_seq==NULL) return 0;

	return m_container_seq->GetNumOfTPOIs();

}

/***********************************************************/
long  Container::GetNumOfADCs () {

	if (m_container_seq==NULL) return 0;

	return m_container_seq->GetNumOfADCs();

}

/***********************************************************/
void Container::CollectSeqData(NDData<double>& seqdata, double& t, long& offset) {

	if (m_container_seq==NULL) return;

	m_container_seq->CollectSeqData(seqdata, t, offset);

}

/***********************************************************/
string          Container::GetInfo () {

	string ret = "ContainerSequence filename: " + m_container_seq_name;

	return ret;

}

