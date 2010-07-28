/** @file mpi_Model.h
 *  @brief Implementation of JEMRIS mpi structures/functions for send/receive of sample/signal and of JEMRIS mpiEvolution
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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


#ifndef _MPI_MODEL_H_
#define _MPI_MODEL_H_

#include<iostream>
#include "config.h"

using namespace std;
#define MPICH_IGNORE_CXX_SEEK

#include <mpi.h>

#include "Sample.h"
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

	Spin_data data;

	const int NUM_DATA = NO_SPIN_PROPERTIES;

    MPI_Datatype MPI_SPINDATA ;
    MPI_Datatype type[NUM_DATA];// = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    for (int i=0;i<NUM_DATA; i++) type[i] =MPI_DOUBLE;
    int          blocklen[NUM_DATA];// = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (int i=0;i<NUM_DATA; i++) blocklen[i] = 1;
    MPI_Aint     disp[NUM_DATA];
    MPI_Aint          base;

    MPI_Address(&(data.x) , disp);
    MPI_Address(&(data.y) , disp+1);
    MPI_Address(&(data.z) , disp+2);
    MPI_Address(&(data.m0), disp+3);
    MPI_Address(&(data.r1), disp+4);
    MPI_Address(&(data.r2), disp+5);
    MPI_Address(&(data.r2s),disp+6);
    MPI_Address(&(data.db), disp+7);
    MPI_Address(&(data.nn), disp+8);
    MPI_Address(&(data.index), disp+9);

    base = disp[0];


    for (int i=0; i <NUM_DATA; i++) disp[i] -= base;

    MPI_Type_struct( NUM_DATA, blocklen, disp, type, &MPI_SPINDATA);
    MPI_Type_commit( &MPI_SPINDATA);

    return  MPI_SPINDATA;

};

/*****************************************************************************/
/* function that does the progress counting; called by an extra thread from the master process */
#ifdef HAVE_MPI_THREADS
void *CountProgress(void * arg) {
	int TotalNoSpins  = *((int *) arg);
	int SpinsDone = TotalNoSpins - *(((int *) arg )+1);
	int NowDone=0;
	int progress_percent = -1;

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
		}
	}
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

    if (errcode=pthread_create(&counter_thread,	/* thread struct             */
                    NULL,                      	/* default thread attributes */
                    CountProgress,              /* start routine             */
                    (void *) &buffer)) {       /* arg to routine            */
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

	MPI::Datatype MPI_SPINDATA = MPIspindata();
	int *sendcount; 		// array with NumberOfSpins[slaveid]
	int *displs;
	int count,rest;
	int recvbuf;
	Spin_data recvdummy;				// dummy buffer

	// create sendcounts, displs
	sendcount 	= new int[size];
	displs		= new int[size];

	// no spin data for master:
	displs[0]	= 0;
	sendcount[0]= 0;

	World* pW = World::instance();

	pSam->GetScatterVectors(sendcount,displs,size);

	//scatter sendcounts:
	MPI::COMM_WORLD.Scatter(sendcount,1,MPI_INT,&recvbuf,1,MPI_INT,0);
	//scatterv spins:
	MPI::COMM_WORLD.Scatterv(pSam->GetSpinsData(),sendcount,displs,MPI_SPINDATA,&recvdummy,recvbuf,MPI_SPINDATA,0);
	// broadcast resolution:
	MPI::COMM_WORLD.Bcast(pSam->GetResolution(),3,MPI_DOUBLE,0);

	delete[] sendcount;
	delete[] displs;

	// now listen for paket requests:
	int SlavesDone=0;
	while (SlavesDone < size -1)
	{
		int SlaveID;
		int SpinsLeft;
		int NoSpins;
		int NextSpinToSend;
		Spin_data *spindata = pSam->GetSpinsData();

		MPI::COMM_WORLD.Recv(&SlaveID,1,MPI_INT,MPI_ANY_SOURCE,REQUEST_SPINS);

		//get next spin paket to send:
		pSam->GetNextPacket(NoSpins,NextSpinToSend,SlaveID);

		if (NoSpins == 0)
			SlavesDone++;

		// receive signal
		for (int i=0; i < RxCA->GetSize(); i++)
			mpi_recv_paket_signal(RxCA->GetCoil(i)->GetSignal(),SlaveID,i);

		// dump temp signal
		pSam->DumpRestartInfo(RxCA);

		// now send NoSpins
		MPI::COMM_WORLD.Send(&NoSpins,1,MPI_INT,SlaveID,SEND_NO_SPINS);
		if (NoSpins > 0)
			MPI::COMM_WORLD.Send(&(spindata[NextSpinToSend]),NoSpins,MPI_SPINDATA,SlaveID,SEND_SAMPLE);
#ifndef HAVE_MPI_THREADS
		// without threads: write progress bar each time new spins are sent:
		static int progress_percent = -1;
		//update progress counter (pjemris without threads support)
		int progr = (100*(NextSpinToSend+1)/pW->TotalSpinNumber);

		if (progr != progress_percent) {
			progress_percent = progr;
			ofstream fout(".jemris_progress.out" , ios::out);
			fout << progr;
			fout.close();
		}
#endif
	}  // end while (SlavesDone < size -1)

#ifdef HAVE_MPI_THREADS
	/* join threads: */
	int *status;                                /* holds return code */
	if (errcode=pthread_join(counter_thread,(void **) &status)) {
       cout << "Joining of threads failed! (so what... ;) )"<<endl;
       // exit(-1);
    }
#endif

	return;

};

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
	NPoints= (long) nospins;
	Sample* pSam = new Sample (NPoints);

	MPI::Datatype MPI_SPINDATA = MPIspindata();
	// get sample:
	MPI::COMM_WORLD.Scatterv(NULL,NULL,NULL,MPI_SPINDATA,pSam->GetSpinsData(),nospins,MPI_SPINDATA,0);
	//get resolution (needed for position randomness)
	MPI::COMM_WORLD.Bcast(pSam->GetResolution(),3,MPI_DOUBLE,0);

	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);
	cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	     << NPoints << " spins for simulation ..." << endl << flush;

	return pSam;

};

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
	for (int i=0; i < RxCA->GetSize(); i++)
		mpi_send_paket_signal(RxCA->GetCoil(i)->GetSignal(),i);

	// clear signal
	//done in pjemris

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
	cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	     << NoSpins << " spins for simulation ..." << endl << flush;


	return true;
};
/*****************************************************************************/
void mpi_send_paket_signal(Signal* pSig,int CoilID) {
	long lSigDim = pSig->GetSize();
	World *pw = World::instance();
	if (pw->m_myRank == 1)
		MPI::COMM_WORLD.Send(pSig->repository.tp, (int) lSigDim, MPI_DOUBLE, 0, SIG_TP + (CoilID)*4);
	MPI::COMM_WORLD.Send(pSig->repository.mx, (int) lSigDim, MPI_DOUBLE, 0, SIG_MX + (CoilID)*4);
	MPI::COMM_WORLD.Send(pSig->repository.my, (int) lSigDim, MPI_DOUBLE, 0, SIG_MY + (CoilID)*4);
	MPI::COMM_WORLD.Send(pSig->repository.mz, (int) lSigDim, MPI_DOUBLE, 0, SIG_MZ + (CoilID)*4);

}

/*****************************************************************************/
void mpi_recv_paket_signal(Signal *pSig,int SlaveId,int CoilID) {
	long lSigDim = pSig->GetSize();

	double* tmp;
	tmp = new double[lSigDim];

	if (SlaveId == 1)
		MPI::COMM_WORLD.Recv(pSig->repository.tp, (int) lSigDim, MPI_DOUBLE, SlaveId, SIG_TP + (CoilID)*4);
	MPI::COMM_WORLD.Recv(tmp, (int) lSigDim, MPI_DOUBLE, SlaveId, SIG_MX + (CoilID)*4);
	for (int i=0; i<lSigDim;i++) pSig->repository.mx[i]+=tmp[i];
	MPI::COMM_WORLD.Recv(tmp, (int) lSigDim, MPI_DOUBLE, SlaveId, SIG_MY + (CoilID)*4);
	for (int i=0; i<lSigDim;i++) pSig->repository.my[i]+=tmp[i];
	MPI::COMM_WORLD.Recv(tmp, (int) lSigDim, MPI_DOUBLE, SlaveId, SIG_MZ + (CoilID)*4);
	for (int i=0; i<lSigDim;i++) pSig->repository.mz[i]+=tmp[i];

	delete[] tmp;

}

#endif
