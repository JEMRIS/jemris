/** @file Sequence.cpp
 *  @brief Implementation of JEMRIS Sequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

/***********************************************************/
bool    Sequence::Prepare(PrepareMode mode){

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
void Sequence::SeqDiag (string fname ) {

	//prepare H5 file structure
	BinaryContext bc;
	DataInfo      di;
	int numaxes = 7;

	bc.Initialize ("seq.h5", IO::OUT);
	if (bc.Status() != IO::OK) return;

	di.fname = "seq.h5";
	di.ndim  = 1;
	di.dims[0] = GetNumOfTPOIs()+1;
	di.path  = "seqdiag";
	
	vector<double*> seqdata;
	for (int i=0; i<numaxes; i++) {
		seqdata.push_back(new double [GetNumOfTPOIs()+1]);
		*(seqdata[i]) = (i==1?-1.0:0.0); //always start seq.-diag. with {0,-1,0,0,0,0,0} !
		seqdata[i]++;
	}
	
	//HDF5 dataset names
	vector<string> seqaxis;
	seqaxis.push_back("T");		//Time
	seqaxis.push_back("RXP");	//receiver phase
	seqaxis.push_back("TXM");	//transmitter magnitude
	seqaxis.push_back("TXP");	//transmitter phase
	seqaxis.push_back("GX");	//X gradient
	seqaxis.push_back("GY");	//Y gradient
	seqaxis.push_back("GZ");	//Z gradient

	//recursive data collect
	double time   = 0.0;
	long   offset = 0;
	CollectSeqData(seqdata,time,offset);


	//write to HDF5 file
	for (int i=0; i<numaxes; i++) {
		di.dname = seqaxis[i];
		bc.SetInfo(di);
		seqdata[i]--;
		bc.WriteData(seqdata[i]);
		delete[] seqdata[i];
	}

}

/***********************************************************/
void  Sequence::CollectSeqData  (vector<double*> seqdata, double& time, long& offset) {
	  
	if (GetType() == MOD_CONCAT) {

		vector<Module*> children = GetChildren();
		ConcatSequence* pSeq     = ((ConcatSequence*) this);

		for (RepIter r=pSeq->begin(); r<pSeq->end(); ++r) {

			for (unsigned int j=0; j<children.size() ; ++j) {

				((Sequence*) children[j])->CollectSeqData(seqdata,time,offset);
				if (children[j]->GetType() != MOD_CONCAT) {
					time   += children[j]->GetDuration();
					offset += children[j]->GetNumOfTPOIs();
				}
			}
		}
	}

	if (GetType() == MOD_ATOM) {
	  
		//copy seqdata values at each TPOI
		for (int i=0; i<GetNumOfTPOIs(); ++i) {

			double dt,dp;
			dt = m_tpoi.GetTime(i);
			dp = m_tpoi.GetPhase(i);
			  
			double val[7] = {0.0,0.0,0.0,0.0,0.0};
			val[0]=time+dt;
			val[1]=dp;
			//here, nonlinear gradients are not taken into account for GetValue
			bool rem  = ((AtomicSequence*) this)->HasNonLinGrad();
			((AtomicSequence*) this)->SetNonLinGrad(false);
			//GetValue(val,dt+k*GetDuration()/1e9);
			GetValue(&val[2],dt);
			((AtomicSequence*) this)->SetNonLinGrad(rem);
			
			for (int j=0; j<seqdata.size(); ++j)
				  *(seqdata[j]+offset+i) = val[j];

		}

	}

}

/***********************************************************/
long  Sequence::GetNumOfADCs () {

	if (GetType() == MOD_CONCAT) {

		long lADC = 0;
		vector<Module*> children = GetChildren();
		ConcatSequence* pSeq     = ((ConcatSequence*) this);

		for (RepIter r=pSeq->begin(); r<pSeq->end(); ++r) {

			for (unsigned int j=0; j<children.size() ; ++j)
					lADC += ((Sequence*) children[j])->GetNumOfADCs();

		}

		return lADC;

	}

	if (GetType() == MOD_ATOM) {

		int iADC = GetNumOfTPOIs();

		for (int i=0; i<GetNumOfTPOIs(); ++i)
			if (m_tpoi.GetPhase(i) < 0.0)  iADC--;

		return iADC;

	}

	return -1;

}
