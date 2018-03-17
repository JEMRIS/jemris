/** @file mpi_Model.h
 *  @brief Implementation of JEMRIS mpi structures/functions for send/receive of sample/signal and of JEMRIS mpiEvolution
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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
#include "MultiPoolSample.h"

#ifdef HAVE_MPI_THREADS
	#include <pthread.h>
#endif

void mpi_send_paket_signal(Signal* pSig,int CoilID);
void mpi_recv_paket_signal(Signal* pSig,int SlaveID,int CoilID);

/*****************************************************************************/
inline MPI_Datatype MPIspindata () {

	int NUM_DATA = World::instance()->GetNoOfSpinProps();

    MPI_Datatype MPI_SPINDATA ;
	MPI_Datatype type = MPI_DOUBLE;
	MPI_Aint     disp = 0;

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

	MPI_Status status;

	while (SpinsDone != TotalNoSpins) {
	  MPI_Recv(&NowDone,1,MPI_INT,MPI_ANY_SOURCE,SPINS_PROGRESS,MPI_COMM_WORLD,&status);
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

    int size, ilen;
    std::string hm (MPI_MAX_PROCESSOR_NAME,' ');

    MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_processor_name(&hm[0], &ilen);

	cout << endl << hm.c_str() << " -> Master Process: send MR sample (" << pSam->GetSize()
		 << " spins) to " << size-1 << " slave(s)"<< endl << endl << flush;

	std::vector<int> sendcount (size); 		    // array with NumberOfSpins[slaveid]
	std::vector<int> displs (size);
	int      recvbuf;
	double*  recvdummy = 0;	// dummy buffer

	// no spin data for master:
	displs[0]	 = 0;
	sendcount[0] = 0;
	
	pSam->GetScatterVectors(sendcount.data(), displs.data(), size);

	// scatter sendcounts:
	MPI_Scatter  (sendcount.data(), 1, MPI_INT, &recvbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);


        //MODIF
        /*int ierr,my_id,num_procs;
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        printf("I'm process %i out of %i processes\n", my_id, num_procs);*/

        int ii;
        long TotalSpinNumber=pSam->GetSize(),nextSpinMem=-1;
        for(ii=0;ii<size;ii++)
        {
            //cout<<ii<<" Sencount "<<sendcount[ii]<<" Displs "<<displs[ii]<<endl;
            if(ii>0)    {
                MPI_Send(&TotalSpinNumber,1,MPI_LONG,ii,SEND_TOTAL_NO_SPINS,MPI_COMM_WORLD);
                MPI_Send(&displs[ii],1,MPI_LONG,ii,SEND_BEGIN_SPIN,MPI_COMM_WORLD);
                //cout<<0<<" Sent spins index information to "<<ii<<" :  "<<TotalSpinNumber<<"  "<<displs[ii]<<endl;
                }
        }
        //MODIF***


	// broadcast number of individual spin prioperties:
	long NProps = pSam->GetNProps();
	MPI_Bcast    (&NProps, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	long hsize = pSam->GetHelperSize();
	MPI_Bcast    (&hsize, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast    (pSam->GetHelper(), hsize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	long dsize = pSam->GetSampleDimsSize();
	MPI_Bcast    (&dsize, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast    (&(pSam->GetSampleDims()[0]), dsize, MPI_INT, 0, MPI_COMM_WORLD);

	int csize = pSam->GetNoSpinCompartments();
	MPI_Bcast    (&csize , 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Datatype MPI_SPINDATA = MPIspindata();

	//scatter sendcounts:
	MPI_Scatterv (pSam->GetSpinsData(), sendcount.data(), displs.data(),
			MPI_SPINDATA, &recvdummy, 0, MPI_SPINDATA, 0, MPI_COMM_WORLD);
	// broadcast resolution:
	MPI_Bcast    (pSam->GetResolution(),3,MPI_DOUBLE,0, MPI_COMM_WORLD);

	// now listen for paket requests:
	int SlavesDone=0;
	while (SlavesDone < size - 1) {

		int     SlaveID;
		int     NoSpins;
		int     NextSpinToSend;
		MPI_Status status;
		MPI_Recv(&SlaveID, 1, MPI_INT, MPI_ANY_SOURCE,
				REQUEST_SPINS, MPI_COMM_WORLD, &status);

		//get next spin paket to send:
		pSam->GetNextPacket(NoSpins,NextSpinToSend,SlaveID);


        //MODIF
        for(ii=0;ii<size;ii++)
        {
            //cout<<ii<<" sencount "<<NoSpins<<" displs "<<NextSpinToSend<<endl;
            if(ii>0 && NextSpinToSend!=nextSpinMem)    {
                MPI_Send(&NextSpinToSend,1,MPI_LONG,SlaveID,SEND_BEGIN_SPIN,MPI_COMM_WORLD);
                //cout<<endl<<0<<" sent spins index information to "<<SlaveID<<" :  "<<"--- "<<NextSpinToSend<<endl;
                nextSpinMem=NextSpinToSend;
                if(nextSpinMem<0)   nextSpinMem-=1;
                }

        }
        //MODIF***


		if (NoSpins == 0)
			SlavesDone++;

		// receive signal
		for (unsigned int i=0; i < RxCA->GetSize(); i++)
			mpi_recv_paket_signal(RxCA->GetCoil(i)->GetSignal(),SlaveID,i);

		// dump temp signal
		pSam->DumpRestartInfo(RxCA);

		// now send NoSpins
		MPI_Send(&NoSpins,1,MPI_INT,SlaveID,SEND_NO_SPINS, MPI_COMM_WORLD);
		if (NoSpins > 0)
			MPI_Send(&(pSam->GetSpinsData())[NextSpinToSend*pSam->GetNProps()],
					NoSpins, MPI_SPINDATA,SlaveID,SEND_SAMPLE, MPI_COMM_WORLD);

#ifndef HAVE_MPI_THREADS

		// without threads: write progress bar each time new spins are sent:
		static int progress_percent = -1;
		static int spinsdone = 0;
		spinsdone += sendcount[SlaveID];
		sendcount[SlaveID] = NoSpins;

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
	World* pW = World::instance();
	Sample* pSam;

	// get number of spins:
	int nospins;
	MPI_Scatter(NULL,1,MPI_INT,&nospins,1,MPI_INT,0, MPI_COMM_WORLD);

    	//MODIF
	long TotalSpinNumber=0,beginTraj=0;
	//cout<<World::instance()->m_myRank<<" Query spins index information:  "<<endl;
	MPI_Status status;
	MPI_Recv(&TotalSpinNumber,1,MPI_LONG,0,SEND_TOTAL_NO_SPINS,MPI_COMM_WORLD,&status);
	MPI_Recv(&beginTraj,1,MPI_LONG,0,SEND_BEGIN_SPIN,MPI_COMM_WORLD,&status);
	pW->setTrajLoading(beginTraj,TotalSpinNumber);
	int num_traj=beginTraj;
	//cout<<World::instance()->m_myRank<<" Received spins index information:  "<<TotalSpinNumber<<"  "<<num_traj<<endl;
	//MODIF***


	NPoints= (long) nospins;
	//get number of physical properties per spin
	long NProps;
	MPI_Bcast    (&NProps,1,MPI_LONG,0, MPI_COMM_WORLD);

	cout << "!!! " << NProps << endl;
	if (NProps < 10)
		pSam = new Sample();
	else
		pSam = new MultiPoolSample ();


	pSam->CreateSpins(NProps, NPoints);

/* >> required for multi-pool samples with exchange*/
	long hsize = 0;
	MPI_Bcast (&hsize, 1, MPI_LONG,0, MPI_COMM_WORLD);

	pSam->CreateHelper(hsize);
	MPI_Bcast  (pSam->GetHelper(),(int)hsize,MPI_DOUBLE,0, MPI_COMM_WORLD);

	long dsize = 0;
	MPI_Bcast (&dsize, 1, MPI_LONG,0, MPI_COMM_WORLD);

	pSam->CreateDims(dsize);
	MPI_Bcast  (&(pSam->GetSampleDims()[0]),(int)dsize,MPI_INT,0, MPI_COMM_WORLD);

	int csize = 0;
	MPI_Bcast (&csize, 1, MPI_INT,0, MPI_COMM_WORLD);

	pSam->SetNoSpinCompartments(csize);
	pW->SetNoOfSpinProps(pSam->GetNProps());
	pW->SetNoOfCompartments(csize);
	pW->InitHelper(pSam->GetHelperSize());
	pSam->GetHelper( pW->Helper() );

	/* << required for multi-pool samples with exchange*/

	//cout <<endl<< "hostname:  "<< " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  NProps=" << NProps << ":  hsize=" << hsize << " , csize=" << csize << endl << flush;

	MPI_Datatype MPI_SPINDATA = MPIspindata();
	// get sample:
	MPI_Scatterv (NULL,NULL,NULL,MPI_SPINDATA,pSam->GetSpinsData(),nospins,MPI_SPINDATA,0, MPI_COMM_WORLD);
	//get resolution (needed for position randomness)
	MPI_Bcast    (pSam->GetResolution(),3,MPI_DOUBLE,0, MPI_COMM_WORLD);

	int ilen; std::string  hm (MPI_MAX_PROCESSOR_NAME, ' ');
	MPI_Get_processor_name(&hm[0],&ilen);
	//MODIF
	//cout <<endl<< "hostname"<< " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	//     << NPoints << " spins for simulation ..." << endl << flush;
    //MODIF***
/*
	cout << endl << "SLAVE: " << setw(2) << MPI::COMM_WORLD.Get_rank()
		 << " , #Pools= " << pSam->GetNoSpinCompartments() << " , SampleDims=" << pSam->GetSampleDimsSize() << " , SampleProps= " << pSam->GetNProps() << " , FirstPackageSize=" << pSam->GetSize()
		 << " , ExMatDim=" << pSam->GetHelperSize() <<  endl << endl;
*/
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

	//backup dimensions
	vector<size_t> d = samp->GetSampleDims();

	// request new spins:
	MPI_Send(&myRank,1,MPI_INT,0,REQUEST_SPINS,MPI_COMM_WORLD);

	// send signal
	for (unsigned int i=0; i < RxCA->GetSize(); i++)
		mpi_send_paket_signal(RxCA->GetCoil(i)->GetSignal(),i);
	
	// Receive number of spins
	MPI_Status status;
	MPI_Recv(&NoSpins,1,MPI_INT,0,SEND_NO_SPINS,MPI_COMM_WORLD,&status);
	
    	//MODIF
	long TotalSpinNumber=World::instance()->getTrajNumber(),beginTraj=0;
	//cout<<World::instance()->m_myRank<<" query spins index information:  "<<endl;
	//MPI_Recv(&TotalSpinNumber,1,MPI_LONG,0,SEND_TOTAL_NO_SPINS,MPI_COMM_WORLD,&status);
	MPI_Recv(&beginTraj,1,MPI_LONG,0,SEND_BEGIN_SPIN,MPI_COMM_WORLD,&status);
	World::instance()->setTrajLoading(beginTraj,TotalSpinNumber);
	//cout<<endl<<World::instance()->m_myRank<<" received spins index information:  "<<TotalSpinNumber<<"  "<<beginTraj<<endl;
	//MODIF***

	// Spins left?
	if (NoSpins == 0) return false;
	
	// prepare sample structure
	samp->ClearSpins();
	samp->CreateSpins(NoSpins);
	samp->SetSampleDims(d);


	
	// Receive
	MPI_Datatype MPI_SPINDATA = MPIspindata();
	MPI_Recv(samp->GetSpinsData(),NoSpins,MPI_SPINDATA,0,SEND_SAMPLE,MPI_COMM_WORLD,&status);
	int ilen; std::string hm (MPI_MAX_PROCESSOR_NAME, ' ');
	MPI_Get_processor_name(&hm[0],&ilen);
	//MODIF
	//cout <<endl<< "hostname" << " -> slave " << setw(2) << MPI::COMM_WORLD.Get_rank() << ":  received "
	//     << NoSpins << " spins for simulation ..." << endl << flush;
    //pW->setTrajLoading(myRank,NoSpins);
    //MODIF***
/*
	cout << endl << "SLAVE: " << setw(2) << MPI::COMM_WORLD.Get_rank()
		 << " , #Pools= " << samp->GetNoSpinCompartments() << " , SampleDims=" << samp->GetSampleDimsSize() << " , SampleProps= " << samp->GetNProps() << " , NextPackageSize=" << samp->GetSize()
		 << " , ExMatDim=" << samp->GetHelperSize() <<  endl << endl;
// */
	return true;

}
/*****************************************************************************/
void mpi_send_paket_signal (Signal* pSig, const int CoilID) {
	
	int     tsize = (int) pSig->Repo()->Size();
	
	if (World::instance()->m_myRank == 1)
		MPI_Send(pSig->Repo()->Times(), (int) pSig->Repo()->Samples(), MPI_DOUBLE, 0, SIG_TP + (CoilID)*4,MPI_COMM_WORLD);

	MPI_Send(pSig->Repo()->Data(), tsize, MPI_DOUBLE, 0, SIG_MX + (CoilID)*4,MPI_COMM_WORLD);

	
}

/*****************************************************************************/
void mpi_recv_paket_signal(Signal *pSig, const int SlaveId, const int CoilID) {

	int     tsize = (int) pSig->Repo()->Size();
	double* data  = pSig->Repo()->Data(); 
	std::vector<double> tmp (tsize);
	MPI_Status status;
	
	if (SlaveId == 1)
	  MPI_Recv(pSig->Repo()->Times(), (int) pSig->Repo()->Samples(), MPI_DOUBLE, SlaveId, SIG_TP + (CoilID)*4,MPI_COMM_WORLD,&status);
	
	MPI_Recv(&tmp[0], tsize, MPI_DOUBLE, SlaveId, SIG_MX + (CoilID)*4,MPI_COMM_WORLD,&status);

	for (int i = 0; i < tsize; i++) 
		data[i] += tmp[i];

}

#endif
