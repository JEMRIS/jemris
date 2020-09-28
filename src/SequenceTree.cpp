/** @file SequenceTree.cpp
 *  @brief Implementation of JEMRIS SequenceTree
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
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

#include "SequenceTree.h"
#include "Module.h"
#include "World.h"
#include "Parameters.h"
#include "ConcatSequence.h"
#include "ContainerSequence.h"
#include "AtomicSequence.h"
#include "Pulse.h"
#include "XMLIO.h"

/***********************************************************/
SequenceTree::SequenceTree() {

	m_dom_doc    = 0;
	m_parameters = 0;
	m_root_seq   = 0;
	m_state      = false;
	m_depth      = 0;
	m_mpf        = new ModulePrototypeFactory();
	m_xio        = new XMLIO();

}

/***********************************************************/
SequenceTree::~SequenceTree() {

	delete m_xio;

	XMLPlatformUtils::Terminate();

	// Delete all Modules
	map<DOMNode*,Module*>::iterator iter;
	for (iter = m_Modules.begin(); iter != m_Modules.end(); iter++ )
		delete iter->second;

	//delete m_mpf; //TMPTMP deleting the mpf causes trouble in some cases (?)

}


/***********************************************************/
void SequenceTree::Initialize(string seqFile) {

    //initialize
	m_state   = false;
	m_seq_file = seqFile;
   	m_dom_doc = m_xio->Parse(seqFile);

	DOMNodeList* dnl = m_dom_doc->getElementsByTagName( StrX("AtomicSequence").XMLchar() );

	DOMNode* topnode;

	if (!(topnode = m_dom_doc->getFirstChild()))
	    return;

	m_state   = ( StrX(topnode->getNodeName()).std_str() == "Parameters");

}

/***********************************************************/
string  SequenceTree::GetSequenceDirectory() {
	return m_seq_file.substr(0, m_seq_file.find_last_of("\\/")+1);
}

/***********************************************************/
DOMNode*            SequenceTree::GetParentNode (DOMNode*     node) {

    return node->getParentNode();

}

/***********************************************************/
DOMNodeList*        SequenceTree::GetChildNodes (DOMNode*     node) {

    return node->getChildNodes();

}

/***********************************************************/
DOMNamedNodeMap*    SequenceTree::GetAttributes (DOMNode*     node) {
    return node->getAttributes();
}

/***********************************************************/
Module*             SequenceTree::GetModule    (DOMNode*     node) {

	map<DOMNode*,Module*>::iterator iter = m_Modules.find(node);

  	if( iter != m_Modules.end() )
  	    return ( iter->second );

	return NULL;

}

/***********************************************************/
unsigned int        SequenceTree::AddModule    (string       name) {

    unsigned int id = 0;
    return id;

}

/***********************************************************/
unsigned int        SequenceTree::Populate     ()  {

	DOMNode* topnode;
	if (!(topnode = m_dom_doc->getFirstChild())) return EMPTY_DOCUMENT;

	//Prepare the parameters first! (if any specified in XML)
	DOMNodeList* dnl = m_dom_doc->getElementsByTagName( StrX("Parameters").XMLchar() );
	if (dnl->getLength() > 0) {

		m_parameters = Parameters::instance();
		SequenceTree::CreateModule(this,dnl->item(0));
		m_parameters->Prepare(PREP_INIT);
		m_parameters->Prepare(PREP_VERBOSE);

		if (dnl->getLength() > 1)
			cout << "Warning: multiple tags 'Parameters' in XML." << endl;

	}

	//populate
	m_depth = 0;
	RunTree(topnode, this, &SequenceTree::CreateModule);
	m_depth--;
	//find top node of the sequence tree
	ConcatSequence* m_root_seq = GetRootConcatSequence();

	//if tree comes from a container sequence, use it as the root
	ContainerSequence* cs = GetContainerSequence();
	if (cs!=NULL) {
		m_root_seq = (ConcatSequence*) cs;
	}

	//run Prepare() several times to solve module cross-dependencies (silent)
	m_root_seq->Prepare(PREP_INIT);
	m_root_seq->Prepare(PREP_INIT);
	m_root_seq->Prepare(PREP_INIT);

	//verbose Prepare call : errors have to be reported
	m_root_seq->Prepare(PREP_VERBOSE);

	//prep. static atom, if exists
	World* pW = World::instance();
	if (pW->pStaticAtom != NULL)
	{
		pW->pStaticAtom->Prepare(PREP_INIT);
		pW->pStaticAtom->Prepare(PREP_VERBOSE);
		m_root_seq->GetDuration();
		pW->pStaticAtom->GetDuration();			//set the duration of the static atom
	}

	//check name consistency
	map<DOMNode*,Module*>::iterator iter1;
	map<DOMNode*,Module*>::iterator iter2;
	for (iter1 = m_Modules.begin(); iter1!=m_Modules.end(); iter1++ )
	  for (iter2 = iter1,iter2++; iter2!=m_Modules.end(); iter2++ )
		if ( (iter1->second)->GetName() == (iter2->second)->GetName() )
			cout	<< "SequenceTree::Populate warning: 2 modules with equal names exist: '"
				<<  (iter1->second)->GetName() << "'. Class types: ("
				<<  (iter1->second)->GetClassType() << "," <<  (iter2->second)->GetClassType() << ")" << endl;

	return OK;

}

