/** @file Model.cpp
 *  @brief Implementation of JEMRIS Model
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as publadched by
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
#include "DynamicVariables.h"
#include "config.h"

#ifdef HAVE_MPI_THREADS
#include "mpi.h"
#endif

#include "time.h"
#include "Trajectory.h"

/**************************************************/
Model::Model() : m_tx_coil_array(0), m_sample(0), m_rx_coil_array(0), m_concat_sequence(0) {

    m_world            = World::instance();
    m_aux              = false;
    m_do_dump_progress = true;
    m_accuracy_factor  = 1.0;

}

/**************************************************/
void Model:: Prepare (CoilArray* pRxCoilArray, CoilArray* pTxCoilArray, ConcatSequence* pConcatSequence, Sample* pSample) {

    m_rx_coil_array    = pRxCoilArray;
    m_tx_coil_array    = pTxCoilArray;
    m_concat_sequence  = pConcatSequence;
    m_sample           = pSample;

}

/**************************************************/
void Model::Solve() {

    //problem size
    m_world->TotalSpinNumber = m_sample->GetSize();
	m_world->SetNoOfCompartments(m_sample->GetNoSpinCompartments());
	m_world->SetNoOfSpinProps(m_sample->GetNProps());
    m_world->TotalADCNumber  = m_concat_sequence->GetNumOfADCs();

    //obtain solution for each spin in the sample
    for (long lSpin=m_world->m_startSpin; lSpin<m_world->TotalSpinNumber ; lSpin++) {

        m_world->SpinNumber = lSpin;
        double dTime  = 0.0;
        long   lIndex = 0;
		
        //Prepare sequence
        m_concat_sequence->Prepare(PREP_INIT);
		
        //get current spin properties
        m_sample->GetValues(lSpin, m_world->Values);

		// Get helper size, initialise and fill.
		m_world->InitHelper(m_sample->GetHelperSize());
		m_sample->GetHelper(m_world->Helper());
		
        //check for activation
        DynamicVariables*  dynvar = DynamicVariables::instance();
        dynvar->SetActivation();
        dynvar->m_Diffusion->UpdateTrajectory(true);

        int m_ncoprops =  (m_world->GetNoOfSpinProps () - 4) / m_world->GetNoOfCompartments();
        //start with equilibrium solution
		for (int i = 0; i < m_world->GetNoOfCompartments(); i++) {
			//start with equilibrium solution
			m_world->solution[0+i*3]=0.0;
			m_world->solution[1+i*3]=0.0;
			m_world->solution[2+i*3]=m_world->Values[i*m_ncoprops+3]; // Values in world [0] to [2] are the x,y,z coordinates, followed by the M0, R1, R2, DB for each pool

		//	cout <<"im Model solution initatilsation" << " Mz "<< m_world->solution[2+i*3]<<" Mx " << m_world->solution[0+i*3]<< " My "<< m_world->solution[1+i*3]<< endl;
		}

        //off-resonance from the sample
        m_world->deltaB = m_sample->GetDeltaB();
		
        //update progress counter
        if (m_do_dump_progress)
		 	UpdateProcessCounter(lSpin);
		
        //Solve while running down the sequence tree
        RunSequenceTree(dTime, lIndex, m_concat_sequence);

        //dump restart info:
        DumpRestartInfo(lSpin);

    }

}

