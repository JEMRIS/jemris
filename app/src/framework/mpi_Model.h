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
MPI_Datatype  MPIspin(long NPoints, struct Spin &spin) {

    MPI_Datatype MPI_SPIN ;
    MPI_Datatype type[9] = {MPI_LONG, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    int          blocklen[9] = {1, NPoints, NPoints, NPoints, NPoints, NPoints, NPoints, NPoints, NPoints};
    MPI_Aint     disp[9];
    int          base;

    MPI_Address(&(spin.size), disp);
    MPI_Address(spin.x , disp+1);
    MPI_Address(spin.y , disp+2);
    MPI_Address(spin.z , disp+3);
    MPI_Address(spin.m0, disp+4);
    MPI_Address(spin.r1, disp+5);
    MPI_Address(spin.r2, disp+6);
    MPI_Address(spin.db, disp+7);
    MPI_Address(spin.nn, disp+8);

    base = disp[0];

    for (int i=0; i <9; i++) disp[i] -= base;

    MPI_Type_struct( 9, blocklen, disp, type, &MPI_SPIN);
    MPI_Type_commit( &MPI_SPIN);

    return  MPI_SPIN;

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

	// Rank of master: 0. So start with 1.
	for (int j=1 ; j<size ; j++) {

		Sample* subSample = pSam->GetSubSample(j,size-1);
		long NPoints = subSample->GetSize();

        	MPI::COMM_WORLD.Send( &NPoints, 1, MPI_LONG, j, tag);
		MPI::Datatype MPI_SPIN = MPIspin(NPoints,subSample->spins);
		MPI::COMM_WORLD.Send(&(subSample->spins), 1, MPI_SPIN, j, tag );
        	MPI::COMM_WORLD.Send(subSample->GetResolution(), 3, MPI_DOUBLE, j, tag);

		delete subSample;

	}

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
	MPI::COMM_WORLD.Recv(&NPoints, 1, MPI_LONG, sender, tag);
	Sample* pSam = new Sample (NPoints);

	MPI::Datatype MPI_SPIN = MPIspin( NPoints,pSam->spins );
	MPI::COMM_WORLD.Recv(&(pSam->spins)       , 1, MPI_SPIN  , sender, tag);
        MPI::COMM_WORLD.Recv(pSam->GetResolution(), 3, MPI_DOUBLE, sender, tag);

	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);
	cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	     << NPoints << " spins for simulation ..." << endl << flush;

	return pSam;

};

/*****************************************************************************/
MPI_Datatype  MPIsignal (long NPoints, struct Repository &srep) {

    MPI_Datatype MPI_SIGNAL;
    MPI_Datatype type[5]     = {MPI_LONG, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    int          blocklen[5] = {1, NPoints, NPoints, NPoints, NPoints};
    MPI_Aint     disp[5];
    int          base;

    MPI_Address( &(srep.size), disp);
    MPI_Address( srep.tp, disp+1);
    MPI_Address( srep.mx, disp+2);
    MPI_Address( srep.my, disp+3);
    MPI_Address( srep.mz, disp+4);

    base = disp[0];
    for (int i=0; i <5; i++)
        disp[i] -= base;

    MPI_Type_struct( 5, blocklen, disp, type, &MPI_SIGNAL);
    MPI_Type_commit( &MPI_SIGNAL);

    return  MPI_SIGNAL;

};

/*****************************************************************************/
void mpi_send_signal(Signal* pSig, int receiver, int tag){

	long lSigDim = pSig->GetSize();
	MPI::Datatype MPI_SIGNAL = MPIsignal(lSigDim,pSig->repository);
	MPI::COMM_WORLD.Ssend(&(pSig->repository), 1, MPI_SIGNAL, receiver, tag );

	return;

};

/*****************************************************************************/
void mpi_receive_and_sum_signal(int tag, Signal* pSumSignal){

	int size     = MPI::COMM_WORLD.Get_size();
	long lSigDim = pSumSignal->GetSize();

	Signal* pSubSignal = new Signal(lSigDim);
	MPI::Datatype MPI_SIGNAL = MPIsignal(lSigDim, pSubSignal->repository);

	for (int j=0;j<size;j++) {

		if (j != MPI::COMM_WORLD.Get_rank()) {

			MPI::COMM_WORLD.Recv(&(pSubSignal->repository), 1, MPI_SIGNAL, j, tag);

			for (int i=0; i<lSigDim; i++) {

				if (j==1) pSumSignal->repository.tp[i] = pSubSignal->repository.tp[i];
				pSumSignal->repository.mx[i] += pSubSignal->repository.mx[i]/(size-1);
				pSumSignal->repository.my[i] += pSubSignal->repository.my[i]/(size-1);
				pSumSignal->repository.mz[i] += pSubSignal->repository.mz[i]/(size-1);

			}

		}

 	}

	delete pSubSignal;

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
