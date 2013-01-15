/** @file mpi_Model.h
 *  @brief Implementation of JEMRIS mpi structures/functions for send/receive of sample/signal and of JEMRIS mpiEvolution
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


#ifndef _MPI_MODEL_H_
#define _MPI_MODEL_H_

#include<iostream>
#include "config.h"

using namespace std;
#define MPICH_IGNORE_CXX_SEEK

#include <mpi.h>

#include "Signal.h"
#include "Declarations.h"
#include "Sample.h"

#ifdef HAVE_MPI_THREADS
	#include <pthread.h>
#endif

void mpi_send_paket_signal(Signal* pSig,int CoilID);
void mpi_recv_paket_signal(Signal* pSig,int SlaveID,int CoilID);

/*****************************************************************************/
MPI_Datatype  MPIspindata() {

	int    NUM_DATA = World::instance()->GetNoOfSpinProps();

    MPI_Datatype MPI_SPINDATA ;

	MPI_Aint     disp = 0;
	MPI_Datatype type = MPI_DOUBLE;
	

    MPI_Type_struct( 1, &NUM_DATA, &disp, &type, &MPI_SPINDATA);
    MPI_Type_commit( &MPI_SPINDATA);

    return  MPI_SPINDATA;

}

/*****************************************************************************/
/* function that does the progress counting; called by an extra thread from the master process */
#ifdef HAVE_MPI_THREADS
/*************************************************************************/

void* CountProgress(void * arg) {

	int TotalNoSpins  = *((int *) arg);
	int SpinsDone = TotalNoSpins - *(((int *) arg )+1);
	int NowDone=0;
	int progress_percent = -1;
	static string bars   = "***************************************************";
	static string blancs = "                                                   ";

	while (SpinsDone != TotalNoSpins) {
		MPI::COMM_WORLD.Recv(&NowDone,1,MPI_INT,MPI_ANY_SOURCE,SPINS_PROGRESS);
		SpinsDone += NowDone;

        //update progress counter
		int progr = (100*(SpinsDone+1)/TotalNoSpins);
		if ( progr != progress_percent) {
			progress_percent = progr;
			ofstream fout(".jemris_progress.out" , ios::out);
			fout << progr;
			fout.close();

			cout << "\rSimulating | "; 
			cout << bars.substr(0, progr/2) << " " <<  blancs.substr(0, 50-progr/2) << "| " <<setw(3) << setfill(' ') << progr << "% done";

			flush(cout);

		}
	}

	return new void*;

}
#endif