/**************************************************/
void Model::RunSequenceTree (double& dTimeShift, long& lIndexShift, Module* module) {

	int ncomp  = m_world->GetNoOfCompartments();
	int nprops = m_world->GetNoOfSpinProps();
	int cprops = (nprops - 4) / ncomp;
	
	//recursive call for each repetition of all concat sequences
	if (module-> GetType() == MOD_CONCAT)	{
		
		vector<Module*> children = module->GetChildren();
		ConcatSequence* pCS      = (ConcatSequence*) module;
		
		for (RepIter r=pCS->begin(); r<pCS->end(); ++r)
			for (unsigned int j=0; j<children.size() ; ++j)
				RunSequenceTree(dTimeShift, lIndexShift, children[j]);

	}
	
	//call Calculate for each TPOI in Atom
	if (module-> GetType() == MOD_ATOM)	{
		
		m_world->pAtom = (AtomicSequence*) module;
		InitSolver();
		
		vector<Module*> children      = module->GetChildren();
		bool            bCollectTPOIs = false;
		
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
					((RFPulse*) p)->SetCoilArray (m_tx_coil_array);
		}
		

		//temporary storage
		double  dtsh = dTimeShift;
		long    ladc = lIndexShift;
		int     iadc = m_world->pAtom->GetNumOfADCs();
		std::vector<double> dmxy (iadc*m_world->GetNoOfCompartments());
		std::vector<double> dmph (iadc*m_world->GetNoOfCompartments());
		std::vector<double> dmz  (iadc*m_world->GetNoOfCompartments());
		double  dMt  = m_world->solution[0];
		double  dMp  = m_world->solution[1];
		double  dMz  = m_world->solution[2];
		
		iadc=0;

		if (bCollectTPOIs)
			m_world->pAtom->CollectTPOIs () ;

		//Solve problem at every TPOI in the atom
		m_world->total_time = dTimeShift;
		// forces CVode to calculate bloch() at this timepoint
		double next_tStop = -1.0;
		int noTPOIS = m_world->pAtom->GetNumOfTPOIs();
		for (int i=0; i<noTPOIS; ++i) {

			m_world->time            = m_world->pAtom->GetTPOIs()->GetTime(i);
			m_world->phase           = m_world->pAtom->GetTPOIs()->GetPhase(i);
			m_world->NonLinGradField = 0.0 ;

			// search next tStop:
			if (next_tStop < m_world->time) {
				int  j          = i+1;
				bool found_next = false;
				while((j<noTPOIS) && (!found_next)) {
					if (m_world->pAtom->GetTPOIs()->GetPhase(j) < 0.0) {
						next_tStop =  m_world->pAtom->GetTPOIs()->GetTime(j);
						found_next = true;
					}
					j++;
				}
				if (found_next == false) next_tStop = 1e200;
			}


			//if numerical or occures in calculation, repeat the current atom with increased accuracy
			if (!Calculate(next_tStop)) {				
				//remove wrong contribution to the signal(s)
				iadc=0;
				for (int j=0; j < i; ++j) {
				  m_world->phase = m_world->pAtom->GetTPOIs()->GetPhase(j);
				  if (m_world->phase < 0.0) continue;
				  m_world->time  = dtsh + m_world->pAtom->GetTPOIs()->GetTime(j);
				  for (int k = 0; k < m_world->GetNoOfCompartments(); k++) {
					  int os = k*3;
					  m_world->solution[os+AMPL]  = -dmxy[os+iadc];
					  m_world->solution[os+PHASE] =  dmph[os+iadc];
					  m_world->solution[os+ZC]    =  -dmz[os+iadc];
				  }
				  m_rx_coil_array->Receive(ladc+iadc);
				  iadc++;
				}
				
				FreeSolver();				
				
				m_accuracy_factor *= 0.1; // increase accuray by factor 1e-3
				m_world->solution[0] = dMt;
				m_world->solution[1] = dMp;
				m_world->solution[2] = dMz;
				RunSequenceTree(dtsh, ladc, m_world->pAtom);
				dTimeShift  = dtsh;
				lIndexShift = ladc;
				m_accuracy_factor *= 10.0; // back to default  accuray
				return;
			}

			if (m_world->phase < 0.0)
				continue;	//negative receiver phase == no ADC !

			m_world->time  += dTimeShift;
			m_rx_coil_array->Receive(lIndexShift++);
			
			//temporary storage of solution
			dmxy[iadc] = m_world->solution[AMPL];
			dmph[iadc] = m_world->solution[PHASE];
			dmz[iadc]  = m_world->solution[ZC];

			iadc++;

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

		m_world->pAtom->UpdateEddyCurrents();
		m_world->pAtom->PrepareEddyCurrents();

	}

}

/*************************************************************************/
void Model::saveEvolution (long index, bool close_files) {

    World* pW = World::instance();

    if ( (pW->saveEvolFileName).empty() || pW->saveEvolStepSize==0 )
        return;

    int n = index / pW->saveEvolStepSize - 1;
    int N = pW->TotalADCNumber / pW->saveEvolStepSize ;

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
/*************************************************************************/
void Model::DumpRestartInfo(long lSpin){
	// serial jemris only:
	if (m_world->m_myRank < 0) {
		static long lastspin=0;
		static time_t lasttime=time(NULL);;
		int WaitTime=30; //dump restart info every 10s.

		if ((time(NULL)-lasttime)>WaitTime ){
			m_sample->ReportSpin(lastspin,lSpin,2);
			m_sample->DumpRestartInfo(m_rx_coil_array);
			lastspin=lSpin+1;
			lasttime=time(NULL);
		}
	}
}

/*************************************************************************/
inline static void progressbar (int percent) {

	static string bars   = "***************************************************";
	static string blancs = "                                                   ";

	cout << "\rSimulating | "; 
	cout << bars.substr(0, percent/2) << " " <<  blancs.substr(0, 50-percent/2) << "| " <<setw(3) << setfill(' ') << percent << "% done";

	flush(cout);

}

/*************************************************************************/
inline void Model::UpdateProcessCounter (const long lSpin) {
	
	if ((m_world->m_myRank > 0 )){

		// parallel jemris:

#ifdef HAVE_MPI_THREADS
		// with pthreads: use continuous progress bar:
		//update progress counter (parallel jemris)
		static time_t lasttime=time(NULL);
		static long lastspin=lSpin-1;
		int spinsDone =0;
		int WaitTime=2; //update progress every 2s
		
		if (((time(NULL)-lasttime)>WaitTime ) || (lSpin + 1 == m_world->TotalSpinNumber )) {
			spinsDone = lSpin - lastspin;
			MPI_Send(&spinsDone,1,MPI_INT,0,SPINS_PROGRESS,MPI_COMM_WORLD);
			
			if (lSpin + 1 == m_world->TotalSpinNumber )
				lastspin = -1;
			else
				lastspin = lSpin;

			lasttime = time(NULL);
		}
#endif

	} else {

		static int progress_percent = -1;

		//update progress counter (serial jemris/pjemris without threads support)
		int progr = (100*(lSpin+1)/m_world->TotalSpinNumber);

		if (progr != progress_percent) {
			progress_percent = progr;
			ofstream fout(".jemris_progress.out" , ios::out);
			fout << progr;
			fout.close();
			progressbar(progr);
		}
	}




}

