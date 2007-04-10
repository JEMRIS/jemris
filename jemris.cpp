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
	if (argc==1) {cout << "usage: jemris <seq.xml> <simu.xml>" << endl; return 0;}
	XmlSequence* xSeq = new XmlSequence(argv[1]);
	if (!xSeq->Status()) {cout << argv[1] << " is not a valid sequence xml file" << endl; return -1;} 
	bool bverbose = false;
	if (argc==2) {bverbose = true;} 
	ConcatSequence* pSeq = xSeq->getSequence(bverbose);
	delete xSeq;

	if (argc==2) {delete pSeq; return 0;} //exit, if no simu.xml file

	XmlSimulation* xSim = new XmlSimulation(argv[2],pSeq);
	if (!xSim->Status()) {cout << argv[2] << " is not a valid simulation xml file" << endl; return -1;} 
	cout <<  endl << "MR SIMULATION" << endl << endl << "Sequence : '"<< pSeq->getName() 
		<< "' , duration = " << pSeq->getDuration() << "msec" << endl;
	Sample* pSam   = xSim->getSample(true);
	MR_Model* pMod = xSim->getModel(true);
	if (pMod==NULL) {delete pSeq; delete pSam; delete xSim; return 0;} //exit, if no Model in simu.xml file
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

