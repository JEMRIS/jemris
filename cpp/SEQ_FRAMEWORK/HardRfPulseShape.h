/*
  This file is part of the MR simulation project
  Date: 03/2006
  Author:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _HARDRFPULSESHAPE_H_
#define _HARDRFPULSESHAPE_H_

#include "RfPulseShape.h"
#include <vector>

class HardRfPulseShape :public RfPulseShape {
	
 public:
	// create object with flipangle, phase and duration
	HardRfPulseShape (double dFlipAngles[], double dPhases[], double dDuration=0,
					  string sName="HardRfPulseShape" ) {
		setName       (sName);
 		setFlipAngles (dFlipAngles);
		setPhases     (dPhases);
		setDuration   (dDuration);

	};
	
	HardRfPulseShape (double dFlipAngle = 0, double dPhase = 0, double dDuration = 0,
					  string sName="HardRfPulseShape" ) {
		setName      (sName);
		setFlipAngle (dFlipAngle);
		setPhase     (dPhase);
		setDuration  (dDuration);

	};
	
	~HardRfPulseShape(){};
	
	void getValue (double * dAllVal, double const time, int const iLoop) {
		double dT=getDuration();

		// cout << "using angle:" << getFlipAngle(iLoop) << " phase: " << getPhase(iLoop) << " in loop: " << iLoop << "\n";
		
		if ( time >= 0 && time <= dT ) {
			dAllVal[0] += getFlipAngle(iLoop) * PI /(180.0*dT);
			dAllVal[1] += getPhase(iLoop)     * PI / 180.0;
		}
	};
	
};

#endif


