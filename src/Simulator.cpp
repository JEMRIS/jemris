/** @file Simulator.cpp
 *  @brief Implementation of JEMRIS Simulator
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2023  Tony Stoecker
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

#include "Simulator.h"
#include "Declarations.h"
#include "SequenceTree.h"
#include "DynamicVariables.h"
#include "Trajectory.h"
#include "MultiPoolSample.h"
#include "Bloch_McConnell_CV_Model.h"
//MODIF
#include "World.h"
//MODIF***
#include <cstdio>
#include <sstream>



/**********************************************************/
Simulator::Simulator() :
	m_rx_coil_array (0), m_xio(0), m_domtree_error_rep (0), m_dom_doc(0), m_evol(0),
	m_world (World::instance()), m_model(0), m_tx_coil_array(0), m_sample(0),
	m_seqtree(0), m_sequence(0), m_state(0), m_signal_prefix("signals"), m_output_dir("") {
	Simulator ("simu.xml");
}

/**********************************************************/
Simulator::Simulator ( const string& fname, const string& fsample, const string& frxarray,
		const string& ftxarray, const string& fsequence, const string& fmodel) :
	m_rx_coil_array (0), m_evol(0),	m_world (World::instance()), m_model(0), m_tx_coil_array(0),
	m_seqtree(0), m_sample(0), m_sequence(0), m_signal_prefix("signals"), m_output_dir("") {

	m_domtree_error_rep = new DOMTreeErrorReporter;
	m_xio               = new XMLIO();
	m_dom_doc           = m_xio->Parse(fname);

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
			SetModel(fmodel);
			SetParameter();
			m_sample->ReorderSample();
		}


	} catch (const XMLException& e) {

	}

}

/**********************************************************/
void Simulator::SetWorld          () {
	m_world = World::instance();
}

/**********************************************************/
void Simulator::SetSample         (std::string fsample) {

	if (fsample.empty())  // REVISE: IS THIS EVER FALSE?
		fsample = GetAttr(GetElem("sample"), "uri");

	std::string type (GetAttr (GetElem ("sample"), "type"));
	std::string mult (GetAttr (GetElem ("sample"), "multiple"));
	int multiple = !mult.empty() ? atoi(mult.c_str()) : 1;

	m_sample = (type == "multipool") ?
			new MultiPoolSample (fsample) :
			new Sample (fsample,multiple);

	m_world->TotalSpinNumber = m_sample->GetSize();
	m_world->SetNoOfSpinProps(m_sample->GetNProps());
	m_world->SetNoOfCompartments(m_sample->GetNoSpinCompartments());
	m_world->InitHelper (m_sample->GetHelperSize());
	m_sample->CopyHelper(m_world->Helper());
	m_sample->ReorderSample();

	string sentinterval = GetAttr(GetElem("sample"),"SentInterval");

	if (!sentinterval.empty())
		m_sample->SetTimeInterval((atof(sentinterval.c_str())));

}

/**********************************************************/
void      Simulator::SetSample      (Sample* sample) {

	m_sample = sample;
	m_world->SetNoOfCompartments(m_sample->GetNoSpinCompartments());
	SetModel();
	SetParameter();

}

/**********************************************************/
void Simulator::SetRxCoilArray      (string frxarray) {

	if (frxarray.empty())
		frxarray = GetAttr(GetElem("RXcoilarray"), "uri");

	m_rx_coil_array = new CoilArray();
	m_rx_coil_array->setMode(0);

	std::string sp (GetAttr(GetElem("RXcoilarray"), "SignalPrefix"));

	if (!sp.empty())
		m_rx_coil_array->SetSignalPrefix(sp);

	m_rx_coil_array->Initialize(frxarray);
	m_rx_coil_array->Populate();

}

/**********************************************************/
void Simulator::SetTxCoilArray      (string ftxarray) {

	if (ftxarray.empty())
		ftxarray = GetAttr(GetElem("TXcoilarray"), "uri");

	m_tx_coil_array = new CoilArray();
	m_tx_coil_array ->setMode(1);
	m_tx_coil_array ->Initialize(ftxarray);
	m_tx_coil_array ->Populate();

}

