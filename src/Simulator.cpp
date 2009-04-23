/** @file Simulator.cpp
 *  @brief Implementation of JEMRIS Simulator
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

#include "Simulator.h"
#include "Declarations.h"
#include "SequenceTree.h"


/**********************************************************/
Simulator::Simulator() {

	Simulator ("simu.xml");

}

/**********************************************************/
Simulator::Simulator (string fname, string fsample, string frxarray, string ftxarray, string fsequence, string fmodel) {

	m_world	            = NULL;
	m_sample            = NULL;
	m_sequence	        = NULL;
	m_rx_coil_array	    = NULL;
	m_tx_coil_array	    = NULL;
	m_model	            = NULL;
	m_state             = false;

	m_domtree_error_rep = new DOMTreeErrorReporter;
	m_xml_read          = new XMLRead();
	m_dom_doc           = m_xml_read->ParseFile(fname);

	DOMNode* topnode;

	if (!(topnode = m_dom_doc->getFirstChild())) return;

	m_state   = ( StrX(topnode->getNodeName()).std_str() == "simulate");

	if (!m_state) return;

	m_evol    = 0;

	try {

		SetWorld       ();
		SetRxCoilArray (frxarray );
		SetTxCoilArray (ftxarray );
		SetSequence    (fsequence);

		if ( fsample!="NoSample" ) {
			SetSample(fsample);
			//setting of model and simulaton-parameters
			SetModel(fmodel);
			SetParameter();
		}


	} catch (const XMLException& e) {

	}

	//m_state = ( m_rx_coil_array->GetStatus() &&  m_tx_coil_array->GetStatus() );

}

/**********************************************************/
void Simulator::SetWorld          () {
	m_world = World::instance();
}

/**********************************************************/
void Simulator::SetSample         (string fsample) {

	if (fsample.empty()) fsample = GetAttr(GetElem("sample"), "uri");
	m_sample = new Sample (fsample);
	m_world->TotalSpinNumber = m_sample->GetSize();
}

/**********************************************************/
void      Simulator::SetSample      (Sample* sample){

	m_sample = sample;
	//re-setting of model and simulation-parameters
	SetModel("CVODE");
	SetParameter();


};
/**********************************************************/
void Simulator::SetRxCoilArray      (string frxarray) {

	if (frxarray.empty()) frxarray = GetAttr(GetElem("RXcoilarray"), "uri");

	m_rx_coil_array = new CoilArray();
	m_rx_coil_array->setMode(0);

	string sp = GetAttr(GetElem("RXcoilarray"), "SignalPrefix");
	if (!sp.empty()) m_rx_coil_array->SetSignalPrefix(sp);

	m_rx_coil_array->Initialize(frxarray);
	m_rx_coil_array->Populate();

}

/**********************************************************/
void Simulator::SetTxCoilArray      (string ftxarray) {

	if (ftxarray.empty()) ftxarray = GetAttr(GetElem("TXcoilarray"), "uri");
	m_tx_coil_array = new CoilArray();
	m_tx_coil_array ->setMode(1);
	m_tx_coil_array ->Initialize(ftxarray);
	m_tx_coil_array ->Populate();

}

/**********************************************************/
void Simulator::SetModel          (string fmodel) {

	if (fmodel.empty()) fmodel = GetAttr(GetElem("model"), "type");

	if (fmodel == "CVODE")
		m_model = new Bloch_CV_Model();
	// ... more models to be implemented here

	if ( m_sample	     != NULL && m_sequence      != NULL &&
	     m_rx_coil_array != NULL && m_tx_coil_array != NULL &&
	     m_model	     != NULL) {

		m_model->Prepare( m_rx_coil_array, m_tx_coil_array, m_sequence, m_sample );

	} else {

		cout << "serious problems! Nullpointers to framework objects!!\n";

	}

}

/**********************************************************/
void Simulator::SetParameter      () {

	DOMElement* element = GetElem("parameter");

	string          PR = GetAttr(element, "PositionRandomness");
	if (!PR.empty()) m_sample->SetPositionRandomness(atof(PR.c_str()));

	string         lFF = GetAttr(element, "R2Prime");
	if (!lFF.empty()) m_sample->SetR2Prime(atof(lFF.c_str()));

	string        step = GetAttr(element, "EvolutionSteps");
	if (!step.empty()) m_world->saveEvolStepSize  = atoi(step.c_str());

	string        file = GetAttr(element, "EvolutionPrefix");
	if (!file.empty()) m_world->saveEvolFileName  = file ;

	string  GMAXoverB0 = GetAttr(element, "ConcomitantFields");
	if (!GMAXoverB0.empty()) m_world->GMAXoverB0  = atof(GMAXoverB0.c_str());

	string  	   PRN = GetAttr(element, "RandomNoise");
	if (!PRN.empty()) m_world->RandNoise  = atof(PRN.c_str());

	string 	   LoadBal = GetAttr(element, "LoadBalancing");
	if (!LoadBal.empty() && (atof(LoadBal.c_str()) == 1)) {
		m_world->m_useLoadBalancing  = true;
		if (m_world->saveEvolStepSize > 0) {
			cout << "Storing magnetization evolution does currently not work together with load balancing. Load balancing is switching off." << endl;
			m_world->m_useLoadBalancing = false;
		}
	}


}

/**********************************************************/
string Simulator::GetAttr         (DOMElement* element, string key) {
	return StrX( element->getAttribute( StrX(key).XMLchar()) ).std_str() ;
}

/**********************************************************/
DOMElement* Simulator::GetElem    (string name) {
	return (DOMElement*)(m_dom_doc->getElementsByTagName(StrX(name).XMLchar()))->item(0);
}

/**********************************************************/
void Simulator::Simulate          (bool bDumpSignal, bool InitSignal) {

	if (InitSignal)  m_rx_coil_array->InitializeSignals( m_sequence->GetNumOfADCs() );
	m_model->Solve();
	if (bDumpSignal) m_rx_coil_array->DumpSignals();

}

/**********************************************************/
void Simulator::SetSequence       (string seq) {

	SequenceTree* seqTree = SequenceTree::instance();

	if ( seq.empty() ) seq = GetAttr (GetElem("sequence"), "uri");

	seqTree->Initialize(seq);
	seqTree->Populate();

	m_sequence = seqTree->GetRootConcatSequence();

	if (m_sequence!=NULL && m_model!=NULL) m_model->SetSequence(m_sequence);

	m_world->TotalADCNumber  = m_sequence->GetNumOfADCs();

}

/**********************************************************/
Simulator::~Simulator             () {

	if (m_xml_read          != NULL) delete m_xml_read;
	if (m_sample            != NULL) delete m_sample;
	if (m_domtree_error_rep != NULL) delete m_domtree_error_rep;
	if (m_rx_coil_array     != NULL) delete m_rx_coil_array;
	if (m_tx_coil_array     != NULL) delete m_tx_coil_array;
	if (m_model             != NULL) delete m_model;

	//the simulator deletes the singletons !
	if (m_world != NULL) delete m_world;
	delete SequenceTree::instance();

}
