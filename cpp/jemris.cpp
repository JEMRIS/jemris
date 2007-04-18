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
#include "XmlSequence.h"
#include "XmlSimulation.h"

int main (int argc, char *argv[])
{
	//show info
	if (argc==1)
	{
		 cout << endl << "usage: a) jemris <seq.xml>            " << endl;
		 cout << "       b) jemris <simu.xml>           " << endl;
		 cout << "       c) jemris <seq.xml> <simu.xml> " << endl;
		 cout << endl << "a) stores sequence diagramme (seq.bin) , b) stores sample (sample.bin)," << endl;
		 cout << "c) performs the simulation and stores sample and signal (sample.bin & signal.bin)" << endl << endl;
		 return 0;
	}

	XmlSequence* xSeq = new XmlSequence(argv[1]);		//parse first argument as sequence file
	
	if (argc==2) //only one input file
	{
		if (xSeq->Status()) //sequence parse ok => write sequence diagram (seq.bin)
		{
			ConcatSequence* pSeq = xSeq->getSequence(true);
			delete xSeq; delete pSeq; return 0;
		}
		else
		{
			XmlSimulation* xSim = new XmlSimulation(argv[1],NULL);	//parse first argument as simulation file
			if (!xSim->Status())
			 {cout	<< argv[1] << " is neither a valid sequence xml file, nor a valid simulation xml file"
				<< endl; return -1;} 
			Sample* pSam   = xSim->getSample(true);
			delete pSam; delete xSim;
			return 0;
		}
	}

	if (!xSeq->Status()) //sequence parse ok => write sequence diagram (seq.bin)
		{cout << argv[1] << " is not a valid sequence xml file" << endl; return -1;} 

	ConcatSequence* pSeq = xSeq->getSequence(false); //get sequence object

	XmlSimulation* xSim = new XmlSimulation(argv[2],pSeq);

	if (!xSim->Status())
		{cout << argv[2] << " is not a valid simulation xml file" << endl; return -1;} 

	cout <<  endl << "MR SIMULATION" << endl << endl << "Sequence : '"<< pSeq->getName() 
		<< "' , duration = " << pSeq->getDuration() << "msec" << endl;

	Sample* pSam   = xSim->getSample(true);
	MR_Model* pMod = xSim->getModel(true);

	Signal* pSig ;
	if ( xSim->getEvolution()>0 )
		{ string fn="sim_data.bin"; pSig = pMod->Solve(false,true,&fn,xSim->getEvolution()); }
	else
		{ pSig = pMod->Solve(); }
	pSig->writeBinaryFile("signal.bin");
	cout <<  endl << "Successful! Solution saved to signal.bin" << endl;
	delete xSim; delete pMod; delete pSam; 

	cout << endl;
	delete pSeq;
}

