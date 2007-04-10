/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;
#include "AllSequences.h"	//the sequences
#include "AllPulses.h"		//the pulseshapes
#include "GradientSpiralPart.h"

//Sequence Class
class  GradientSpiralConcatSequence :public ConcatSequence
{
public:
 //constructor
 GradientSpiralConcatSequence(int iparts=128, double ddur=20, double dturns=32, double dtune=0.2, double dres=1.0)
 { Create(iparts, ddur, dturns, dtune, dres); Prepare(false); };
 //destructor
 ~GradientSpiralConcatSequence(){ };


 void Create(int iparts, double ddur, double dturns, double dtune, double dres)
 {
  //create concat sequences 
  ConcatSequence* C = this;
  C->setName("GradientSpiralConcatSequence");

  AtomicSequence* A_Parts[iparts];
  for (int i=0; i<iparts; ++i)
  {
	A_Parts[i] = new AtomicSequence("A_Segment");
	A_Parts[i]->setPulse(new HardRfPulseShape(i*1.0, 90.0*pow(-1.0,i), ddur/iparts, "RF" ) );
	A_Parts[i]->setPulse(new GradientSpiralPart(iparts, i+1, ddur, dturns, dtune, dres, AXIS_GX) );
	A_Parts[i]->setPulse(new GradientSpiralPart(iparts, i+1, ddur, dturns, dtune, dres, AXIS_GY) );
        if (i==0) { C->InsertChild(NULL,A_Parts[i]); }
        else	  { C->InsertChild(A_Parts[i-1],A_Parts[i]); }
	//cout << "inserted part " << i << " , dur = " << ((GradientSpiralPart*) (A_Parts[i]->getPulse(0)))->getDuration() << endl;
  }

 };//end of Create() function
};//end of class