/***********************************************************/
int        SequenceTree::RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*) ,int depth ) {

	DOMNode* child;

	if (node) {

		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {

			string s = StrX(node->getNodeName()).std_str() ;
			transform(s.begin(), s.end(), s.begin(), (int(*)(int)) toupper);

			if ( s != "PARAMETERS" ) {
				unsigned int code = fun(ptr,node);
				if (code>0) return depth;
			}

			depth++;
			m_depth = (depth>m_depth?depth:m_depth);

			bool static_atom = false;

			for (child = node->getFirstChild(); child != 0; child=child->getNextSibling())
			{
				RunTree(child,ptr,fun,depth);
				string cn = StrX(child->getNodeName()).std_str();
				transform(cn.begin(), cn.end(), cn.begin(), (int(*)(int)) toupper);
				if (s == "PARAMETERS" && cn == "ATOMICSEQUENCE" )
				{
					if (static_atom) cout << "SequenceTree::Populate warning: multiple static Atoms defined!\n";
					AtomicSequence* A = ((AtomicSequence*) m_Modules.find(child)->second);
					World* pW = World::instance();
					pW->pStaticAtom = A;
					static_atom = true;
				}
			}
		}
	}

	return depth;

}

/***********************************************************/
unsigned int SequenceTree::CreateModule(void* ptr,DOMNode* node){

	SequenceTree* ST = (SequenceTree*) ptr;
	Module* module   = ST->m_mpf->CloneModule(node);


	if (!module) return 1;

	ST->m_Modules.insert(pair<DOMNode*, Module*> (node, module));
	module->SetSeqTree(ST);
	module->Initialize(node);

	return OK;

}

/***********************************************************/
Module*               SequenceTree::GetParent    (DOMNode* node)         {

	DOMNode* parent = node->getParentNode();

	if (!parent)
		return NULL;

	return m_Modules.find(parent)->second;

}

/***********************************************************/
vector<Module*>       SequenceTree::GetChildren  (DOMNode* node)         {

	vector<Module*> children;

	DOMNodeList* dnl = node->getChildNodes();

	for (unsigned int i = 0; i < dnl->getLength(); i++) {
		if (dnl->item(i)->getNodeType() == DOMNode::ELEMENT_NODE)
			children.push_back( m_Modules.find(dnl->item(i))->second );
	}

	return children;

}


/***********************************************************/
Module*               SequenceTree::GetChild     (DOMNode* node, unsigned int position) {

	vector<Module*> children = GetChildren(node);
	return children.at(position);

}


/***********************************************************/
ConcatSequence*               SequenceTree::GetRootConcatSequence() {

	DOMNodeList* dnl = m_dom_doc->getElementsByTagName( StrX("ConcatSequence").XMLchar() );
	if ( m_Modules.find(dnl->item(0)) == m_Modules.end() ) return NULL;
	return ((ConcatSequence*) m_Modules.find(dnl->item(0))->second);

}

/***********************************************************/
ContainerSequence*               SequenceTree::GetContainerSequence() {

	DOMNodeList* dnl = m_dom_doc->getElementsByTagName( StrX("ContainerSequence").XMLchar() );
	if ( m_Modules.find(dnl->item(0)) == m_Modules.end() ) return NULL;
	return ((ContainerSequence*) m_Modules.find(dnl->item(0))->second);

}

/***********************************************************/
Module* SequenceTree::GetModuleByAttributeValue  (string name, string value) {

    map<DOMNode*, Module*>::iterator itmod;

    for( itmod = m_Modules.begin(); itmod != m_Modules.end(); itmod++ )
        if ( value == StrX(((DOMElement*) itmod->first )->getAttribute (StrX(name).XMLchar())).std_str() )
            return itmod->second;

    return NULL;

}

/***********************************************************/
void          SequenceTree::SerializeModules(string xml_file){

	DOMImplementation* impl    =  DOMImplementationRegistry::getDOMImplementation(StrX("Core").XMLchar() );

	if (impl==NULL) return;

	DOMDocument* docbackup =  m_dom_doc;
	DOMDocument* doc       =  impl->createDocument( 0, StrX("JEMRIS_MODULES").XMLchar(), 0);
	m_dom_doc              = doc;
	DOMNode*     topnode   = doc->getFirstChild();

	DOMElement* concats    = doc->createElement (  StrX("CONCATS").XMLchar() );
	DOMElement* atoms      = doc->createElement (  StrX("ATOMS"  ).XMLchar() );
	DOMElement* pulses     = doc->createElement (  StrX("PULSES" ).XMLchar() );

	topnode->appendChild(concats);
	topnode->appendChild(atoms);
	topnode->appendChild(pulses);

	map<string, Module*>* ModList = m_mpf->GetModuleList();
	map<string, Module*>::iterator itmod;

	for(itmod = ModList->begin(); itmod != ModList->end(); itmod++) {

		string module_name = itmod->first;
		DOMElement* node   = doc->createElement ( StrX(module_name).XMLchar() );
		Module* module     = m_mpf->CloneModule(node);

		module->SetSeqTree(this);
		module->Initialize(node);
		module->Prepare(PREP_INIT);

		//if any automatically added children, remove them
		for (int i=0;i<module->GetNumberOfChildren(); i++)
			node->removeChild(module->GetChild(i)->GetNode());

		module->AddAllDOMattributes();

		if (module->GetType() == MOD_CONCAT) concats->appendChild(node);
		if (module->GetType() == MOD_CONTAINER) concats->appendChild(node); //TMPTMP not good
		if (module->GetType() == MOD_ATOM  )   atoms->appendChild(node);
		if (module->GetType() == MOD_PULSE )  pulses->appendChild(node);
		if (module->GetType() == MOD_VOID  ) topnode->appendChild(node);

		cout << module->GetClassType() << endl;
	}

	XMLIO xio;
	xio.Write(impl, doc->getDocumentElement(), xml_file);

	m_dom_doc = docbackup;

}
