/** @file Model.cpp
 *  @brief Implementation of JEMRIS Model
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#include "Model.h"
#include "Sample.h"
#include "CoilArray.h"
#include "RFPulse.h"


/**************************************************/
Model::Model() {

    m_world            = World::instance();
    m_aux              = false;
    m_do_dump_progress = true;

}

/**************************************************/
void Model:: Prepare (CoilArray* pRxCoilArray, CoilArray* pTxCoilArray, ConcatSequence* pConcatSequence, Sample* pSample) {

    m_rx_coil_array    = pRxCoilArray;
    m_tx_coil_array    = pTxCoilArray;
    m_concat_sequence  = pConcatSequence;
    m_sample           = pSample;

};

/**************************************************/
void Model::Solve() {

    //problem size
    m_world->TotalSpinNumber = m_sample->GetSize();
    m_world->TotalADCNumber  = m_concat_sequence->GetNumOfADCs();

    //progress counter
    int progress_percent = -1;

    //obtain solution for each spin in the sample
    for (long lSpin=0; lSpin<m_world->TotalSpinNumber ; lSpin++) {

        m_world->SpinNumber = lSpin;
        double dTime  = 0.0;
        long   lIndex = 0;

        //Prepare sequence
        m_concat_sequence->Prepare(PREP_UPDATE);

        //get current spin properties
        m_world->Values = m_sample->GetValues(lSpin);

        //start with equilibrium solution
        m_world->solution[0]=0.0;
        m_world->solution[1]=0.0;
        m_world->solution[2]=m_world->Values[M0];

        //off-resonance from the sample
        m_world->deltaB = m_sample->GetDeltaB();

        //update progress counter
        int progr = (100*(lSpin+1)/m_world->TotalSpinNumber);

        if (m_do_dump_progress && progr != progress_percent) {
            progress_percent = progr;
            ofstream fout(".jemris_progress.out" , ios::out);
            fout << progr;
            fout.close();
        }

        //Solve while running down the sequence tree
        RunSequenceTree(dTime, lIndex, m_concat_sequence);

    }

}

/**************************************************/
void Model::RunSequenceTree (double& dTimeShift, long& lIndexShift, Module* module) {

	//recursive call for each repetition of all concat sequences
	if (module-> GetType() == MOD_CONCAT)	{

		vector<Module*> children = module->GetChildren();
		ConcatSequence* pCS = (ConcatSequence*) module;

		for (RepIter r=pCS->begin(); r<pCS->end(); ++r)
			for (unsigned int j=0; j<children.size() ; ++j)
				RunSequenceTree(dTimeShift, lIndexShift, children[j]);
	}

	//call Calculate for each TPOI in Atom
	if (module-> GetType() == MOD_ATOM)	{

		InitSolver();

		m_world->pAtom = (AtomicSequence*) module;
		vector<Module*> children = module->GetChildren();
		bool bCollectTPOIs = false;

		//dynamic changes of ADCs
		for (unsigned int j=0; j<children.size() ; ++j) {

			Pulse* p = (Pulse*) children[j];

			//Reset TPOIs for phaselocking events
			if (p->GetPhaseLock ()) {
			    p->SetTPOIs () ;
				bCollectTPOIs = true;
			}

			//set the transmitter coil (only once, i.e. at the first spin)
			if (m_world->SpinNumber == 0)
				if (p->GetAxis() == AXIS_RF)
					((RFPulse*) p)->SetCoilArray(m_tx_coil_array);
		}

		if (bCollectTPOIs) m_world->pAtom->CollectTPOIs () ;

		//Solve problem at every TPOI in the atom
		for (int i=0; i<m_world->pAtom->GetNumOfTPOIs(); ++i) {

			m_world->time            = m_world->pAtom->GetTPOIs()->GetTime(i);
			m_world->phase           = m_world->pAtom->GetTPOIs()->GetPhase(i);
			m_world->NonLinGradField = 0.0 ;

			Calculate();

			if (m_world->phase<0.0) //negative receiver phase == no ADC !
			    continue;

			m_world->time  += dTimeShift;
			m_rx_coil_array->Receive(lIndexShift++);

			//write time evolution
			if (m_world->saveEvolStepSize != 0 && lIndexShift%(m_world->saveEvolStepSize) == 0) {

                int n = lIndexShift / m_world->saveEvolStepSize  - 1;
                int N = m_world->TotalADCNumber / m_world->saveEvolStepSize ;
                int m = m_world->SpinNumber;
                int M = m_world->TotalSpinNumber;
                m_world->saveEvolFunPtr( lIndexShift, n+1 == N && m+1 == M );

			}

		}

		dTimeShift += m_world->pAtom->GetDuration();

		FreeSolver();

	}

}

/*************************************************************************/
void Model::saveEvolution (long index, bool close_files) {

    World* pW = World::instance();

    if ( (pW->saveEvolFileName).empty() || pW->saveEvolStepSize==0 )
        return;

    int n = index / pW->saveEvolStepSize - 1;
    int N = pW->TotalADCNumber / pW->saveEvolStepSize ;

    int m = pW->SpinNumber ;
    int M = pW->TotalSpinNumber ;

    //create ofstreams at first call
    if ( pW->saveEvolOfstream == NULL ) pW->saveEvolOfstream = new ofstream[N];

    // If not yet done, open file and write information
    // of sample size and current evol-time-point
    if ( !pW->saveEvolOfstream[n].is_open() ) {

        stringstream sF;
        sF << pW->saveEvolFileName << "_" << setw(3) << setfill('0') << n+1 << ".bin";
        pW->saveEvolOfstream[n].open( (sF.str()).c_str() , ios::binary);

        double dNumOfSpins = ((double) M);

        pW->saveEvolOfstream[n].write((char *) &(dNumOfSpins), sizeof(dNumOfSpins));
        pW->saveEvolOfstream[n].write((char *) &(pW->time)   , sizeof(pW->time )  );

    }

    //write current magnetisation state of this spin
    double Mx = pW->solution[AMPL]* cos (pW->solution[PHASE]) ;
    double My = pW->solution[AMPL]* sin (pW->solution[PHASE]) ;
    double dSpinNumber = ((double) pW->SpinNumber );
    pW->saveEvolOfstream[n].write((char *) &(dSpinNumber)     , sizeof(dSpinNumber)     );
    pW->saveEvolOfstream[n].write((char *) &(pW->Values[XC])  , sizeof(pW->Values[XC])  );
    pW->saveEvolOfstream[n].write((char *) &(pW->Values[YC])  , sizeof(pW->Values[YC])  );
    pW->saveEvolOfstream[n].write((char *) &(pW->Values[ZC])  , sizeof(pW->Values[ZC])  );
    pW->saveEvolOfstream[n].write((char *) &(Mx)              , sizeof(Mx)              );
    pW->saveEvolOfstream[n].write((char *) &(My)              , sizeof(My)              );
    pW->saveEvolOfstream[n].write((char *) &(pW->solution[ZC]), sizeof(pW->solution[ZC]));

    // at the last call close all files
    if (close_files) {
        for (int i=0;i<N;++i)
            pW->saveEvolOfstream[i].close() ;
        delete[] pW->saveEvolOfstream ;
        pW->saveEvolOfstream = NULL;
    }

    return;

}
