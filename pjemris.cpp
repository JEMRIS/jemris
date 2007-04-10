/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;
#include "mpi_mrsim.h"		//functions to send/receive sample and signal
#include "XmlSequence.h"	//Provides parsing of seq xml-files and generation of seq objects
#include "XmlSimulation.h"	//Provides Sample and Model parsed from xml simu file

int main (int argc, char *argv[])
{
	//init MPI
	MPI::Init(argc, argv);
	int my_rank = MPI::COMM_WORLD.Get_rank();
	int master=0, second=1, tag=42;
	double t1 = MPI::Wtime();

	if (argc<3)
	{
		if ( my_rank == master) cout << "usage: pjemris <seq.xml> <simu.xml>" << endl;
		return 0;
	}

	//parse XML sequence and create sequence object
	XmlSequence* xSeq = new XmlSequence(argv[1]);
	if (!xSeq->Status()) {cout << argv[1] << "is not a valid sequence xml file" << endl; return -1;} 
	ConcatSequence* pSeq = xSeq->getSequence(false); 
	delete xSeq;

	//parse XML simulation file
	XmlSimulation* xSim = new XmlSimulation(argv[2],pSeq);
	if (!xSim->Status()) { if (my_rank == master) {cout << argv[2] << " is not a valid simulation xml file" << endl;} return -1;} 

	//MASTER: writes seq-file, generates and sends sample
	if ( my_rank == master)
	{
		cout << endl << "parallel MR SIMULATION"  << endl << endl << "Sequence: '"<< pSeq->getName()
		<< "' , duration = " << pSeq->getDuration() << "msec" << endl;
		pSeq->writeSeqDiagram("seq.bin");
		Sample* pSam = xSim->getSample(true);
		mpi_devide_and_send_sample( pSam, tag );
		delete pSam; delete pSeq ; delete xSim;
	}
	//SLAVE: receives the (sub)sample, generates and solves model, then sends (sub)signal
	else
	{
		Sample* pSam = mpi_receive_sample(master, tag);
		xSim->setSample(pSam);
		Signal* pSig ;
		bool bverbose = false;
		if ( my_rank == second) bverbose=true;
		MR_Model* pMod = xSim->getModel(bverbose);
		if ( xSim->getEvolution()>0 ) //store evolution of magnetisation
		{
			char IntStr[10] ; sprintf( IntStr, "%02d", my_rank ) ;
			string fn="sim_data"; fn += IntStr; fn += ".bin";
			pSig = pMod->Solve(false,true,&fn,xSim->getEvolution());
		}
		else {	pSig = pMod->Solve();  }
		mpi_send_signal(pSig, master, my_rank==second, tag);
		delete pMod; delete pSam; delete pSig; delete pSeq; delete xSim;
	}

	//MASTER: receives (and sums) signals, then writes result to file
	if ( my_rank == master)
	{
		Signal* pSig = mpi_receive_and_sum_signal(second, tag);
		pSig->writeBinaryFile("signal.bin");
		delete pSig ;
	}

	//finished
	MPI::COMM_WORLD.Barrier();
	double t2 = MPI::Wtime();
	if ( my_rank == master)
		cout << endl << "Finished!  MPI Process Wtime = " << t2-t1 << " seconds" << endl;
	MPI::Finalize();
	return 0;
}

