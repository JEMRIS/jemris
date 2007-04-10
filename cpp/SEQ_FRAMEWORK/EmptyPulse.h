/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _EMPTYPULSE_H_
#define _EMPTYPULSE_H_

#include "PulseShape.h"

class EmptyPulse :public PulseShape{

public:
 EmptyPulse(double dDuration=0.0, int iADCs=0, string PulseName="EmptyPulse") {
	setName(PulseName);
	setDuration(dDuration);
	setNumOfADCs(iADCs);
	setNLPs();
 };
 ~EmptyPulse(){};

 void getValue(double* dAllVal, double const time,int const iLoop){};

}; 

#endif
