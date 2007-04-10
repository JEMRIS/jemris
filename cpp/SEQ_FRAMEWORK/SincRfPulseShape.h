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
   SincRfPulseShape (double dFlipAngle=90.0, double dPhase=0, double dBW=10,
		     int iN=2 double dalpha=0.5, string sName="SincRfPulseShape" ) {
	setName(sName);
	setFlipAngle(dFlipAngle) ;
	setPhase (dPhase);
	setDuration( (2.0*iN)/dBW );
	m_iN=iN;
	m_dBW=dBW;
 	m_dalpha=dalpha;
   };

// magnitude calculation
  bool Prepare(bool verbose){
	double t0=1.0/m_dBW, Sum = 0.0, DeltaT = getDuration()/10000.0 ;
	for (double t=-m_iN*t0; t<=m_iN*t0; t+=DeltaT)
	{
		sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		Sum += (1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
	}
	m_dAamplitude = getFlipAngle()*PI/(DeltaT*Sum*180.0);
 	return true;
  };

  ~SincRfPulseShape(){};

  void getValue(double * dAllVal, double const time,int const iLoop){
	double dT=getDuration();
	if ( time >= 0 && time <= dT )
	{
		double t0=1.0/m_dBW;
		double t=time-m_iN*t0;
		double sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		double dVal = m_dAamplitude*(1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
		dAllVal[0] += fabs(dVal);
		dAllVal[1] += (dVal<0.0?PI:0.0);
		dAllVal[1] += getPhase() * PI / 180.0;
	}
  };

private:
 double m_dAamplitude, m_dalpha, m_dBW;
 int m_iN;

};

#endif


