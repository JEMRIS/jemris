/** @file CoilArray.cpp
 *  @brief Implementation of JEMRIS CoilArray
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

#include "BinaryContext.h"
#include "CoilArray.h"
#include "Coil.h"
#include "StrX.h"
#include <sstream>
#include "SequenceTree.h"
#include "ConcatSequence.h"

/***********************************************************/
CoilArray::CoilArray () {

    m_dom_doc = 0;
    m_radius  = 0;
    m_mode    = RX;
    m_params  = NULL;
    m_signal_prefix = "signals";
    m_senmap_prefix = "sensmaps";
    m_signal_output_dir = "";
    m_senmap_output_dir = "";
    m_cpf     = new CoilPrototypeFactory();
    m_xio     = new XMLIO();

}

/***********************************************************/
CoilArray::~CoilArray() {

	delete m_xio;
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
bool CoilArray::Prepare (const PrepareMode mode) {

	for (unsigned int i=0; i<m_coils.size(); i++)
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

   	m_dom_doc = m_xio->Parse(uri.c_str());

}

/**************************************************/
void CoilArray::InitializeSignals (long lADCs) {

	for (unsigned int i=0; i<GetSize(); i++)
		m_coils[i]->InitSignal(lADCs);

}

/**************************************************/
void CoilArray::Receive (long lADC){

	for (unsigned int i=0; i<GetSize(); i++)
		m_coils[i]->Receive(lADC);

}

/**********************************************************/
IO::Status CoilArray::DumpSignals (string prefix, bool normalize) {

	/*	if (prefix != "")
		m_signal_prefix = "channel";
	
	for (unsigned int i=0; i < GetSize(); i++) {

	    m_coils[i]->GetSignal()->DumpTo(sstr.str(),normalize);

		}*/

	if (normalize) // WIP parameter to check if signal is already normalized to avoid double normalization in DumpSignalsISMRMRD()
		m_normalized = true; 

	BinaryContext bc (m_signal_output_dir + m_signal_prefix + ".h5", IO::OUT);
	NDData<double> di;
	std::string URL, URN;

	for (int c = 0; c < GetSize(); c++) {
		
		Repository* repository = m_coils[c]->GetSignal()->Repo();
		RNG*        rng        = m_coils[c]->GetSignal()->Noise();

		if (normalize) {
				
			for (int j = 0; j < repository->NProps(); j++) {

				for (long i = 0; i < repository->Samples(); i++) {	

					(*repository)[i*repository->NProps() + j] /= World::instance()->TotalSpinNumber;
				
					//dwelltime-weighted random noise
					if (World::instance()->RandNoise > 0.0) {
					
						double dt =  1.0;
					
						if      (i                    > 0) dt = repository->TP(i  ) - repository->TP(i-1);
						else if (repository->Samples() > 1) dt = repository->TP(i+1) - repository->TP(i  );
					
						//definition: Gaussian has std-dev World::instance()->RandNoise at a dwell-time of 0.01 ms
						//for (int j = 0; j < repository->Compartments(); j++) {
						(*repository)[i*repository->NProps() + j*3 + 0] += World::instance()->LargestM0*World::instance()->RandNoise*rng->normal()*0.1/sqrt(dt);
						(*repository)[i*repository->NProps() + j*3 + 1] += World::instance()->LargestM0*World::instance()->RandNoise*rng->normal()*0.1/sqrt(dt);
					}
				}

			}
			
		}

		stringstream sstr;
		sstr << setw(2) << setfill('0') << c;

		di = NDData<double> (repository->Samples(), repository->NProps());
		memcpy (&di[0], repository->Data(), di.Size() * sizeof(double));
		URN = sstr.str();
		bc.Write(di, URN, "/signal/channels/");
		
		if (URN == "00") {
			di = NDData<double> (repository->Samples());
			memcpy (&di[0], repository->Times(), di.Size() * sizeof(double));
			bc.Write(di, "times", "/signal");
		}


	}

	//REVISE
	return IO::OK;
	
}

/**********************************************************/
IO::Status CoilArray::DumpSignalsISMRMRD (string prefix, bool normalize) {

	if (m_normalized) // WIP parameter to check if signal was already normalized by DumpSignals(). Can be removed, if DumpSignals is removed.
		normalize = false;

	// Open ISMRMRD dataset containing sequence data
	ISMRMRD::Dataset d_tmp((m_signal_output_dir + m_signal_prefix + prefix + "_tmp.h5").c_str(), "dataset", false);
	std::string xml;
    d_tmp.readHeader(xml);

	// Write new file containing both sequence and signal data
	std::remove((m_signal_output_dir + m_signal_prefix + prefix + ".h5").c_str());
	ISMRMRD::Dataset d((m_signal_output_dir + m_signal_prefix + prefix + ".h5").c_str(), "dataset", true);
	d.writeHeader(xml);

	ISMRMRD::Acquisition acq;

	// Write sensitivity maps - (mis)use the first N acquisitions, where N is the number of coils
	size_t sl = m_coils[0]->GetPoints();
	NDData<double> mag = (m_coils[0]->GetNDim() == 3) ? NDData<double> (sl, sl, sl) : NDData<double> (sl, sl, 1);
	NDData<double> pha = mag;
	std::vector<size_t> dims = mag.Dims();
	if(dims[0]*dims[1] <= 65535 && dims[2]*3 <= 65535){ // maximum of uint16
		acq.resize(dims[0]*dims[1], dims[2], 3);

		for (unsigned i = 0; i < m_coils.size(); ++i) {
			m_coils[i]->GridMap();
			memcpy (&mag[0], m_coils[i]->MagnitudeMap(), sizeof(double)*mag.Size());
			memcpy (&pha[0], m_coils[i]->PhaseMap(), sizeof(double)*mag.Size());
			for(size_t x = 0; x < dims[0]; ++x){
				for(size_t y = 0; y < dims[1]; ++y){
					for(size_t z = 0; z < dims[2]; ++z)
						acq.data(x*dims[1] + y, z) = std::polar(mag(x,y,z), pha(x,y,z));
				}
			}
			// save dimensions for reshaping and interpolation in reco
			acq.traj(0,0) = dims[0];
			acq.traj(1,0) = dims[1];
			acq.traj(2,0) = dims[2];
			acq.user_int()[0] = m_coils[i]->GetExtent();
			acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_SURFACECOILCORRECTIONSCAN_DATA);
			acq.sample_time_us() = 1;
			acq.scan_counter() = 0;
			d.appendAcquisition(acq);
		}
	}
	else{
		printf("Coil size too big to save it in ISMRMRD file. Number of points should not exceed 255."); // has to be < sqrt(65535)
	}

	// Dump data to ISMRMRD file
	std::vector<ISMRMRD::Acquisition> acqList;
	int offset = 0;
	for (int c = 0; c < GetSize(); c++) {

		Repository* repository = m_coils[c]->GetSignal()->Repo();
		RNG*        rng        = m_coils[c]->GetSignal()->Noise();

		for (long i = 0; i < repository->Samples(); i++) {
			
			if (normalize) {
				
				for (int j = 0; j < repository->NProps(); j++) 
					(*repository)[i*repository->NProps() + j] /= World::instance()->TotalSpinNumber;
				
				//dwelltime-weighted random noise
				if (World::instance()->RandNoise > 0.0) {
					
					double dt =  1.0;
					
					if      (i                    > 0) dt = repository->TP(i  ) - repository->TP(i-1);
					else if (repository->Samples() > 1) dt = repository->TP(i+1) - repository->TP(i  );
					
					//definition: Gaussian has std-dev World::instance()->RandNoise at a dwell-time of 0.01 ms
					for (int j = 0; j < repository->Compartments(); j++) {
						(*repository)[i*repository->NProps() + j*3 + 0] += World::instance()->RandNoise*rng->normal()*0.1/sqrt(dt);
						(*repository)[i*repository->NProps() + j*3 + 1] += World::instance()->RandNoise*rng->normal()*0.1/sqrt(dt);
					}
					
				}

			}
			
		}
		NDData<double> di (repository->NProps(), repository->Samples());
		memcpy (&di[0], repository->Data(), di.Size() * sizeof(double));

		if (c==0){
			for (int n = 0; n < d_tmp.getNumberOfAcquisitions(); ++n){

				d_tmp.readAcquisition(n, acq);

				if (!acq.isFlagSet(ISMRMRD::ISMRMRD_ACQ_USER1)){ // USER1 flag is set for TPOI's without ADCs

					acq.resize(acq.number_of_samples(), GetSize(), acq.trajectory_dimensions()); // set number of coils
					for (int s = 0; s < acq.number_of_samples(); ++s){
						std::complex<float> sig (di(0,offset+s), di(1,offset+s));
						acq.data(s,c) = sig;
					}
					acqList.push_back(acq);
					offset += acq.number_of_samples();
				}
			}
		}
		else{
			offset = 0;
			for(int n = 0; n<acqList.size(); ++n){
				for (int s = 0; s < acqList[n].number_of_samples(); ++s){
						std::complex<float> sig (di(0,offset+s), di(1,offset+s));
						acqList[n].data(s,c) = sig;
				}
				offset += acqList[n].number_of_samples();
			}
		}

	}

	// Write acquistions from acqList
	for(int n = 0; n< acqList.size(); ++n)
		d.appendAcquisition(acqList[n]);


	Repository* repository = m_coils[0]->GetSignal()->Repo();
	if(offset != repository->Samples())
		cout << "Not all signal samples written to ISMRMRD file. Number of unwritten samples: " << repository->Samples() - offset << endl;

	std::remove((m_signal_output_dir + m_signal_prefix + prefix + "_tmp.h5").c_str());

	return IO::OK;

}

