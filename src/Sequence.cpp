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
	NDData<double> seqdata(numaxes+3,GetNumOfTPOIs()+1);	/** Extra axes for META, slice number and last scan in slice*/
	
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

	if ( GetNumOfTPOIs()==0  ) return;

	Prepare(PREP_INIT);

	NDData<double>      di (GetNumOfTPOIs() + 1);
	std::vector<double>  t (GetNumOfTPOIs() + 1);
	std::vector<size_t>  meta (GetNumOfTPOIs() + 1);
	std::vector<size_t>  slc_ctr (GetNumOfTPOIs() + 1);
	std::vector<size_t>  lastScaninSlice (GetNumOfTPOIs() + 1);

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
	CollectSeqData (seqdata, seqtime, offset);

	// Transpose Data and collect meta and t-vector for cumtrapz
	seqdata = transpose(seqdata);
	std::copy (&seqdata[0], &seqdata[0]+di.Size(), t.begin());
	for (size_t i = 1; i < meta.size(); ++i)
		meta[i] = seqdata(i,numaxes);

	// Slice information
	for (size_t i = 1; i < meta.size(); ++i)
		slc_ctr[i] = seqdata(i,numaxes+1);
	for (size_t i = 1; i < meta.size(); ++i)
		lastScaninSlice[i] = seqdata(i,numaxes+2);

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
	/** ToDo: - future: set all counters as slices, averages, contrasts etc. with a looptype parameter
			  	e.g 1=slice loop, 2=shot loop, 4=contrast loop etc.
			  - Check if the trajectory at the TPOi's is matching the trajectory at the sampling points in the Pulseq file
			  if simulation:
			  	in CoilArray.cpp:
				1. #include "SequenceTree.h" + #include "ConcatSequence.h"
				2. Get sequence:
					SequenceTree seqTree;
					seqTree.Initialize(input);
					seqTree.Populate();
					ConcatSequence* seq = seqTree.GetRootConcatSequence();
				3. seq->SeqISMRMRD(m_signal_output_dir + m_signal_prefix + ".h5");
				4. Append signal and coil data
	*/
	std::remove(fname.c_str()); // otherwise data is appended
	ISMRMRD::Dataset d(fname.c_str(), "dataset", true);

	// Header
	ISMRMRD::IsmrmrdHeader h;
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
	ISMRMRD::Acquisition acq;
	u_int16_t axes = 3;
	u_int16_t readout;

	size_t adc_start = 0;
	int slices = 0; // WIP: get total slice number from slice flag
	for (size_t i = 1; i < meta.size(); ++i){
		if (meta[i] != meta[i-1]){
			acq.clearAllFlags();
			readout = i - adc_start;
			acq.resize(readout, acq.active_channels(), axes);
			for (size_t k = 0; k<readout; ++k){
				acq.traj(0,k) = kx[k+adc_start];
				acq.traj(1,k) = ky[k+adc_start];
				acq.traj(2,k) = kz[k+adc_start];
			}

			if (meta[i-1] == 1)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_NAVIGATION_DATA); // misuse navigation data flag for ADCs with no specific purpose
			else if (meta[i-1] == 4)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PARALLEL_CALIBRATION);
			else if (meta[i-1] == 8)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_PHASECORR_DATA);
			else if (meta[i-1] != 2)
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_IS_DUMMYSCAN_DATA); // TPOI's without ADCs get dummyscan flag, imaging scans get no flag

			acq.idx().slice = slc_ctr[i];
			if (lastScaninSlice[i])
				acq.setFlag(ISMRMRD::ISMRMRD_ACQ_LAST_IN_SLICE);
			d.appendAcquisition(acq);
			adc_start = i;
		}
	}

	// Write maximum slice number
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
