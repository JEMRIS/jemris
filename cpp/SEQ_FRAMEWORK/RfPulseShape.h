/*
  This file is part of the MR simulation project
  Date: 03/2006
  Author:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RFPULSESHAPE_H_
#define _RFPULSESHAPE_H_

#include "PulseShape.h"
#include <vector>

class RfPulseShape :public PulseShape {
	
 public:
	RfPulseShape          () {m_dBW=0.0; setAxis(AXIS_RF); setPhaseLock(false); };
	~RfPulseShape         () {};
	
	virtual void getValue (double * dAllVal, double const time,int const iLoop){};
	
	double  getFlipAngle  () {return m_dFlipAngle;}; 
	void    setFlipAngle  (double value) {m_dFlipAngle = value;  m_dFlipAngles[0]=value; };

	double  getPhase      () {return m_dPhase;}; 
	void    setPhase      (double value) {m_dPhase     = value; m_dPhases[0]=value; };
	
	double  getFlipAngle  (int iLoop) {return m_dFlipAngles[iLoop];}; 
	void    setFlipAngles (double values[]) 
		{for (int i = 0; i < sizeof(values); ++i)
			m_dFlipAngles[i] = values[i];};

	double  getPhase      (int iLoop) {return m_dPhases[iLoop];}; 
	void    setPhases     (double values[]) 
		{for (int i = 0; i < sizeof(values); ++i)
			m_dPhases[i] = values[i];};
	
	bool    getPhaseLock  () {return m_bPhaseLock;};
	void    setPhaseLock  (bool bval)    {m_bPhaseLock = bval;};
	
	double  getBandWidth  () {return m_dBW;}
	
 protected:
	
	double	m_dFlipAngle;
	double	m_dPhase;

	double  m_dFlipAngles[100];
	double  m_dPhases[100];
	
	bool	m_bPhaseLock;
	//! bandwidth of the pulse
	double  m_dBW;
	
};

#endif

