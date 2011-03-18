/** @file Module.cpp
 *  @brief Implementation of JEMRIS Module
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

#include "Module.h"
#include "Parameters.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"
#include "Pulse.h"
#include "XMLIO.h"

/***********************************************************/
Module::Module() {

	m_node          = NULL ;
	m_seq_tree      = NULL ;
	m_duration      = 0.0  ;
	m_calls         = 0    ;

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

	ATTRIBUTE("Duration", m_duration);
	//ATTRIBUTE("Observe" , NULL);		//special case of observing attributes

	return Prototype::Prepare(mode);

};

/***********************************************************/
Module* Module::GetPrototypeByAttributeValue(string name, string attrib) {
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
void           Module::AddAllDOMattributes (bool show_hidden){

	map<string,Attribute*>::iterator iter;

	for(iter = m_attributes.begin(); iter != m_attributes.end(); iter++) {

		string attrib_name = iter->first;
		Attribute* attrib = iter->second;

		//do not take hidden and unobservable attributes into account
		if (!attrib->IsObservable() && !attrib->IsPublic()) continue;
		//mark hidden attributes
		if (!attrib->IsPublic()) attrib_name +="HIDDEN";

		stringstream sstr;
		if (attrib->IsObservable()) {
			void* p = attrib->GetAddress() ;
			if (attrib->GetTypeID()==typeid(  double*).name()) sstr << (*((double*)   p));
			if (attrib->GetTypeID()==typeid(     int*).name()) sstr << (*((int*)      p));
			if (attrib->GetTypeID()==typeid(    long*).name()) sstr << (*((long*)     p));
			if (attrib->GetTypeID()==typeid(unsigned*).name()) sstr << (*((unsigned*) p));
			if (attrib->GetTypeID()==typeid(    bool*).name()) sstr << (*((bool*)     p));
			if (attrib->GetTypeID()==typeid(  string*).name()) sstr << (*((string*)   p));
		}

		//some exceptions (ugly): Constants in Analytic Pulses, Modules in DelayAtoms, NLG field terms in Gradients
		if ( attrib_name.find("Constant",0) != string::npos && !attrib->IsPublic() ) continue;
		if ( attrib_name.find("Module"  ,0) != string::npos && !attrib->IsPublic() ) continue;
		if ( attrib_name.find("NLG_"    ,0) != string::npos && !attrib->IsPublic() ) continue;

		AddDOMattribute( attrib_name, sstr.str() );
	}

	/*
	map<string, void*>::iterator it;

	for( it = m_attrib_addr.begin(); it != m_attrib_addr.end(); it++ ) {

	    map<string, bool>::iterator is_obs_attrib = m_attrib_observable.find(it->first);
        map<string, bool>::iterator is_xml_attrib = m_attrib_xml.find(it->first);

        if (is_obs_attrib->second) {

			string attrib_name = it->first;

			if (!is_xml_attrib->second) {
				if (!show_hidden) continue;
			    attrib_name +="HIDDEN";
			}

			stringstream sstr;

			void* p = it->second;
			map<string, string>::iterator type = m_attrib_type.find(it->first);


			if (IsDynamic(attrib_name)) {
				//cout << GetName() << ":" << attrib_name << " is a dynamic attribute:" << endl;
				AddDOMattribute(attrib_name, "dynamic" );
				if ( type->second == typeid(double*).name()	) {
					pair<multimap< string , double > ::iterator, multimap< string , double > ::iterator> itp;
					itp = m_record_double.equal_range(attrib_name);
					multimap< string , double >::iterator it;
					for (it=itp.first; it!=itp.second; ++it) {
						double d=it->second;
						if (attrib_name == "InitialPhase") d=fmod(d,360.0);
						sstr << d << " ";
					}
					AddDOMattribute(attrib_name+"Values", sstr.str() );
				}
				if ( type->second == typeid(int*).name()	) {
					pair<multimap< string , int > ::iterator, multimap< string , int > ::iterator> itp;
					itp = m_record_int.equal_range(attrib_name);
					multimap< string , int >::iterator it;
					for (it=itp.first; it!=itp.second; ++it) {
						sstr << it->second << " ";
					}
					AddDOMattribute(attrib_name+"Values", sstr.str() );
				}
			}
			else {
				if ( type->second == typeid(double*).name()	     ) sstr << (*((double*)       p));
				if ( type->second == typeid(float*).name()	     ) sstr << (*((float*)        p));
				if ( type->second == typeid(int*).name()	     ) sstr << (*((int*)          p));
				if ( type->second == typeid(unsigned int*).name()) sstr << (*((unsigned int*) p));
				if ( type->second == typeid(bool*).name()	     ) sstr << (*((bool*)         p));
				if ( type->second == typeid(long*).name()	     ) sstr << (*((long*)         p));
				if ( type->second == typeid(PulseAxis*).name()   ) sstr << (*((int*)          p));
				if ( type->second == typeid(string*).name()	     ) sstr << (*((string*)       p));

				if (!show_hidden)
					if (sstr.str().empty() || sstr.str()=="0")  continue;

				AddDOMattribute(attrib_name, sstr.str() );
			}

		}
	}
*/
};

/***********************************************************/
void    Module::DumpTree (string file, Module* mod,int ichild, int level) {

	//root node redirects output buffer, if filename is given
	streambuf* sobuf = 0;
	std::ofstream* pfout = 0;
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
		<< spaces_aft.str() << "                  TYPE              CLASS        NAME  duration      ADCs     TPOIs |  module specific\n"
		<< spaces_aft.str() << "                  ----------------------------------------------------------------- |  ---------------\n"
		<< "sequence-root";

 	for (int j=level; j<m_seq_tree->GetDepth(); ++j) cout << "--";
	cout << "> ";

	string class_type = mod->GetClassType();
	string name       = mod->GetName();
	transform(class_type.begin(), class_type.end(), class_type.begin(), (int(*)(int)) toupper);

	string type;
	int    adcs = 0;

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

	int    tpois = mod->GetNumOfTPOIs();
	char chform[70];
	sprintf(chform,"%8s %20s %8s %9.3f  %7d  %8d",type.c_str(),class_type.c_str(),name.c_str(),mod->GetDuration(),adcs,tpois);
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
bool           Module::WriteStaticXML (string xml_file) {

	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(StrX("Core").XMLchar() );

	if (impl==NULL) return false;

	DOMDocument* doc          = impl->createDocument( 0, StrX("PARAM").XMLchar(), 0);
	DOMNode*     topnode      = doc->getFirstChild();
    Parameters*  parameters   = m_seq_tree->GetParameters();
    DOMNode*     backup_node  = parameters->GetNode();
	XMLIO*       xmlio        = new XMLIO();

    parameters->SetNode(topnode);
    parameters->AddAllDOMattributes(false);

	if ( ((DOMElement*) topnode)->getAttributeNode(StrX("Name").XMLchar()) != NULL)
		((DOMElement*) topnode)->removeAttribute (StrX("Name").XMLchar());

    parameters->SetNode(backup_node);

	//recursively add elements
	if (!StaticDOM(doc,topnode)) return false;

	xmlio->Write (impl, topnode, xml_file);

	delete doc;
	delete topnode;
	delete parameters;
	delete backup_node;
	delete xmlio; 

	return true;

};

/***********************************************************/
bool Module::StaticDOM(DOMDocument* doc, DOMNode* node, bool append){
	bool ret = true;
/*
	DOMNode* backup_node = m_node;

	DOMElement* elem;


	//trigger the recording of sequence changes through repetition counters
	if (GetType() == MOD_CONCAT ) {
		if (append){
			elem = doc->createElement (  StrX("CONCAT").XMLchar() );
			m_node = elem;
			AddAllDOMattributes(false);
			node->appendChild(elem);
		    m_node = backup_node;
			if ( ((DOMElement*) elem)->getAttributeNode(StrX("Name").XMLchar()) != NULL)
				((DOMElement*) elem)->removeAttribute (StrX("Name").XMLchar());
		}
		ConcatSequence* cs = ((ConcatSequence*) this);
		for (int r=0;r< cs->GetMyRepetitions(); r++) {
			cs->SetRepCounter(r,true);
			vector<Module*> children = GetChildren();
			for (unsigned int j=0; j<children.size() ; ++j) {
				bool lastrep = (r+1==cs->GetMyRepetitions()) ;
		        ret = ( children[j]->StaticDOM(doc, elem, (append && lastrep) ) && ret);
			}
		}
		return ret;
	}

	if (!append) return ret;

	string class_type = GetClassType();
	transform(class_type.begin(), class_type.end(), class_type.begin(), (int(*)(int)) toupper);

	if (GetType() == MOD_ATOM  ) 	elem = doc->createElement (  StrX("ATOMIC").XMLchar() );
	if (GetType() == MOD_PULSE )	elem = doc->createElement (  StrX("PULSE").XMLchar() );
	Module* p = GetParent();

	m_node = elem;

	if (GetType() == MOD_PULSE ){
		string val="";
		if ( class_type == "HARDRFPULSE"      ) val="RF_RECT";
		if ( class_type == "SINCRFPULSE"      ) val="RF_SINC";
		if ( class_type == "GAUSSIANRFPULSE"  ) val="RF_GAUSS";
		if ( class_type == "SECHRFPULSE"      ) val="RF_SECH";
		if ( class_type == "TRAPGRADPULSE"    ) val="GR";
		if ( class_type == "EMPTYPULSE"       ) {
			if ( ( (Pulse*) this)->GetNADC() > 0 )
				val="RO";
		    else
			    val="NONE";
			//special case of empty pulses in a variable-length DelayAtom
			string p_class_type = p->GetClassType();
			transform(p_class_type.begin(), p_class_type.end(), p_class_type.begin(), (int(*)(int)) toupper);
			if (p_class_type=="DELAYATOMICSEQUENCE" && p->IsDynamic("Delay")){
				string dval = StrX(((DOMElement*) node)->getAttribute (StrX("DelayValues").XMLchar())).std_str() ;
				//cout << GetName() << " " << p_class_type << " " << dval << endl;
				AddDOMattribute("Duration","dynamic");
				AddDOMattribute("DurationValues",dval);
				((DOMElement*) node)->removeAttribute (StrX("DelayValues").XMLchar());
			}
		}
		//if ( GetClassType() == "") val="";
		AddDOMattribute("Type", val );
	}
	AddAllDOMattributes(false);

	node->appendChild(elem);

	if ( GetType() == MOD_PULSE && class_type == "EMPTYPULSE" ) {
	}


    m_node = backup_node;

	vector<Module*> children = GetChildren();
		for (unsigned int j=0; j<children.size() ; ++j)
                  ret = ( children[j]->StaticDOM(doc, elem) && ret);

	//clean up the XML: remove attributes unnecessary for IDEA
	if ( ((DOMElement*) elem)->getAttributeNode(StrX("Name").XMLchar()) != NULL)
		((DOMElement*) elem)->removeAttribute (StrX("Name").XMLchar());
	if (GetType() == MOD_ATOM  ) {
		map<string, void*>::iterator it;
		for( it = m_attrib_addr.begin(); it != m_attrib_addr.end(); it++ ) {
			if ( ((DOMElement*) elem)->getAttributeNode(StrX(it->first).XMLchar()) != NULL)
			((DOMElement*) elem)->removeAttribute (StrX(it->first).XMLchar());
		}
	}


*/
	return ret;
};

