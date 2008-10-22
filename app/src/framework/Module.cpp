/** @file Module.cpp
 *  @brief Implementation of JEMRIS Module
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

#include "Module.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"
#include "Pulse.h"

/***********************************************************/
Module::Module() {

	m_node          = NULL ;
	m_seq_tree      = NULL ;
	m_duration      = 0.0  ;
	m_imag_part     = 0.0  ;
	m_has_imag_part = false;
	m_world         = World::instance();

};

/***********************************************************/
Module::~Module() {

	map<void*,void*>::iterator istate;

	for( istate = m_subject_state.begin(); istate != m_subject_state.end(); istate++ ) 	{

	    //delete all copies of subject state variables
		map<void*,string>::iterator itype  = m_subject_type.find(istate->first);
		if ( itype->second == typeid(double*).name()      )	delete ((double*)       istate->second);
		if ( itype->second == typeid(int*).name()         )	delete ((int*)          istate->second);
		if ( itype->second == typeid(unsigned int*).name()) delete ((unsigned int*) istate->second);
		if ( itype->second == typeid(float*).name()       )	delete ((float*)        istate->second);
		if ( itype->second == typeid(long*).name()        )	delete ((long*)         istate->second);
		if ( itype->second == typeid(string*).name()      )	delete ((string*)       istate->second);
		if ( itype->second == typeid(PulseAxis*).name()   ) delete ((PulseAxis*)    istate->second);

	}

};

/***********************************************************/
void            Module::Initialize (DOMNode* node) {

    //if node has no "Name" attribute, insert it and use the node name for its value
	string s     = StrX(((DOMElement*) node)->getAttribute (StrX("Name").XMLchar() )).std_str() ;
   	if (s.empty())
		((DOMElement*) node)->setAttribute(StrX("Name").XMLchar(),node->getNodeName());

    //set the module name to the node name as well
	SetName( StrX(node->getNodeName()).std_str() );

	//set the instances of this node and the sequence tree
	m_node       = node;
	m_seq_tree   = SequenceTree::instance();
	m_parameters = m_seq_tree->GetParameters();

}

/***********************************************************/
bool Module::Prepare  (PrepareMode mode){

	ATTRIBUTE("Duration", &m_duration);
	ATTRIBUTE("Observe" , NULL);		//special case of observing attributes

	return Prototype::Prepare(mode);

};

/***********************************************************/
Module* Module::GetModuleByAttributeValue(string name, string attrib) {
	return m_seq_tree->GetModuleByAttributeValue(name,attrib);
}

/***********************************************************/
Module*         Module::GetParent () {

	if (m_seq_tree==NULL || m_node==NULL) return NULL;
	return m_seq_tree->GetParent(m_node);

}

/***********************************************************/
vector<Module*> Module::GetChildren () {

	if (m_seq_tree==NULL || m_node==NULL) return vector<Module*>() ;
	return m_seq_tree->GetChildren(m_node);

}

/***********************************************************/
Module*         Module::GetChild (unsigned int position) {

	if (m_seq_tree==NULL || m_node==NULL) return NULL;
	return m_seq_tree->GetChild(m_node, position);

}

/***********************************************************/
int             Module::GetNumberOfChildren () {

    vector<Module*> vc=GetChildren();
    return vc.size();

};

/***********************************************************/
bool            Module::InsertChild (string name){

	if ( m_seq_tree==NULL || GetNode()==NULL) return false;

	DOMElement* node = m_seq_tree->GetDOMDocument()->createElement(StrX(name).XMLchar());
	if (node==NULL) return false;

	Module* mod = m_seq_tree->GetMPF()->CloneModule(node);
	if (mod==NULL)  return false;

	GetNode()->appendChild (node);
	m_seq_tree->GetModuleMap()->insert(pair<DOMNode*, Module*> (node, mod));
	mod->Initialize(node);

	return true;

};

/***********************************************************/
bool            Module::AddDOMattribute    (const string attribute, const string value){

	if (HasDOMattribute(attribute))
	    return false;

	((DOMElement*) m_node)->setAttribute( StrX(attribute).XMLchar(), StrX(value).XMLchar() );

	return true;

};

