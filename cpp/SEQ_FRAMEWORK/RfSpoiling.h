/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RFSPOILING_H_
#define _RFSPOILING_H_

#include "RfPulseShape.h"

class RfSpoiling :public RfPulseShape{

public:
  RfSpoiling (double dQuadPhaseInc=0, double dDuration=0, double iNpp=0, string sName="RfSpoling" ) {
	setName(sName);
	m_dQuadInc = dQuadPhaseInc;
	setDuration( dDuration);
  	setPhaseLock(true);
	m_iNpp=iNpp;
	Prepare(false);
   };

  ~RfSpoiling (){};

  bool Prepare(bool verbose){
	m_dPhase = 0.0;
	m_dInc   = 0.0;
	m_iLastLoopCounter = -m_iNpp;
	//assume that iNpp steps were taken before already
	for (int i=0;i<m_iNpp;++i)
	{
		m_dInc   += m_dQuadInc;
		m_dPhase += m_dInc;
		m_dInc   = fmod(m_dInc,360.0)  ;
		m_dPhase = fmod(m_dPhase,360.0)  ;
	}
	return true;
  };


  void getValue(double* dAllVal, double const time,int const iLoopCounter){

	if ( time < 0 || time > getDuration()) return;

	if (iLoopCounter != m_iLastLoopCounter) 
	{
		m_iLastLoopCounter = iLoopCounter;
		m_dInc   += m_dQuadInc;
		m_dPhase += m_dInc;
		m_dInc   = fmod(m_dInc,360.0)  ;
		m_dPhase = fmod(m_dPhase,360.0)  ;
	}

	dAllVal[1] += m_dPhase * PI / 180.0;
  };

private:
	double m_dInc;
	double m_dQuadInc;
	double m_iLastLoopCounter;
	double m_iNpp;

};

#endif


