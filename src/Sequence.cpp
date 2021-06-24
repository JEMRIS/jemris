/** @file Sequence.cpp
 *  @brief Implementation of JEMRIS Sequence
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

#include "Sequence.h"
#include "ConcatSequence.h"
#include "AtomicSequence.h"
#include <stdio.h>
#include "BinaryContext.h"
#include "EddyPulse.h"

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
	NDData<double> seqdata(numaxes+3,GetNumOfTPOIs()+1);	/** Extra axes for META, slice and shot number*/
	
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
	seqdata (numaxes+1,0) = 0;
	seqdata (numaxes+2,0) = 0;
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
void Sequence::SeqISMRMRD (const string& fname ) {

	/* WIP: - Add counters from other loops to ISMRMRD file (use only slice counter or set,slice,(averages)???)
		    - Plot reconstruction results in GUI
		    - Check if the trajectory at the TPOi's is matching the trajectory at the sampling points in the Pulseq file */

	if ( GetNumOfTPOIs()==0  ) return;

	Prepare(PREP_INIT);

	NDData<double>      di (GetNumOfTPOIs() + 1);
	std::vector<double>  t (GetNumOfTPOIs() + 1);
	std::vector<size_t>  meta (GetNumOfTPOIs() + 1);
	std::vector<size_t>  slc_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  shot_ctr (GetNumOfTPOIs() + 1);

	int numaxes = (MAX_SEQ_VAL+1)+2;	/** Two extra: time, receiver phase */

	// Start with 0 and track excitations and refocusing
	NDData<double> seqdata(numaxes+3,GetNumOfTPOIs()+1);	/** Extra axes for META, slice number and last scan in slice*/
	

	//turn off nonlinear gradients in static events for sequence diagram calculation
	// needed???
	World* pW = World::instance();
	if (pW->pStaticAtom != NULL) pW->pStaticAtom->SetNonLinGrad(false);

	// recursive data collect
	double seqtime=  0.;
	long   offset =  0l;
	seqdata (1,0) = -1.;
	seqdata (numaxes+1,0) = 0;
	seqdata (numaxes+2,0) = 0;
	pW->m_slice = 0; // reset singletons
	pW->m_shot = 0;
	pW->m_shotmax = 0;
	CollectSeqData (seqdata, seqtime, offset);

	// Transpose Data and collect meta and t-vector for cumtrapz
	seqdata = transpose(seqdata);
	std::copy (&seqdata[0], &seqdata[0]+di.Size(), t.begin());
	for (size_t i = 1; i < meta.size(); ++i)
		meta[i] = seqdata(i,numaxes);

	// Slice and shot information
	for (size_t i = 1; i < slc_ctr.size(); ++i)
		slc_ctr[i] = seqdata(i,numaxes+1);
	for (size_t i = 1; i < shot_ctr.size(); ++i)
		shot_ctr[i] = seqdata(i,numaxes+2);

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

	// Set system info to not crash reco
	sys.systemVendor.set("JEMRIS");
	sys.systemModel.set("v2.8.4");
	sys.systemFieldStrength_T.set(0);
	h.acquisitionSystemInformation.set(sys);

	// Encoding
	Parameters* P = Parameters::instance();
	ISMRMRD::Encoding e;
	e.trajectory = ISMRMRD::TrajectoryType::OTHER;
	e.encodedSpace.matrixSize.x = P->m_iNx;
	e.encodedSpace.matrixSize.y = P->m_iNy;
	e.encodedSpace.matrixSize.z = P->m_iNz;
	e.encodedSpace.fieldOfView_mm.x = P->m_fov_x;
	e.encodedSpace.fieldOfView_mm.y = P->m_fov_y;
	e.encodedSpace.fieldOfView_mm.z = P->m_fov_z;
	e.reconSpace.matrixSize.x = P->m_iNx;
	e.reconSpace.matrixSize.y = P->m_iNy;
	e.reconSpace.matrixSize.z = P->m_iNz;
	e.reconSpace.fieldOfView_mm.x = P->m_fov_x;
	e.reconSpace.fieldOfView_mm.y = P->m_fov_y;
	e.reconSpace.fieldOfView_mm.z = P->m_fov_z;

	// Acquisitions
	std::vector<ISMRMRD::Acquisition> acqList;
	ISMRMRD::Acquisition acq;
	u_int16_t axes = 3;
	u_int16_t readout;

	size_t adc_start = 0;
	size_t last_adc = 0; // helper variable for last scan in slice
	for (size_t i = 1; i < meta.size(); ++i){
		if (meta[i] != meta[i-1] || i == meta.size()-1){
			acq.clearAllFlags();
			readout = i - adc_start;
			acq.resize(readout, acq.active_channels(), axes);
			for (size_t k = 0; k<readout; ++k){
				acq.traj(0,k) = kx[k+adc_start];
				acq.traj(1,k) = ky[k+adc_start];
				acq.traj(2,k) = kz[k+adc_start];
			}

			if (meta[i-1] == 1)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_DUMMYSCAN_DATA); // ADCs with no specific purpose
			else if (meta[i-1] == 4)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION);
			else if (meta[i-1] == 8)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PHASECORR_DATA);
			else if (meta[i-1] == 16)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_NOISE_MEASUREMENT);
			else if (meta[i-1] != 2)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_USER1); // TPOI's without ADCs get user1 flag

			acq.idx().slice = slc_ctr[i-1];
			if (shot_ctr[i-1] == pW->m_shotmax-1 && meta[i-1] == 2){ // set last scan in slice - WIP: partitions are not yet supported
				if (slc_ctr[last_adc] == slc_ctr[i-1])
					acqList[last_adc].clearFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE); // flag is only set for last ADC in loop
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE);
				last_adc = acqList.size();
			}
			acqList.push_back(acq);
			adc_start = i;
		}
	}

	for(size_t i=0; i<acqList.size(); ++i)
		d.appendAcquisition(acqList[i]);

	// Write maximum slice number
	int slices = *max_element(slc_ctr.begin(), slc_ctr.end()) + 1;
	e.encodingLimits.slice = ISMRMRD::Limit(0, slices-1, slices/2);
	h.encoding.push_back(e);

	// Serialize header and write it to the data file
    std::stringstream str;
	ISMRMRD::serialize( h, str);
    std::string xml_header = str.str();
	d.writeHeader(xml_header);

}
/***********************************************************/
void Sequence::OutputSeqData (map<string,string> &scanDefs, const string& outDir, const string& outFile ) {

	OutputSequenceData seqdata;
	CollectSeqData (&seqdata);
	seqdata.SetDefinitions(scanDefs);
	seqdata.WriteFiles(outDir,outFile);

}
