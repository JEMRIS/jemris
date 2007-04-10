/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RFPULSESHAPE_H_
#define _RFPULSESHAPE_H_


#include "PulseShape.h"

class RfPulseShape :public PulseShape{

public:
   RfPulseShape (){ setAxis(AXIS_RF); setPhaseLock(false); };
  ~RfPulseShape(){};
   
  virtual void getValue(double * dAllVal, double const time,int const iLoop){};

  double getFlipAngle ( ){return m_dFlipAngle;}; 
  void setFlipAngle (double value ){m_dFlipAngle= value;};
  double getPhase ( ){return m_dPhase;}; 
  void setPhase (double value ){m_dPhase= value;};

 bool getPhaseLock(){return m_bPhaseLock;};
 void setPhaseLock(bool bval){m_bPhaseLock=bval;};
protected:

  double	m_dFlipAngle;
  double	m_dPhase;
  bool		m_bPhaseLock;

public:
};

#endif

