/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _MPI_MRSIM_H_
#define _MPI_MRSIM_H_

#include<iostream>
using namespace std;

#ifdef i386
#include <mpi++.h>
#endif
#ifdef x86_64
#include <mpi++.h>
#endif
#ifdef ppc64
#define MPICH_IGNORE_CXX_SEEK
#include <mpi.h>
#endif

#include "Model_MPI_Datatypes.h"	//definitions of MPI datatypes for signal and sample
#include "Sample.h"
#include "Signal.h"

void mpi_devide_and_send_sample(Sample* pSam, int tag){

	int size = MPI::COMM_WORLD.Get_size();
	Sample * subSample;

	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);

	cout	<< endl;
	cout << hostname << " -> Master Process: send MR sample (" << pSam->sSample.NumberOfPoints
		<< " spins) to " << size-1 << " slave(s)"<< endl << endl;

	for (int j=0 ; j<size ; j++)
	{
		if (j != MPI::COMM_WORLD.Get_rank())
		{
			subSample = pSam->getSubSample(j,size-1);
			MPI::Datatype Sampletype = MPISample(subSample->sSample.NumberOfPoints,subSample->sSample);
			MPI::COMM_WORLD.Send( &(subSample->sSample.NumberOfPoints), 1, MPI::INT, j, tag);
			MPI::COMM_WORLD.Send(&(subSample->sSample), 1, Sampletype, j, tag );
		}
	}
	return;
};

Sample* mpi_receive_sample(int sender, int tag){

	int iSamplePoints;
	MPI::COMM_WORLD.Recv(&iSamplePoints, 1, MPI::INT, sender, tag);
	Sample* pSam = new Sample (iSamplePoints);
	MPI::Datatype Sampletype = MPISample(iSamplePoints,pSam->sSample);
	MPI::COMM_WORLD.Recv(&(pSam->sSample), 1, Sampletype, sender, tag);

	int ilen; char hm[MPI_MAX_PROCESSOR_NAME]; char* hostname=&hm[0];
	MPI::Get_processor_name(hostname,ilen);

	cout << hostname << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received " 
		<< iSamplePoints << " spins for simulation ..." << endl;

	return pSam;
};

void mpi_send_signal(Signal* pSig, int receiver, bool send_signal_dim, int tag){
	long lSigDim = pSig->sSignal.NumberOfPoints;
	MPI::Datatype Signaltype = MPISignal(lSigDim,pSig->sSignal);
	if (send_signal_dim)
		MPI::COMM_WORLD.Send(&lSigDim, 1, MPI::LONG, receiver, tag );
	MPI::COMM_WORLD.Send(&(pSig->sSignal), 1, Signaltype, receiver, tag );
	return;
};

Signal* mpi_receive_and_sum_signal(int dimsender, int tag){

	int size    = MPI::COMM_WORLD.Get_size();
	long lSigDim; 

	MPI::COMM_WORLD.Recv(&lSigDim, 1, MPI::LONG, dimsender, tag);
	Signal* pSubSignal = new Signal(lSigDim);
	Signal* pSumSignal = new Signal(lSigDim);
	MPI::Datatype Signaltype = MPISignal(lSigDim, pSubSignal->sSignal);

	for (int j=0;j<size;j++)
	{
		if (j != MPI::COMM_WORLD.Get_rank())
		{
			MPI::COMM_WORLD.Recv(&(pSubSignal->sSignal), 1, Signaltype, j, tag);
			for (int i=0; i<lSigDim; i++)
			{
				if (j==1) pSumSignal->sSignal.t[i] = pSubSignal->sSignal.t[i];
				pSumSignal->sSignal.Mx[i] += pSubSignal->sSignal.Mx[i]/(size-1);
				pSumSignal->sSignal.My[i] += pSubSignal->sSignal.My[i]/(size-1);
				pSumSignal->sSignal.Mz[i] += pSubSignal->sSignal.Mz[i]/(size-1);
			}
		}
 	}

	delete pSubSignal;
	return pSumSignal;
};

#endif
