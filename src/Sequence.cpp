/** @file Sequence.cpp
 *  @brief Implementation of JEMRIS Sequence
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

#include "Sequence.h"
#include "ConcatSequence.h"
#include "AtomicSequence.h"

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

};

/***********************************************************/
void Sequence::SeqDiag (string fname ) {

    ofstream fout(fname.c_str() , ios::binary);
	double time = 0.0;
	WriteSeqFile(&fout,time);
	fout.close();

};

/***********************************************************/
void  Sequence::WriteSeqFile (ofstream* pfout, double& time) {

	if (GetType() == MOD_CONCAT) {

		vector<Module*> children = GetChildren();
		ConcatSequence* pSeq     = ((ConcatSequence*) this);

		for (RepIter r=pSeq->begin(); r<pSeq->end(); ++r) {

			for (unsigned int j=0; j<children.size() ; ++j) {

				((Sequence*) children[j])->WriteSeqFile(pfout,time);
				if (children[j]->GetType() != MOD_CONCAT)
					time += children[j]->GetDuration();
			}
		}
	}

	if (GetType() == MOD_ATOM) {

		//write value to the binary file at each TPOI
		for (int i=0; i<=GetNumOfTPOIs(); ++i) {

			double dt,dp;
			if (i==0)	{ dt = GetDuration()/1e9; dp=-1.0; }
			else		{ dt = m_tpoi.GetTime(i-1); dp = m_tpoi.GetPhase(i-1); }
			double t  = time + dt;

			// two time-points at each TPOI, one slightly shifted, to
			// ensure proper display of the pulse diagram in all cases
			int km = ((i>0&&i<GetNumOfTPOIs())?2:1);
			for (int k=0;k<km;k++) {

				pfout->write((char *)(&(t)),sizeof(t));
				pfout->write((char *)(&(dp)),sizeof(dp));

				double val[5] = {0.0,0.0,0.0,0.0,0.0};

				//here, nonlinear gradients are not taken into account for GetValue
				bool rem  = ((AtomicSequence*) this)->HasNonLinGrad();
				((AtomicSequence*) this)->SetNonLinGrad(false);
				GetValue(val,dt+k*GetDuration()/1e9);
				((AtomicSequence*) this)->SetNonLinGrad(rem);

				for (unsigned int j=0;j<5;++j)
					pfout->write((char *)(&(val[j])),sizeof(val[j]));
			}

		}

	}

};

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
