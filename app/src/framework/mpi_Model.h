/** @file mpi_Model.h
 *  @brief Implementation of JEMRIS mpi structures/functions for send/receive of sample/signal and of JEMRIS mpiEvolution
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


#ifndef _MPI_MODEL_H_
#define _MPI_MODEL_H_

#include<iostream>

using namespace std;
#define MPICH_IGNORE_CXX_SEEK

#ifdef i386Linux
    #include <mpi++.h>
#endif
#ifdef i386Darwin
    #include <mpi.h>
#endif
#ifdef x86_64Linux
    #include <mpi++.h>
#endif
#ifdef ppc64Linux
    #include <mpi.h>
#endif
#ifdef i386MINGW32_NT-5.1
    #include <mpi.h>
#endif

#include "Sample.h"
#include "Signal.h"
#include "Declarations.h"
#include "Sample.h"

/*****************************************************************************/
MPI_Datatype  MPIspindata() {

	Spin_data data;

	const int NUM_DATA = 8;

    MPI_Datatype MPI_SPINDATA ;
    MPI_Datatype type[NUM_DATA] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    int          blocklen[NUM_DATA] = {1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Aint     disp[NUM_DATA];
    MPI_Aint          base;

    MPI_Address(&(data.x) , disp);
    MPI_Address(&(data.y) , disp+1);
    MPI_Address(&(data.z) , disp+2);
    MPI_Address(&(data.m0), disp+3);
    MPI_Address(&(data.r1), disp+4);
    MPI_Address(&(data.r2), disp+5);
    MPI_Address(&(data.db), disp+6);
    MPI_Address(&(data.nn), disp+7);

    base = disp[0];


    for (int i=0; i <NUM_DATA; i++) disp[i] -= base;

    MPI_Type_struct( NUM_DATA, blocklen, disp, type, &MPI_SPINDATA);
    MPI_Type_commit( &MPI_SPINDATA);

    return  MPI_SPINDATA;

};

/*****************************************************************************/
void mpi_devide_and_send_sample (Sample* pSam, int tag) {

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

	World* pW = World::instance();

	if (pW->m_useLoadBalancing) {
		// begin new version
		// min/max number of spins to each cpu:
		int maxPaketSize  = 1999; // if packetsize >= 128kb (==2000 spins) -> mpich binaries for ubuntu crashes on (some?) 64bit machines...
		int minPaketSize  = 10;

		int TotalNoSpins 	= pSam->GetSize();
		// id of next spin to send
		int NextSpinToSend 	= 0;

		//send half of sample to cpus the old way:
		count = floor((float) TotalNoSpins/2/(size-1));
		if (count > maxPaketSize) count = maxPaketSize;
	//	if (count < minPaketSize) count = minPaketSize;
		NextSpinToSend = count*(size-1);


		// create sendcounts, displs
		sendcount 	= new int[size];
		displs		= new int[size];

		// no spin data for master:
		displs[0]	= 0;
		sendcount[0]= 0;

		for (int i=1;i<size;i++) {
			sendcount[i] 	= count;
			displs[i]=displs[i-1]+sendcount[i-1];
		}

		//scatter sendcounts:
		int recvbuf;
		MPI::COMM_WORLD.Scatter(sendcount,1,MPI_INT,&recvbuf,1,MPI_INT,0);

		//scatterv spins:
		Spin_data recvdummy;				// dummy buffer
		MPI::COMM_WORLD.Scatterv(pSam->GetSpinsData(),sendcount,displs,MPI_SPINDATA,&recvdummy,recvbuf,MPI_SPINDATA,0);

		// broadcast resolution:
		MPI::COMM_WORLD.Bcast(pSam->GetResolution(),3,MPI_DOUBLE,0);

		// now load balancing:
		int SlavesDone=0;
		Spin_data *spindata = pSam->GetSpinsData();
		int progress_percent = -1;
		{ofstream fout(".jemris_progress.out" , ios::out);
		fout << 1;
		fout.close();}

		while (SlavesDone < size -1)
		{
			int SlaveID;
			int SpinsLeft;
			int NoSpins;
			MPI::COMM_WORLD.Recv(&SlaveID,1,MPI_INT,MPI_ANY_SOURCE,REQUEST_SPINS);
			// spins left to send?
			if (TotalNoSpins != NextSpinToSend)
			{
				SpinsLeft = TotalNoSpins - NextSpinToSend;
				// make spin packets smaller as the number of spins decreases:
				NoSpins = floor ((float) SpinsLeft/2/(size-1) );
				if (NoSpins > maxPaketSize) NoSpins = maxPaketSize;
				if (NoSpins < minPaketSize)
				{
					NoSpins = minPaketSize;
					if (NoSpins > SpinsLeft) NoSpins = SpinsLeft;
				}
			} else {
				NoSpins = 0;
				SlavesDone ++;
			}

			// now send NoSpins
			MPI::COMM_WORLD.Send(&NoSpins,1,MPI_INT,SlaveID,SEND_NO_SPINS);
			if (NoSpins > 0)
			{
				MPI::COMM_WORLD.Send(&(spindata[NextSpinToSend]),NoSpins,MPI_SPINDATA,SlaveID,SEND_SAMPLE);
				NextSpinToSend += NoSpins;
			}

			// dump progress; not very accurate...
			int progr = (99 -100*(TotalNoSpins - NoSpins - NextSpinToSend)/TotalNoSpins);

			if (progr != progress_percent) {
				progress_percent = progr;
				ofstream fout(".jemris_progress.out" , ios::out);
				fout << progr;
				fout.close();
			}
		}  // end while (SlavesDone < size -1)
		{ofstream fout(".jemris_progress.out" , ios::out);
						fout << 100;
						fout.close();
		}
		//end load balancing
	} else {
		// begin no load balancing
		// spins per process:
		count = (int) (( (double) pSam->GetSize() ) / ((double) (size - 1) ) + 0.01);
		rest  = (int) (fmod((double) pSam->GetSize() , (double) (size - 1) ) + 0.01);


		// create sendcounts, displs
		sendcount 	= new int[size];
		displs		= new int[size];

		// no spin data for master:
		displs[0]	= 0;
		sendcount[0]= 0;

		for (int i=1;i<size;i++) {
			sendcount[i] 	= count;
			if (rest > 0) {
				sendcount[i]++;
				rest--;
			}
			displs[i]=displs[i-1]+sendcount[i-1];
		}

		//scatter sendcounts:
		int recvbuf;
		MPI::COMM_WORLD.Scatter(sendcount,1,MPI_INT,&recvbuf,1,MPI_INT,0);

		//scatterv spins:
		Spin_data recvdummy;				// dummy buffer
		MPI::COMM_WORLD.Scatterv(pSam->GetSpinsData(),sendcount,displs,MPI_SPINDATA,&recvdummy,recvbuf,MPI_SPINDATA,0);

		// broadcast resolution:
		MPI::COMM_WORLD.Bcast(pSam->GetResolution(),3,MPI_DOUBLE,0);
	} // if useLoadBalancing

	delete[] sendcount;
	delete[] displs;

	return;

};

/*
 * Receive a(sub)sample from MPI master process.
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
 *  For load balancing: receive a small package of spins
 * 	if no spins are left -> return false else true
 */
bool mpi_recieve_sample_paket(Sample *samp){
	World* pW 	= World::instance();
	int myRank 	= pW->m_myRank;
	int NoSpins = 0;

	// request new spins:
	MPI::COMM_WORLD.Send(&myRank,1,MPI_INT,0,REQUEST_SPINS);

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
void mpi_send_signal(Signal* pSig, int receiver, int tag){

	long lSigDim = pSig->GetSize();

	// send timepoints only once (first slave); use non-blocking send:
	MPI::Request my_request;
	if (MPI::COMM_WORLD.Get_rank() == 1) {
		my_request = MPI::COMM_WORLD.Isend(pSig->repository.tp, (int) lSigDim, MPI_DOUBLE, 0, 23 );
	}

	MPI::COMM_WORLD.Reduce(pSig->repository.mx,NULL,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);
	MPI::COMM_WORLD.Reduce(pSig->repository.my,NULL,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);
	MPI::COMM_WORLD.Reduce(pSig->repository.mz,NULL,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);

	if (MPI::COMM_WORLD.Get_rank() == 1) {
		my_request.Wait();
	}

	return;

};

/*****************************************************************************/
void mpi_receive_and_sum_signal(int tag, Signal* pSumSignal){

	int size     = MPI::COMM_WORLD.Get_size();
	long lSigDim = pSumSignal->GetSize();

	// create buffer with zeros; used for reduce routines (no data from master!)
	double *buffer;
	buffer = new double[lSigDim];
	for (int i=0;i<lSigDim;i++) buffer[i]=0;

	// init respository:
	pSumSignal->repository.size  = lSigDim;
	for (int i=0; i<lSigDim; i++) {
		pSumSignal->repository.mx[i] = 0;
		pSumSignal->repository.my[i] = 0;
		pSumSignal->repository.mz[i] = 0;

	}

	MPI::Request my_request;
	// Get timepoints from first slave:
	my_request = MPI::COMM_WORLD.Irecv(pSumSignal->repository.tp, (int) lSigDim, MPI_DOUBLE, 1, 23 );

	MPI::COMM_WORLD.Reduce(buffer,pSumSignal->repository.mx,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);
	MPI::COMM_WORLD.Reduce(buffer,pSumSignal->repository.my,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);
	MPI::COMM_WORLD.Reduce(buffer,pSumSignal->repository.mz,(int) lSigDim,MPI_DOUBLE,MPI_SUM,0);

	my_request.Wait();
	delete[] buffer;


};

/*****************************************************************************/
class mpiEvolution {

 public:


     /**
      * @brief Constructor
      */
     mpiEvolution () { pW = World::instance(); };

     /**
      * @brief Default destructor
      */
    ~mpiEvolution() { };

    /**
     * @brief Slave: redefine function saveEvolution for the call in Model::RunTree
     */
    inline void send() { pW->saveEvolFunPtr = &mpiEvolution::saveEvolution; };

    /**
     * @brief Master: loop over all evol-time-points and spins.
     * Receive the solution, then call World::SaveEvolution.
     */
    void Receive() {

        if ( (pW->saveEvolFileName).empty() || pW->saveEvolStepSize==0 ) return;

        long N     = pW->TotalSpinNumber;
        long M     = pW->TotalADCNumber / pW->saveEvolStepSize ;
        int size   = MPI::COMM_WORLD.Get_size();
        long l     = N%(size-1);

        double dVal[8];

        pW->Values = &dVal[0];

        for (long n=0; n < N*M ; ++n) {

            int sender;
            long ADCindex, nspin;

            MPI::COMM_WORLD.Recv(&(sender)             , 1, MPI_INT   , MPI_ANY_SOURCE, 9);
            MPI::COMM_WORLD.Recv(&(pW->time)           , 1, MPI_DOUBLE, sender, 10);
            MPI::COMM_WORLD.Recv(&(ADCindex)           , 1, MPI_LONG  , sender, 11);
            MPI::COMM_WORLD.Recv(&(nspin)              , 1, MPI_LONG  , sender, 12);

            long k         = N/(size-1) +     (sender>l?0:1);
            long ibeg      = (sender-1) * k + (sender>l?l:0);
            pW->SpinNumber = ibeg + nspin;

            MPI::COMM_WORLD.Recv(&(dVal[XC])           , 1, MPI_DOUBLE, sender, 13);
            MPI::COMM_WORLD.Recv(&(dVal[YC])           , 1, MPI_DOUBLE, sender, 14);
            MPI::COMM_WORLD.Recv(&(dVal[ZC])           , 1, MPI_DOUBLE, sender, 15);
            MPI::COMM_WORLD.Recv(&(pW->solution[AMPL]) , 1, MPI_DOUBLE, sender, 16);
            MPI::COMM_WORLD.Recv(&(pW->solution[PHASE]), 1, MPI_DOUBLE, sender, 17);
            MPI::COMM_WORLD.Recv(&(pW->solution[ZC])   , 1, MPI_DOUBLE, sender, 18);

            pW->saveEvolFunPtr( ADCindex, n+1 == N*M );

        }

        return;

    };


    /**
     * @brief Save evolution
     *
     * Executed by the slaves during Model::RunTree()
     * Sends current time-point of magnetisation evolution
     * to the Master.
     */
    static void saveEvolution (long ADCindex, bool dummy ) {

        World* pW = World::instance();
        if ( (pW->saveEvolFileName).empty() || pW->saveEvolStepSize==0 ) return;

        int rank = MPI::COMM_WORLD.Get_rank();
        long n=pW->SpinNumber;
        MPI::COMM_WORLD.Send(&(rank)               , 1, MPI_INT   , 0,  9);
        MPI::COMM_WORLD.Send(&(pW->time)           , 1, MPI_DOUBLE, 0, 10);
        MPI::COMM_WORLD.Send(&(ADCindex)           , 1, MPI_LONG  , 0, 11);
        MPI::COMM_WORLD.Send(&(n)                  , 1, MPI_LONG  , 0, 12);
        MPI::COMM_WORLD.Send(&(pW->Values[XC])     , 1, MPI_DOUBLE, 0, 13);
        MPI::COMM_WORLD.Send(&(pW->Values[YC])     , 1, MPI_DOUBLE, 0, 14);
        MPI::COMM_WORLD.Send(&(pW->Values[ZC])     , 1, MPI_DOUBLE, 0, 15);
        MPI::COMM_WORLD.Send(&(pW->solution[AMPL]) , 1, MPI_DOUBLE, 0, 16);
        MPI::COMM_WORLD.Send(&(pW->solution[PHASE]), 1, MPI_DOUBLE, 0, 17);
        MPI::COMM_WORLD.Send(&(pW->solution[ZC])   , 1, MPI_DOUBLE, 0, 18);

       return;

     };

    World* pW;

};

#endif
