/** @file Sequence.cpp
 *  @brief Implementation of JEMRIS Sequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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
	NDData<double> seqdata(numaxes+1,GetNumOfTPOIs()+1);	/** Extra axis for META */
	
	// HDF5 dataset names
	vector<string> seqaxis;
	seqaxis.push_back("T");							//Time
	seqaxis.push_back("RXP");						//receiver phase
	seqaxis.push_back("TXM");						//transmitter magnitude
	seqaxis.push_back("TXP");						//transmitter phase
	std::string gradSuffixes = "XYZ";
	for (int i=0; i<gradSuffixes.length(); i++)
		seqaxis.push_back(std::string("G") + gradSuffixes[i]);	//Patloc and Linear gradients
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
void Sequence::OutputSeqData (map<string,string> &scanDefs, const string& outDir, const string& outFile ) {

	OutputSequenceData seqdata;
	CollectSeqData (&seqdata);
	seqdata.SetDefinitions(scanDefs);
	seqdata.WriteFiles(outDir,outFile);

}
