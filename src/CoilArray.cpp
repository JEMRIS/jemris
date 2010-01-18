/** @file CoilArray.cpp
 *  @brief Implementation of JEMRIS CoilArray
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#include "CoilArray.h"
#include "Coil.h"
#include "StrX.h"
#include <sstream>

/***********************************************************/
CoilArray::CoilArray () {

    m_dom_doc  = 0;
    m_mode   = RX;
    m_signal_prefix = "signal";
    m_cpf     = new CoilPrototypeFactory();
    m_xml_read = new XMLRead();

}

/***********************************************************/
CoilArray::~CoilArray() {

	delete m_xml_read;
	delete m_cpf;
	XMLPlatformUtils::Terminate();

}

/***********************************************************/
unsigned int CoilArray::Populate () {

	DOMNode* topnode;

	if (!(topnode = m_dom_doc->getFirstChild()))
		return EMPTY_DOCUMENT;

	if ((string)StrX(topnode->getNodeName()).localForm() != "CoilArray")
		return EMPTY_DOCUMENT;

	RunTree(topnode, this, &CoilArray::CreateCoil);

	//Prepare(PREP_INIT);
	Prepare(PREP_VERBOSE);

	return OK;

}

/***********************************************************/
bool CoilArray::Prepare (PrepareMode mode) {

	for (int i=0; i<m_coils.size(); i++)
		m_coils.at(i)->Prepare(mode);

	return true;

}

/***********************************************************/
unsigned int CoilArray::CreateCoil (void* ptr,DOMNode* node) {

	CoilArray* CA = (CoilArray*) ptr;
	Coil* coil    = CA->m_cpf->Clone(node);
	if (!coil)
		return 1;
	CA->m_coils.push_back(coil);
	coil->Initialize(node);
	return OK;

}

/**************************************************/
DOMNode* CoilArray::RunTree (DOMNode* node, void* ptr, unsigned int (*fun) (void*, DOMNode*) ) {

	DOMNode* child;
	DOMNode* rnode = NULL;

	if (node) {


		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {

			if ((string)StrX(node->getNodeName()).localForm() != "CoilArray")
				if (fun(ptr,node)>0)
					return node;

			for (child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
			    rnode = RunTree(child,ptr,fun);
			    if (rnode!=NULL) break;
			}
		}
	}

	return rnode;

}


/**************************************************/
void CoilArray::Initialize (string uri) {

   	m_dom_doc = m_xml_read->ParseFile(uri.c_str());

}

/**************************************************/
void CoilArray::InitializeSignals(long lADCs){

	for (unsigned int i=0; i<GetSize(); i++)
		m_coils[i]->InitSignal(lADCs);

};

/**************************************************/
void CoilArray::Receive (long lADC){

	for (unsigned int i=0; i<GetSize(); i++)
		m_coils[i]->Receive(lADC);

};

/**********************************************************/
void CoilArray::DumpSignals (string prefix, bool normalize) {
	string tmp;
	if (prefix != "") {
		tmp = m_signal_prefix;
		m_signal_prefix = prefix;
	}

	for (unsigned int i=0; i<GetSize(); i++) {
	    stringstream sstr;
		sstr << m_signal_prefix << setw(2) << setfill('0') << i+1 << ".bin";
		m_coils[i]->GetSignal()->DumpTo(sstr.str(),normalize);
	}

	if (prefix != "") {
		m_signal_prefix = tmp;
	}


}

/**********************************************************/
void CoilArray::DumpSensMaps() {

    string prefix = (m_mode==RX)?"RX":"TX";

    for (unsigned int i=0; i<GetSize(); i++) {

        stringstream sstr;
        sstr << prefix << "sensmap" << setw(2) << setfill('0') << i+1 << ".bin";
        m_coils[i]->DumpSensMap(sstr.str());

    }

}

/**********************************************************/
Coil* CoilArray::GetCoil(unsigned channel) {

	if (channel<m_coils.size())
		return m_coils[channel];
	else
		return NULL;
}

/**********************************************************/
int CoilArray::ReadRestartSignal(){
	// return: 0, if files sucessfully read; -2 if no files preesent; -1 if wrong restart files.
	bool fail = false;
	for (int i=0; i<GetSize();i++) {
		double data;
		Repository rep = m_coils[i]->GetSignal()->repository;
		ifstream tmp;
	    stringstream sstr;
		sstr << ".tmp_sig" << setw(2) << setfill('0') << i+1 << ".bin";
		tmp.open(sstr.str().c_str(), ifstream::binary);
		if (!tmp.is_open()) {
			if (i==0) return (-2); else fail=true;
		}
		tmp.seekg (0, ios::end);
		int length = tmp.tellg()/sizeof(double)/4;
		if (length != rep.size) fail=true;
		if (fail) {
			tmp.close();
			for (int j=0;j<GetSize();j++) {
				m_coils[j]->InitSignal(rep.size);
			}
			return (-1);
		}
		tmp.seekg (0, ios::beg);
		for (int k=0; k<length;k++) {
			tmp.read ((char*) &(rep.tp[k]),sizeof(double));
			tmp.read ((char*) &(rep.mx[k]),sizeof(double));
			tmp.read ((char*) &(rep.my[k]),sizeof(double));
			tmp.read ((char*) &(rep.mz[k]),sizeof(double));
		}
		tmp.close();
	}
	return (0);
}
