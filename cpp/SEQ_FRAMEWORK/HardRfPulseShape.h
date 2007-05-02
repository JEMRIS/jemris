/*
  This file is part of the MR simulation project
  Date: 03/2006
  Author:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _HARDRFPULSESHAPE_H_
#define _HARDRFPULSESHAPE_H_

#include "RfPulseShape.h"

class HardRfPulseShape :public RfPulseShape {
	
 public:
	// create object with flipangle, phase and duration
	HardRfPulseShape (double dFlipAngle = 0, double dPhase=0, double dDuration=0,
					  string sName="HardRfPulseShape" ) {
		setName      (sName);
		setFlipAngle (dFlipAngle);
		setPhase     (dPhase);
		setDuration  (dDuration);
	};
	
	~HardRfPulseShape(){};
	
	void getValue (double * dAllVal, double const time, int const iLoop) {
		double dT=getDuration();
		if ( time >= 0 && time <= dT ) {
			dAllVal[0] += getFlipAngle() * PI /(180.0*dT);
			dAllVal[1] += getPhase()     * PI / 180.0;
		}
	};
	
};

#endif


