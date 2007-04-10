/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RFPHASECYCLING_H_
#define _RFPHASECYCLING_H_

#include "RfPulseShape.h"

class RfPhaseCycling :public RfPulseShape{

public:
   RfPhaseCycling (double* dPhases, int iFold, double dDuration, string sName="RfPhaseCycling" ) {
	setName(sName);
	for (int i=0;i<iFold;i++) { m_dPhases[i] = dPhases[i]; }
	m_iFold   = iFold;
	setPhaseLock(true);
	setDuration(dDuration);
   };

  ~RfPhaseCycling (){};

  void getValue(double* dAllVal, double const time,int const iLoopCounter){

	if ( time < 0 || time > getDuration()) return;
	m_dPhase = m_dPhases[iLoopCounter%m_iFold];
	dAllVal[1] += m_dPhase* PI / 180.0;
  };

private:
	double m_dPhases[128];
	int     m_iFold;

};

#endif


