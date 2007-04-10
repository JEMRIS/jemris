/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SINCRFPULSESHAPE_H_
#define _SINCRFPULSESHAPE_H_

#include "RfPulseShape.h"

class SincRfPulseShape :public RfPulseShape{

public:
   SincRfPulseShape (double dFlipAngle=0, double dPhase=0, double dBW,
		     int iNL=2, int iNR=2, double dalpha=0.5, string sName="SincRfPulseShape" ) {
	setName(sName);
	setFlipAngle(dFlipAngle) ;
	setPhase (dPhase);
	setDuration( dDuration);
	m_iNL=iNL;
 	m_iNR=iNR;
 	m_dalpha=dalpha;
   };

  ~SincRfPulseShape(){};

  void getValue(double * dAllVal, double const time,int const iLoop){
	double dT=getDuration();
	if ( time >= 0 && time <= dT )
	{
		double dVal = 
		dAllVal[0] += dVal;
		dAllVal[1] += getPhase() * PI / 180.0;
	}
  };

private:
 double m_dAamplitude, m_dalpha;
 int m_iNR, m_iNL;

};

#endif


