/*
  This file is part of the MR simulation project
  Date: 03/2006
  Author:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _GradientSpiralExtRfConcatSequence_H_
#define _GradientSpiralExtRfConcatSequence_H_

#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;
#include "AllSequences.h"	//the sequences
#include "AllPulses.h"		//the pulseshapes
#include "GradientSpiralPart.h"

//Sequence Class
class  GradientSpiralExtRfConcatSequence :public ConcatSequence {
 public:
	//constructor
	GradientSpiralExtRfConcatSequence(string filename, double dturns=32.0,
									  double dtune=0.2, double dres=1.0,
									  string name="GradientSpiralExtRfConcatSequence") {
		Create(filename, dturns, dtune, dres, name); Prepare(false); 
	};
	//destructor
	~GradientSpiralExtRfConcatSequence(){ };


	void Create(string filename, double dturns, double dtune, double dres, string name) {
		//create concat sequences 
		ConcatSequence* C = this;
		C->setName(name);
//cout << "GSERF bhhgng 1 " << endl;
		
		ExternalPulseShape* pEPS = new ExternalPulseShape(filename);
		int     iparts   = pEPS->getNumOfNLPs();
		double  ddur     = pEPS->getDuration();
		double* dRFamplt = pEPS->getValArray1();//B1 amplitude
		double* dRFphase = pEPS->getValArray2();//phases in rad
		double  dRFdur   = ddur/iparts;
		
//cout << "GSERF bhhgng 2 " << iparts << endl;
		AtomicSequence** A_Parts = new AtomicSequence*[iparts];
		for (int i=0; i<iparts; ++i) {
			//cout << "inserted part " << i ;
			A_Parts[i] = new AtomicSequence("A_Segment");
			//cout << "! " ;
			A_Parts[i]->setPulse(new HardRfPulseShape(dRFamplt[i]*dRFdur*180.0/PI, dRFphase[i]*180.0/PI, dRFdur, "RF" ) );
			A_Parts[i]->setPulse(new GradientSpiralPart(iparts, i+1, ddur, dturns, dtune, dres, AXIS_GX) );
			A_Parts[i]->setPulse(new GradientSpiralPart(iparts, i+1, ddur, dturns, dtune, dres, AXIS_GY) );
			//cout << "! " ;
			if (i==0) { C->InsertChild(NULL,A_Parts[i]); }
			else	  { C->InsertChild(A_Parts[i-1],A_Parts[i]); }
			//cout << "! " ;
			RfPulseShape* pRF = ((RfPulseShape*) A_Parts[i]->getPulse(0));
			//cout << "! " << endl;
			//cout << "inserted part " << i << " , dur = " << ((GradientSpiralPart*) (A_Parts[i]->getPulse(0)))->getDuration()
			// << "   RF = (" << pRF->getFlipAngle() << "," << pRF->getPhase() << ")" << endl;
		}
		delete A_Parts; delete pEPS;
//cout << "GSERF bhhgng 3 " << endl;
		double dAreaX = ((GradientSpiralPart*) A_Parts[0]->getPulse(1))->getGS()->getArea();
		double dAreaY = ((GradientSpiralPart*) A_Parts[0]->getPulse(2))->getGS()->getArea();
		AtomicSequence* A_reph = new AtomicSequence("A_rephaser");
		A_reph->setPulse(new TGPS(-1.0*dAreaX, AXIS_GX, "GRephase_X") );
		A_reph->setPulse(new TGPS(-1.0*dAreaY, AXIS_GY, "GRepahse_Y") );
		((GradientPulseShape*) A_reph->getPulse(0))->setSlewRate(100.0);
		((GradientPulseShape*) A_reph->getPulse(0))->setMaxAmpl(10.0);
		((GradientPulseShape*) A_reph->getPulse(1))->setSlewRate(100.0);
		((GradientPulseShape*) A_reph->getPulse(1))->setMaxAmpl(10.0);
		C->InsertChild(A_Parts[iparts-1],A_reph);
		
	};//end of Create() function

};//end of class

#endif