/***********************************************************/
void           Module::AddAllDOMattributes (){

	map<string, void*>::iterator it;

	for( it = m_attrib_addr.begin(); it != m_attrib_addr.end(); it++ ) {

	    map<string, bool>::iterator is_obs_attrib = m_attrib_observable.find(it->first);
        map<string, bool>::iterator is_xml_attrib = m_attrib_xml.find(it->first);

        if (is_obs_attrib->second) {

			string attrib_name = it->first;

			if (!is_xml_attrib->second)
			    attrib_name +="HIDDEN";

			stringstream sstr;

			void* p = it->second;
			map<string, string>::iterator type = m_attrib_type.find(it->first);

			if ( type->second == typeid(double*).name()	     ) sstr << (*((double*)       p));
			if ( type->second == typeid(float*).name()	     ) sstr << (*((float*)        p));
			if ( type->second == typeid(int*).name()	     ) sstr << (*((int*)          p));
			if ( type->second == typeid(unsigned int*).name()) sstr << (*((unsigned int*) p));
			if ( type->second == typeid(bool*).name()	     ) sstr << (*((bool*)         p));
			if ( type->second == typeid(long*).name()	     ) sstr << (*((long*)         p));
			if ( type->second == typeid(PulseAxis*).name()   ) sstr << (*((int*)          p));
			if ( type->second == typeid(string*).name()	     ) sstr << (*((string*)       p));

			AddDOMattribute(attrib_name, sstr.str() );

		}
	}
};

/***********************************************************/
void    Module::DumpTree (string file, Module* mod,int ichild, int level) {

	//root node redirects output buffer, if filename is given
	streambuf* sobuf;
	std::ofstream* pfout;
	if (mod ==NULL) {

	    mod=this;

		if (!file.empty()) {

			sobuf = cout.rdbuf();			// Save the original stdout buffer.
			pfout = new ofstream(file.c_str());

			if (*pfout) cout.rdbuf(pfout->rdbuf()); // Redirect cout output to the opened file.

		}

	}

	//Dump info on this module
    vector<Module*> children = mod->GetChildren();
	stringstream spaces_bef;
    for (int j=0;j<level;++j) spaces_bef << "  ";
	stringstream spaces_aft;
 	for (int j=level; j<m_seq_tree->GetDepth(); ++j) spaces_aft << "  ";

    if (ichild)	cout	<< spaces_bef.str() << "|_ child " << ichild << "   ";
    else		cout	<< "dump of sequence tree\n"
		<< spaces_aft.str() << "                  TYPE              CLASS        NAME  duration      ADCs |  module specific\n"
		<< spaces_aft.str() << "                  ------------------------------------------------------- |  ---------------\n"
		<< "sequence-root";

 	for (int j=level; j<m_seq_tree->GetDepth(); ++j) cout << "--";
	cout << "> ";

	string class_type = mod->GetClassType();
	string name       = mod->GetName();
	transform(class_type.begin(), class_type.end(), class_type.begin(), (int(*)(int)) toupper);

	string type;
	int    adcs;

	if (mod->GetType() == MOD_CONCAT) {
		type="CONCAT";
		adcs=((Sequence*) mod)->GetNumOfADCs();
	}

	if (mod->GetType() == MOD_ATOM) {
		type="ATOM";
		adcs=((Sequence*) mod)->GetNumOfADCs();
	}

	if (mod->GetType() == MOD_PULSE) {
		type="PULSE";
		adcs=((Pulse*) mod)->GetNADC() ;
	}

	char chform[60];
	sprintf(chform,"%8s %20s %8s %9.3f  %7d",type.c_str(),class_type.c_str(),name.c_str(),mod->GetDuration(),adcs);
	cout << chform << "  | " << mod->GetInfo() << "\n";

    level++;

    for (unsigned int i=0; i<children.size() ; ++i)
        DumpTree(file, mod->GetChild(i),i+1,level);

	// root node restores cout to original state.
	if (ichild == 0 && !file.empty() ) {
		cout.rdbuf(sobuf);
		delete pfout;
	}

};

/***********************************************************/
void           Module::HideAttribute (string attrib, bool observable){

	map<string,bool>::iterator it1 = m_attrib_xml.find(attrib);
	if (it1 != m_attrib_xml.end()) it1->second=false;

	map<string,bool>::iterator it2 = m_attrib_observable.find(attrib);
	if (it2 != m_attrib_observable.end()) it2->second=observable;

};