/*****************************************************************************/
void mpi_devide_and_send_sample (Sample* pSam, CoilArray* RxCA ) {

#ifdef HAVE_MPI_THREADS
	// copy + paste thread example
	pthread_t counter_thread;
	int errcode;                                /* holds pthread error code */

	int buffer[2];
	buffer[0] = pSam->GetSize();
	buffer[1] = pSam->SpinsLeft();

	errcode=pthread_create(&counter_thread,	/* thread struct             */
						   NULL,                      	/* default thread attributes */
						   CountProgress,              /* start routine             */
						   (void *) &buffer);

    if (errcode) {       /* arg to routine            */
    	cout << "thread creation failed !! exit."<< endl;
    	exit (-1);
    }
	// end copy + paste thread example
#endif

	int size = MPI::COMM_WORLD.Get_size();
	int ilen;
    char hm[MPI_MAX_PROCESSOR_NAME];
    char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);

	cout << endl << hostname << " -> Master Process: send MR sample (" << pSam->GetSize()
		 << " spins) to " << size-1 << " slave(s)"<< endl << endl << flush;

	int*     sendcount; 		    // array with NumberOfSpins[slaveid]
	int*     displs;
	int      recvbuf;
	double*  recvdummy = 0;	// dummy buffer

	// create sendcounts, displs
	sendcount 	 = new int[size];
	displs		 = new int[size];

	// no spin data for master:
	displs[0]	 = 0;
	sendcount[0] = 0;
	
	pSam->GetScatterVectors(sendcount,displs,size);

	// scatter sendcounts:
	MPI::COMM_WORLD.Scatter  (sendcount, 1, MPI_INT, &recvbuf, 1, MPI_INT, 0);

	// broadcast number of individual spin prioperties:
	MPI::COMM_WORLD.Bcast    (pSam->GetSampleDims(),1,MPI_LONG,0);

	long hsize = pSam->GetHelperSize();

	// scatter sendcounts:
	MPI::COMM_WORLD.Bcast    (&hsize,            1,    MPI_LONG,0);

	// broadcast number of individual spin prioperties:
	MPI::COMM_WORLD.Bcast    (pSam->GetHelper(),hsize,MPI_DOUBLE,0);

	int csize = pSam->GetNoSpinCompartments();
	MPI::COMM_WORLD.Bcast    (&csize , 1 ,MPI_INT,0);

	MPI::Datatype MPI_SPINDATA = MPIspindata();

	//scatter sendcounts:
	MPI::COMM_WORLD.Scatterv (pSam->GetSpinsData(),sendcount,displs,MPI_SPINDATA,&recvdummy,0,MPI_SPINDATA,0);
	// broadcast resolution:
	MPI::COMM_WORLD.Bcast    (pSam->GetResolution(),3,MPI_DOUBLE,0);

	// now listen for paket requests:
	int SlavesDone=0;
	while (SlavesDone < size - 1) {

		int     SlaveID;
		int     NoSpins;
		int     NextSpinToSend;
		//double* spindata = pSam->GetSpinsData();

		MPI::COMM_WORLD.Recv(&SlaveID,1,MPI_INT,MPI_ANY_SOURCE,REQUEST_SPINS);

		//get next spin paket to send:
		pSam->GetNextPacket(NoSpins,NextSpinToSend,SlaveID);

		if (NoSpins == 0)
			SlavesDone++;

		// receive signal
		for (unsigned int i=0; i < RxCA->GetSize(); i++)
			mpi_recv_paket_signal(RxCA->GetCoil(i)->GetSignal(),SlaveID,i);

		// dump temp signal
		pSam->DumpRestartInfo(RxCA);

		// now send NoSpins
		MPI::COMM_WORLD.Send(&NoSpins,1,MPI_INT,SlaveID,SEND_NO_SPINS);
		if (NoSpins > 0)
			MPI::COMM_WORLD.Send(&(pSam->GetSpinsData())[NextSpinToSend*pSam->GetNProps()],NoSpins,MPI_SPINDATA,SlaveID,SEND_SAMPLE);

#ifndef HAVE_MPI_THREADS

		// without threads: write progress bar each time new spins are sent:
		static int progress_percent = -1;
		static int spinsdone=0;
		spinsdone+=sendcount[SlaveID];	sendcount[SlaveID]=NoSpins;
		//update progress counter (pjemris without threads support)
		World* pW = World::instance();
		int progr = (100*(spinsdone+1)/pW->TotalSpinNumber);
		// case of restart: set progress to 100 at end:
		if (SlavesDone==(size-1)) progr=100;

		if (progr != progress_percent) {
			progress_percent = progr;
			ofstream fout(".jemris_progress.out" , ios::out);
			fout << progr;
			fout.close();
		}
#endif

	}  // end while (SlavesDone < size -1)

	flush (cout);

	delete[] sendcount;
	delete[] displs;

#ifdef HAVE_MPI_THREADS
	/* join threads: */
	int *status;                                /* holds return code */
	errcode = pthread_join(counter_thread,(void **) &status);
	if (errcode) {
       cout << "Joining of threads failed! (so what... ;) )"<<endl;
       // exit(-1);
    }
#endif

	return;

}

/*
 * Receive first portion of sample from MPI master process.
 * @return pointer to the new (sub)sample.
 *         Deletion of the (sub)sample has to be done elsewehere!
 *         (This is e.g. done by the Simulator class)
 */