/**********************************************************/
IO::Status CoilArray::DumpSensMaps (bool verbose) {

	BinaryContext bc (m_senmap_output_dir + m_senmap_prefix+".h5", IO::OUT);
	if (bc.Status() != IO::OK)	return bc.Status();

	if (verbose) cout << "dumping sensitivity maps to " << m_senmap_prefix << ".h5 ...\n";

	size_t sl = m_coils[0]->GetPoints();
	NDData<double> mag = (m_coils[0]->GetNDim() == 3) ? NDData<double> (sl, sl, sl) : NDData<double> (sl, sl);
	NDData<double> pha = mag;

	for (unsigned i = 0; i < m_coils.size(); ++i) {
		stringstream sstr;
		sstr << setw(2) << setfill('0') << i;
		m_coils[i]->GridMap();
		memcpy (&mag[0], m_coils[i]->MagnitudeMap(), sizeof(double)*mag.Size());
		memcpy (&pha[0], m_coils[i]->PhaseMap(), sizeof(double)*mag.Size());
	    bc.Write (mag, sstr.str(), "/maps/magnitude");
	    bc.Write (pha, sstr.str(), "/maps/phase");
	}

	if (verbose) cout << "done!\n";

	
	return IO::OK;
	
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

	/*	// return: 0, if files successfully read; -2 if no files present; -1 if wrong restart files.
	bool fail = false;
	for (unsigned int i=0; i<GetSize();i++) {
		Repository rep = m_coils[i]->GetSignal()->m_repository;
		ifstream tmp;
	    stringstream sstr;
		sstr << ".tmp_sig" << setw(2) << setfill('0') << i+1 << ".bin";
		tmp.open(sstr.str().c_str(), ifstream::binary);
		if (!tmp.is_open()) {
			if (i==0) return (-2); else fail=true;
		}
		tmp.seekg (0, ios::end);
		// REVISE works only for single compartment
		int length = tmp.tellg()/sizeof(double)/4;
		if (length != rep.size) fail=true;
		if (fail) {
			tmp.close();
			for (unsigned int j=0;j<GetSize();j++) {
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
		}*/
	return (0);
}
