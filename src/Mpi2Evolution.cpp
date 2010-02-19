/*
 * Mpi2Evolution.cpp
 *
 *  Created on: Jan 13, 2010
 *      Author: dpflug
 */

#include "Mpi2Evolution.h"
#include "World.h"
#include <cmath>
#include "config.h"

// init variables:
#ifdef HAVE_MPI_THREADS
vector<MPI::File> 		Mpi2Evolution::m_files;
#endif
vector<bool>			Mpi2Evolution::m_first_write;


/*****************************************************************************/
Mpi2Evolution::Mpi2Evolution() {
}

/*****************************************************************************/
void SetSaveFunction() {
	World* pW = World::instance();
	pW->saveEvolFunPtr = &Mpi2Evolution::saveEvolution;
};

/*****************************************************************************/
void Mpi2Evolution::OpenFiles(int is_restart){
	World* pW= World::instance();
	if (pW->saveEvolStepSize == 0) return;


#ifndef HAVE_MPI_THREADS
	if (pW->m_myRank == 0)
		cout << "\n!!! Warning: !!!\nSaving spin evolution in parallel JEMRIS mode requires MPI2.0 (parallel file I/O). no evol files will be written.\nUse MPI2.0 or sequential jemris.\n" << endl;
#else
	long M     = pW->TotalADCNumber / pW->saveEvolStepSize ;
	string fname;
	int SpinNo = pW->TotalSpinNumber;

	MPI::COMM_WORLD.Bcast(&is_restart,1,MPI_INT,0);

	MPI::COMM_WORLD.Bcast(&SpinNo,1,MPI_INT,0);
	MPI::Offset filesize;
	filesize = (SpinNo * 7 +2)* sizeof(double);

	for (int i=0; i<M; i++) {
        stringstream sF;
        sF << pW->saveEvolFileName << "_" << setw(3) << setfill('0') << i+1 << ".bin";
        // delete existing old file; (c) by Rolf Rabenseifer...
        if (is_restart != 1){
        	MPI::File fh=MPI::File::Open(MPI::COMM_WORLD,(sF.str()).c_str(),MPI::MODE_DELETE_ON_CLOSE | MPI::MODE_CREATE | MPI::MODE_WRONLY, MPI::INFO_NULL  );
        	fh.Close();
        }
        m_files.push_back(MPI::File::Open(MPI::COMM_WORLD,(sF.str()).c_str(),MPI::MODE_WRONLY | MPI::MODE_CREATE, MPI::INFO_NULL  ));

		m_files[i].Preallocate(filesize);
		if (pW->m_myRank == 1) {
			double dummy = (double) SpinNo;
			m_files[i].Write_at(0,&dummy,1,MPI_DOUBLE);
			m_first_write.push_back(true);
		}
	}

#endif

}


/*****************************************************************************/
void Mpi2Evolution::CloseFiles(){
#ifdef HAVE_MPI_THREADS
	for (int i=0; i<m_files.size(); i++) {
		m_files[i].Close();
	}
#endif
}
/*****************************************************************************/
void Mpi2Evolution::saveEvolution(long index, bool close_files) {
#ifdef HAVE_MPI_THREADS

	   World* pW = World::instance();
	    if ( (pW->saveEvolFileName).empty() || pW->saveEvolStepSize==0 )
	        return;

	    int n = index / pW->saveEvolStepSize - 1;
	    int N = pW->TotalADCNumber / pW->saveEvolStepSize ;

	    //write timepoint at first call
	    if (pW->m_myRank==1 ) {
	    	if (m_first_write[n]) {
	    		m_first_write[n]=false;
				m_files[n].Write_at(1*sizeof(double),&(pW->time),1,MPI_DOUBLE);
	    	}
	    }

	    //write current magnetisation state of this spin
	    double Mx = pW->solution[AMPL]* cos (pW->solution[PHASE]) ;
	    double My = pW->solution[AMPL]* sin (pW->solution[PHASE]) ;
	    double tmp[7];
	    tmp[0]=pW->Values[ID];
	    tmp[1]=pW->Values[XC];
	    tmp[2]=pW->Values[YC];
	    tmp[3]=pW->Values[ZC];
	    tmp[4]=Mx;
	    tmp[5]=My;
	    tmp[6]=pW->solution[ZC];

	    MPI::Offset offset = (2+7*pW->Values[ID])*sizeof(double);
	    m_files[n].Write_at(offset,tmp,7,MPI_DOUBLE);


	    return;
#endif

}