Sample* mpi_receive_sample(int sender, int tag){

	long NPoints;

	// get number of spins:
	int nospins;
	MPI::COMM_WORLD.Scatter(NULL,1,MPI_INT,&nospins,1,MPI_INT,0);
	Sample* pSam = new Sample();

	NPoints= (long) nospins;
	//get number of physical properties per spin
	MPI::COMM_WORLD.Bcast    (pSam->GetSampleDims(),1,MPI_LONG,0);
	
	pSam->CreateSpins(NPoints);

	long hsize = 0;
	MPI::COMM_WORLD.Bcast (&hsize, 1, MPI_LONG,0);

	pSam->CreateHelper(hsize);
	MPI::COMM_WORLD.Bcast  (pSam->GetHelper(),(int)hsize,MPI_DOUBLE,0);
	
	int csize = 0;
	MPI::COMM_WORLD.Bcast (&csize, 1, MPI_INT,0);

	pSam->SetNoSpinCompartments(csize);
	World::instance()->SetNoOfSpinProps(pSam->GetNProps());
	World::instance()->SetNoOfCompartments(csize);

	MPI::Datatype MPI_SPINDATA = MPIspindata();
	// get sample:
	MPI::COMM_WORLD.Scatterv (NULL,NULL,NULL,MPI_SPINDATA,pSam->GetSpinsData(),nospins,MPI_SPINDATA,0);
	//get resolution (needed for position randomness)
	MPI::COMM_WORLD.Bcast    (pSam->GetResolution(),3,MPI_DOUBLE,0);

	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);
	//cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	//     << NPoints << " spins for simulation ..." << endl << flush;

	return pSam;

}

/*****************************************************************************/
/**
 *  receive next small package of spins (first paket by mpi_receive_sample)
 * 	if no spins are left -> return false else true
 */
bool mpi_recieve_sample_paket(Sample *samp, CoilArray* RxCA ){

	World* pW 	= World::instance();
	int myRank 	= pW->m_myRank;
	int NoSpins = 0;

	// request new spins:
	MPI::COMM_WORLD.Send(&myRank,1,MPI_INT,0,REQUEST_SPINS);

	// send signal
	for (unsigned int i=0; i < RxCA->GetSize(); i++)
		mpi_send_paket_signal(RxCA->GetCoil(i)->GetSignal(),i);
	
	// Receive number of spins
	MPI::COMM_WORLD.Recv(&NoSpins,1,MPI_INT,0,SEND_NO_SPINS);
	
	// Spins left?
	if (NoSpins == 0) return false;
	
	// prepare sample structure
	samp->ClearSpins();
	samp->CreateSpins(NoSpins);
	
	// Receive
	MPI::Datatype MPI_SPINDATA = MPIspindata();
	MPI::COMM_WORLD.Recv(samp->GetSpinsData(),NoSpins,MPI_SPINDATA,0,SEND_SAMPLE);
	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);
	//cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	//     << NoSpins << " spins for simulation ..." << endl << flush;

	return true;

}
/*****************************************************************************/
void mpi_send_paket_signal (Signal* pSig, int CoilID) {
	
	int     tsize = (int) pSig->Repo()->Size();
	
	if (World::instance()->m_myRank == 1)
		MPI::COMM_WORLD.Send(pSig->Repo()->Times(), (int) pSig->Repo()->Samples(), MPI_DOUBLE, 0, SIG_TP + (CoilID)*4);

	MPI::COMM_WORLD.Send(pSig->Repo()->Data(), tsize, MPI_DOUBLE, 0, SIG_MX + (CoilID)*4);

	
}

/*****************************************************************************/
void mpi_recv_paket_signal(Signal *pSig, int SlaveId, int CoilID) {

	int     tsize = (int) pSig->Repo()->Size();
	double* data  = pSig->Repo()->Data(); 
	double* tmp   = (double*) malloc (tsize*sizeof(double));
	
	if (SlaveId == 1)
		MPI::COMM_WORLD.Recv(pSig->Repo()->Times(), (int) pSig->Repo()->Samples(), MPI_DOUBLE, SlaveId, SIG_TP + (CoilID)*4);
	
	MPI::COMM_WORLD.Recv(tmp, tsize, MPI_DOUBLE, SlaveId, SIG_MX + (CoilID)*4);

	for (int i = 0; i < tsize; i++) 
		data[i] += tmp[i];
	
	free (tmp);

}

#endif