/**********************************************************/
void Simulator::SetModel          (std::string fmodel) {

	if (fmodel.empty())
		fmodel = GetAttr(GetElem("model"), "type");

	if (fmodel == "BM_CVODE")
		m_model = new Bloch_McConnell_CV_Model ();
	else
		m_model = new Bloch_CV_Model ();

	if (m_sample        != NULL && m_sequence      != NULL &&
	    m_rx_coil_array != NULL && m_tx_coil_array != NULL &&
	    m_model         != NULL)
		m_model->Prepare( m_rx_coil_array, m_tx_coil_array, m_sequence, m_sample );
	else
		cout << "serious problems! Nullpointers to framework objects!!\n";

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
	}

	string 	   reorderSamp = GetAttr(element, "SampleReorder");
	if (!reorderSamp.empty()) {
		m_sample->SetReorderStrategy(reorderSamp);
	}

	// load trajectories for dynamic variables:
	DynamicVariables *dynVar = DynamicVariables::instance();

//MODIF
	string     Flow = GetAttr(GetElem("sample"), "FlowTrajectories");
	if (!Flow.empty()) {
        string  t_loop = GetAttr(GetElem("sample"), "FlowLoopDuration");;
        string  n_loop = GetAttr(GetElem("sample"), "FlowLoopNumber");
        if (!(t_loop.empty() || n_loop.empty()))
            dynVar->m_Flow->FlowLoop(atof(t_loop.c_str()),atol(n_loop.c_str()));
		dynVar->m_Flow->LoadFile(Flow);//,m_world->TotalSpinNumber);
		//cout<<m_world->m_myRank<<" CALLED FLOW LOADING"<<endl;
    }
//MODIF***

//Mod
    string     Respiration= GetAttr(GetElem("sample"), "RespirationTrajectory");
    if (!Respiration.empty()) {
        dynVar->m_Respiration->LoadFile(Respiration);
    }

//

	string     Motion = GetAttr(GetElem("sample"), "MotionTrajectory");
	if (!Motion.empty()) {
		dynVar->m_Motion->LoadFile(Motion);
	}

	string     T2prime = GetAttr(GetElem("sample"), "T2primeTrajectory");
	if (!T2prime.empty()) {
		dynVar->m_T2prime->LoadFile(T2prime);
	}

	string     T1 = GetAttr(GetElem("sample"), "R1Trajectory");
	if (!T1.empty()) {
		dynVar->m_R1->LoadFile(T1);
	}

	string     T2 = GetAttr(GetElem("sample"), "R2Trajectory");
	if (!T2.empty()) {
		dynVar->m_R2->LoadFile(T2);
	}

	string     M0 = GetAttr(GetElem("sample"), "M0Trajectory");
	if (!M0.empty()) {
		dynVar->m_M0->LoadFile(M0);
	}

	string active = GetAttr(GetElem("sample"), "ActiveCircles");
	if (!active.empty()) {
	    stringstream ss(active); // Insert the string into a stream
	    double pos[3];
	    double r;
	    while (ss >> pos[0]) {
	        if (!(ss >> pos[1])) { cout << "Error reading active Areas; need 4 values per circle. (x/y/z/ radius)." << endl; return;}
	        if (!(ss >> pos[2])) {cout << "Error reading active Areas; need 4 values per circle. (x/y/z/ radius)." << endl; return;}
	        if (!(ss >> r)) {cout << "Error reading active Areas; need 4 values per circle. (x/y/z/ radius)." << endl; return;}
	        dynVar->AddActiveCircle(pos,r);
	    }
	}

	string     diffusion = GetAttr(GetElem("sample"), "Diffusionfile");
	if (!diffusion.empty()) {
		dynVar->m_Diffusion->LoadFile(diffusion);
	}

}

/**********************************************************/
string Simulator::GetAttr         (DOMElement* element, const string& key) {
	return StrX( element->getAttribute( StrX(key).XMLchar()) ).std_str() ;
}

/**********************************************************/
DOMElement* Simulator::GetElem    (string name) {
	return (DOMElement*)(m_dom_doc->getElementsByTagName(StrX(name).XMLchar()))->item(0);
}

