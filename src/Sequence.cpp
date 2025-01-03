/** @file Sequence.cpp
 *  @brief Implementation of JEMRIS Sequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2025  Tony Stoecker
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

#include "Sequence.h"
#include "ConcatSequence.h"
#include "AtomicSequence.h"
#include <stdio.h>
#include "BinaryContext.h"
#include "EddyPulse.h"
#include "SequenceTree.h"

/***********************************************************/
bool    Sequence::Prepare(const PrepareMode mode){

	ATTRIBUTE("Aux1"     , m_aux1 );
	ATTRIBUTE("Aux2"     , m_aux2 );
	ATTRIBUTE("Aux3"     , m_aux3 );

	bool btag = Module::Prepare(mode);

        //hide XML attributes which were set by Module::Prepare()
	if (mode != PREP_UPDATE)
	    HideAttribute("Duration");

	vector<Module*> children = GetChildren();

	for (unsigned int i=0; i<children.size() ; i++) {

		DEBUG_PRINT("  Sequence::Prepare() of " << GetName() << " calls Prepare(" << mode <<
			    ") of " << children[i]->GetName() << endl;)
		btag = (children[i]->Prepare(mode) && btag);

	}

    if (GetParent()==m_parameters && !btag && mode == PREP_VERBOSE) //only the top node of the tree cries out
            cout << "\n!!! warning in Prepare(1) of sequence " << GetName() <<endl << endl;

    return btag;

}

/***********************************************************/
void Sequence::SeqDiag (const string& fname ) {

	//prepare H5 file structure
	BinaryContext bc (fname, IO::OUT);
	if (bc.Status() != IO::OK) return;
	if ( GetNumOfTPOIs()==0  ) return;

	Prepare(PREP_INIT);

	NDData<double>      di (GetNumOfTPOIs() + 1);
	std::vector<double>  t (GetNumOfTPOIs() + 1);
	std::vector<size_t>  meta (GetNumOfTPOIs() + 1);
	int numaxes = (MAX_SEQ_VAL+1)+2;	/** Two extra: time, receiver phase */

	// Start with 0 and track excitations and refocusing
	NDData<double> seqdata(numaxes+1,GetNumOfTPOIs()+1);	/** Extra axis for META */
	
	// HDF5 dataset names
	vector<string> seqaxis;
	seqaxis.push_back("T");							//Time
	seqaxis.push_back("RXP");						//receiver phase
	seqaxis.push_back("TXM");						//transmitter magnitude
	seqaxis.push_back("TXP");						//transmitter phase
	std::string gradSuffixes = "XYZ";
	for (int i=0; i<gradSuffixes.length(); i++)
		seqaxis.push_back(std::string("G") + gradSuffixes[i]);	// Gradients
	seqaxis.push_back("META");						//Meta - used to adjust k-space for excite/refocusing
	

	//turn off nonlinear gradients in static events for sequence diagram calculation
	World* pW = World::instance();
	if (pW->pStaticAtom != NULL) pW->pStaticAtom->SetNonLinGrad(false);

	// recursive data collect
	double seqtime=  0.;
	long   offset =  0l;
	seqdata (1,0) = -1.;
	CollectSeqData (seqdata, seqtime, offset);

	// Faster
	seqdata = transpose(seqdata);
	std::copy (&seqdata[0], &seqdata[0]+di.Size(), t.begin());
	for (size_t i = 1; i < meta.size(); ++i)
		meta[i] = seqdata(i,numaxes);

	// Write columns to HDF5 file
	std::string URN;
	for (size_t i=0; i<4+gradSuffixes.length(); i++) {
		URN = seqaxis[i];
		memcpy (&di[0], &seqdata[i*di.Size()], di.Size() * sizeof(double));
		bc.Write(di, URN, "/seqdiag");
		if (i >= 4 )			/* Gradient channels*/
			bc.Write (cumtrapz(di,t,meta), std::string("K") + gradSuffixes[i-4], "/seqdiag");
	}


	// Write meta data to HDF5 file
	URN = seqaxis[seqaxis.size()-1];
	memcpy (&di[0], &seqdata[(numaxes)*di.Size()], di.Size() * sizeof(double));
	bc.Write(di, URN, "/seqdiag");
}

