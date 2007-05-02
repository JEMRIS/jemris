/*
  This file is part of the MR simulation project
  Date: 03/2006
  Author:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/



#include <iostream>
using namespace std;

#include "RfPulseShape.h"
#include "PulseShape.h"


void   HardRfPulseShape::getValue (double * dAllVal, double const time, 
								int const iLoop) {
	
	double dT=getDuration();
	if ( time >= 0 && time <= dT ) {
		dAllVal[0] = getFlipAngle() * PI /(180.0*dT);
		dAllVal[1] = getPhase()     * PI / 180.0;
	}
	return ;
}

double HardRfPulseShape::getMagnitudeValue (double time) {
	return (getFlipAngle() * PI /(180.0*getDuration()));
}

double HardRfPulseShape::getPhaseValue (double time) {
	if (time==time);
	return (getPhase()*1.0);
}