/**********************************************************/
void Simulator::Simulate          (bool bDumpSignal, bool bSolve) {

	m_rx_coil_array->InitializeSignals (m_sequence->GetNumOfADCs());

	bool img_adcs = true;
	if (bDumpSignal) {
		m_kspace = new KSpace<double,4>();
		KSpace<double,4>::KPoint p;
		m_kspace->PushBack(p);
		CheckRestart();

		// Initialize temporary ISMRMRD file with sequence data
		img_adcs = m_sequence->SeqISMRMRD(m_rx_coil_array->GetSignalOutputDir() + m_rx_coil_array->GetSignalPrefix() + "_ismrmrd_tmp.h5");
		if (!img_adcs)
			remove((m_rx_coil_array->GetSignalOutputDir() + m_rx_coil_array->GetSignalPrefix() + "_ismrmrd_tmp.h5").c_str());
	}

	m_model->Solve();

	if (bDumpSignal) {
		m_rx_coil_array->DumpSignals();
		m_kspace->Write(m_rx_coil_array->GetSignalOutputDir() + m_rx_coil_array->GetSignalPrefix() + ".h5", "kspace", "/");
		if (img_adcs)
			m_rx_coil_array->DumpSignalsISMRMRD("_ismrmrd", true);
		DeleteTmpFiles();
	}

}

/**********************************************************/
void Simulator::SetSignalPrefix(string prefix) {

	m_signal_prefix = prefix;
	if (m_rx_coil_array != NULL) {
		m_rx_coil_array->SetSignalPrefix(m_signal_prefix);
	}
}

/**********************************************************/
void Simulator::SetOutputDir(string output_dir) {

	m_output_dir = output_dir;
	if (m_rx_coil_array != NULL) {
		m_rx_coil_array->SetSignalOutputDir(m_output_dir);
	}
}

/**********************************************************/
void Simulator::SetSequence       (string seq) {

	m_seqtree = new SequenceTree;


	if ( seq.empty() ) seq = GetAttr (GetElem("sequence"), "uri");

	m_seqtree->Initialize(seq);
	m_seqtree->Populate();

	m_sequence = m_seqtree->GetRootConcatSequence();

	if (m_sequence!=NULL && m_model!=NULL) m_model->SetSequence(m_sequence);

	m_world->pSeqTree        = m_seqtree;
	m_world->TotalADCNumber  = m_sequence->GetNumOfADCs();

}

/**********************************************************/
Simulator::~Simulator             () {

	if (m_xio               != NULL) delete m_xio;
	if (m_domtree_error_rep != NULL) delete m_domtree_error_rep;
	if (m_rx_coil_array     != NULL) delete m_rx_coil_array;
	if (m_tx_coil_array     != NULL) delete m_tx_coil_array;
	if (m_model             != NULL) delete m_model;
	if (m_sample            != NULL) delete m_sample;
	if (m_seqtree           != NULL) delete m_seqtree;
	if (m_world             != NULL) delete m_world;

}

/**********************************************************/
void Simulator::DeleteTmpFiles(){
	remove(".spins_state.dat");
	for (unsigned int i=0; i<m_rx_coil_array->GetSize(); i++) {
		stringstream sstr;
		sstr << ".tmp_sig" << setw(2) << setfill('0') << i+1 << ".bin";
		remove(sstr.str().c_str());
	}
}
/**********************************************************/
void Simulator::CheckRestart(){
	int sampstate = m_sample->ReadSpinsState();
	if (sampstate == -1) MoveTmpFiles();
	if (sampstate == 0) {
		int sigstate=m_rx_coil_array->ReadRestartSignal();
		if (sigstate == 0) {
			cout << "\nRestart files found. Resuming calculation.\n" << endl;
			return;
		}
		MoveTmpFiles();
		m_sample->ClearSpinsState();
	}
}
/**********************************************************/
void Simulator::MoveTmpFiles(){
	cout << "Restart file does not fit to current simulation. move .spins_state.dat to spins_state.bak; .tmp_sig*.bin to tmp_sig.bak. " << endl;
	rename(".spins_state.dat","spins_state.bak");
	for (unsigned int j=0;j<m_rx_coil_array->GetSize();j++) {
	    stringstream sstr1,sstr2;
		sstr1 << ".tmp_sig" << setw(2) << setfill('0') << j+1 << ".bin";
		sstr2 << "tmp_sig" << setw(2) << setfill('0') << j+1 << ".bak";
		rename(sstr1.str().c_str(),sstr2.str().c_str());
	}

}