/***********************************************************/
bool Sequence::SeqISMRMRD (const string& fname ) {

	/* WIP: - Plot reconstruction results in GUI */

	if ( GetNumOfTPOIs()==0  ) return false;

	Prepare(PREP_INIT);

	NDData<double>      di (GetNumOfTPOIs() + 1);
	std::vector<double>  t (GetNumOfTPOIs() + 1);
	std::vector<size_t>  meta (GetNumOfTPOIs() + 1);
	std::vector<size_t>  slc_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  shot_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  part_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  set_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  contr_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  avg_ctr (GetNumOfTPOIs() + 1);

	int numaxes = (MAX_SEQ_VAL+1)+2;	/** Two extra: time, receiver phase */

	// Start with 0 and track excitations and refocusing
	NDData<double> seqdata(numaxes+7,GetNumOfTPOIs()+1);	/** Extra axes for META and 6 kspace counters */
	
	//turn off nonlinear gradients in static events for sequence diagram calculation
	World* pW = World::instance();
	if (pW->pStaticAtom != NULL) pW->pStaticAtom->SetNonLinGrad(false);

	// recursive data collect
	double seqtime=  0.;
	long   offset =  0l;
	seqdata (1,0) = -1.;
	seqdata (numaxes+1,0) = 0;
	seqdata (numaxes+2,0) = 0;
	seqdata (numaxes+3,0) = 0;
	seqdata (numaxes+4,0) = 0;
	seqdata (numaxes+5,0) = 0;
	seqdata (numaxes+6,0) = 0;
	pW->m_slice = 0; // reset singletons
	pW->m_shot = 0;
	pW->m_shotmax = 0;
	CollectSeqData (seqdata, seqtime, offset);

	// Transpose Data and collect meta and t-vector for cumtrapz
	seqdata = transpose(seqdata);
	std::copy (&seqdata[0], &seqdata[0]+di.Size(), t.begin());
	for (size_t i = 1; i < meta.size(); ++i)
		meta[i] = seqdata(i,numaxes);

	// Get counters
	for (size_t i = 1; i < slc_ctr.size(); ++i)
		slc_ctr[i] = seqdata(i,numaxes+1);
	for (size_t i = 1; i < shot_ctr.size(); ++i)
		shot_ctr[i] = seqdata(i,numaxes+2);
	for (size_t i = 1; i < slc_ctr.size(); ++i)
		part_ctr[i] = seqdata(i,numaxes+3);
	for (size_t i = 1; i < shot_ctr.size(); ++i)
		set_ctr[i] = seqdata(i,numaxes+4);
	for (size_t i = 1; i < slc_ctr.size(); ++i)
		contr_ctr[i] = seqdata(i,numaxes+5);
	for (size_t i = 1; i < shot_ctr.size(); ++i)
		avg_ctr[i] = seqdata(i,numaxes+6);

	int slices = *max_element(slc_ctr.begin(), slc_ctr.end()) + 1;
	int shots = pW->m_shotmax;
	int partitions = pW->m_partitionmax;
	int contrasts = *max_element(contr_ctr.begin(), contr_ctr.end()) + 1;
	int sets = *max_element(set_ctr.begin(), set_ctr.end()) + 1;
	int averages = *max_element(avg_ctr.begin(), avg_ctr.end()) + 1;

	// Calculate k-space trajectory
	NDData<double> kx (GetNumOfTPOIs() + 1);
	NDData<double> ky (GetNumOfTPOIs() + 1);
	NDData<double> kz (GetNumOfTPOIs() + 1);
	memcpy (&di[0], &seqdata[4*di.Size()], di.Size() * sizeof(double));
	kx = cumtrapz(di,t,meta);
	memcpy (&di[0], &seqdata[5*di.Size()], di.Size() * sizeof(double));
	ky = cumtrapz(di,t,meta);
	memcpy (&di[0], &seqdata[6*di.Size()], di.Size() * sizeof(double));
	kz = cumtrapz(di,t,meta);

	// Write acquisitions & trajectory to ISMRMRD file
	std::remove(fname.c_str()); // otherwise data is appended
	ISMRMRD::Dataset d(fname.c_str(), "dataset", true);

	// Header
	ISMRMRD::IsmrmrdHeader h;
	ISMRMRD::AcquisitionSystemInformation sys;
	ISMRMRD::MeasurementInformation measInfo;
	ISMRMRD::ExperimentalConditions expCond;
	ISMRMRD::UserParameters usrParams;
	ISMRMRD::UserParameterString usrString;

	// Set system info and sequence name
	sys.systemVendor.set("JEMRIS");
	sys.systemModel.set("v2.8.4");
	sys.systemFieldStrength_T.set(0);
	h.acquisitionSystemInformation.set(sys);
	string seq_name = pW->pSeqTree->GetSequenceFilename();
	measInfo.protocolName.set(seq_name);
	h.measurementInformation.set(measInfo);
	expCond.H1resonanceFrequency_Hz = 0;
	h.experimentalConditions = expCond;

	// save md5 signature in user string
	usrString.name = "seq_signature";
	usrString.value = pW->m_seqSignature;
	usrParams.userParameterString.push_back(usrString);
	h.userParameters.set(usrParams);

	// Encoding
	Parameters* P = Parameters::instance();
	ISMRMRD::Encoding e;
	e.trajectory = ISMRMRD::TrajectoryType::OTHER;
	e.encodedSpace.matrixSize.x = (P->m_iNx) > 0 ? P->m_iNx: 1;
	e.encodedSpace.matrixSize.y = (P->m_iNy) > 0 ? P->m_iNy: 1;
	e.encodedSpace.matrixSize.z = (partitions>1) ? P->m_iNz : ((slices>1) ? 1 : P->m_iNz);
	e.encodedSpace.fieldOfView_mm.x = (P->m_fov_x > 0) ? P->m_fov_x : 1;
	e.encodedSpace.fieldOfView_mm.y = (P->m_fov_y > 0) ? P->m_fov_y : 1;
	e.encodedSpace.fieldOfView_mm.z = (P->m_fov_z > 0) ? P->m_fov_z : 1;
	e.reconSpace.matrixSize.x = (P->m_iNx) > 0 ? P->m_iNx: 1;
	e.reconSpace.matrixSize.y = (P->m_iNy) > 0 ? P->m_iNy: 1;
	e.reconSpace.matrixSize.z = (partitions>1) ? P->m_iNz : ((slices>1) ? 1 : P->m_iNz);
	e.reconSpace.fieldOfView_mm.x = (P->m_fov_x > 0) ? P->m_fov_x : 1;
	e.reconSpace.fieldOfView_mm.y = (P->m_fov_y > 0) ? P->m_fov_y : 1;
	e.reconSpace.fieldOfView_mm.z = (P->m_fov_z > 0) ? P->m_fov_z : 1;

	// Acquisitions
	std::vector<ISMRMRD::Acquisition> acqList;
	ISMRMRD::Acquisition acq;
	u_int16_t axes = 3;
	u_int16_t readout;

	size_t adc_start = 0;
	size_t last_adc = 0; // helper variable for last scan in slice
	size_t last_idx = 0; // helper variable for last scan in slice
	size_t scan_ctr = 0; 

	// Check if imaging ADCs exist
	bool img_adcs = false;

	for (size_t i = 1; i < meta.size(); ++i){
		if (meta[i] != meta[i-1] || i == meta.size()-1){
			acq.clearAllFlags();
			readout = i - adc_start;
			acq.resize(readout, acq.active_channels(), axes);

			// trajectory - scale from [rad/mm] to dimensionless
			for (size_t k = 0; k<readout; ++k){
				acq.traj(0,k) = kx[k+adc_start] * P->m_fov_x / (2*M_PI);
				acq.traj(1,k) = ky[k+adc_start] * P->m_fov_y / (2*M_PI);
				if (pW->m_partitionmax > 1)
					acq.traj(2,k) = kz[k+adc_start] * P->m_fov_z / (2*M_PI);
				else
					acq.traj(2,k) = 0;
			}
			// Check if imaging ADCs exist
			if(check_bit(meta[i-1], ADC_IMG_T))
				img_adcs = true;

			// set ADC flags
			bool no_flag = true;
			if (check_bit(meta[i-1], ADC_T)){
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_DUMMYSCAN_DATA); // ADCs with no specific purpose
				no_flag = false;
			}
			if (check_bit(meta[i-1], ADC_ACS_T)){
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION);
				no_flag = false;
				if (check_bit(meta[i-1], ADC_IMG_T))
					acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION_AND_IMAGING);
			}
			if (check_bit(meta[i-1], ADC_PC_T)){
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PHASECORR_DATA);
				no_flag = false;
			}
			if (check_bit(meta[i-1], ADC_NOISE_T)){
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
				no_flag = false;
			}
			if (!check_bit(meta[i-1], ADC_IMG_T) && no_flag)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_USER1); // TPOI's without ADCs get user1 flag

			// kspace counters
			acq.idx().slice = slc_ctr[i-1];
			acq.idx().kspace_encode_step_1 = shot_ctr[i-1];
			acq.idx().kspace_encode_step_2 = part_ctr[i-1];
			acq.idx().set = set_ctr[i-1];
			acq.idx().contrast = contr_ctr[i-1];
			acq.idx().average = avg_ctr[i-1];

			// set last scan in slice
			if (shot_ctr[i-1] == pW->m_shotmax-1 && part_ctr[i-1] == pW->m_partitionmax-1 && (check_bit(meta[i-1], ADC_IMG_T) || check_bit(meta[i-1], ADC_ACS_T))){ 
				if (slc_ctr[last_idx] == slc_ctr[i-1] && set_ctr[last_idx] == set_ctr[i-1] && contr_ctr[last_idx] == contr_ctr[i-1] && avg_ctr[last_idx] == avg_ctr[i-1])
					acqList[last_adc].clearFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE); // flag is currently only set for last ADC in loop
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE);
				last_adc = acqList.size();
				last_idx = i-1;
			}

			// set ADC dwelltime & scan counter (support ISMRMRD-Viewer)
			acq.sample_time_us() = 1e3 * (t[i-1] - t[i-2]);
			acq.scan_counter() = scan_ctr;
			if (!acq.isFlagSet(ISMRMRD::ISMRMRD_ACQ_USER1))
				scan_ctr += 1;

			acqList.push_back(acq);
			adc_start = i;
		}
	}

	for(size_t i=0; i<acqList.size(); ++i)
		d.appendAcquisition(acqList[i]);

	// Write encoding limits
	e.encodingLimits.slice = ISMRMRD::Limit(0, slices-1, slices/2);
	e.encodingLimits.kspace_encoding_step_1 = ISMRMRD::Limit(0, shots-1, shots/2);
	e.encodingLimits.kspace_encoding_step_2 = ISMRMRD::Limit(0, partitions-1, partitions/2);
	e.encodingLimits.contrast = ISMRMRD::Limit(0, contrasts-1, contrasts/2);
	e.encodingLimits.set = ISMRMRD::Limit(0, sets-1, sets/2);
	e.encodingLimits.average = ISMRMRD::Limit(0, averages-1, averages/2);
	e.encodingLimits.segment = ISMRMRD::Limit(0, 0, 0);
	h.encoding.push_back(e);

	// Serialize header and write it to the data file
    std::stringstream str;
	ISMRMRD::serialize( h, str);
    std::string xml_header = str.str();
	d.writeHeader(xml_header);

	return img_adcs;
}
/***********************************************************/
void Sequence::OutputSeqData (map<string,string> &scanDefs, const string& outDir, const string& outFile ) {

	OutputSequenceData seqdata;
	CollectSeqData (&seqdata);
	seqdata.SetDefinitions(scanDefs);
	seqdata.WriteFiles(outDir,outFile);

}
