/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RFRECEIVERPHASE_H_
#define _RFRECEIVERPHASE_H_

#include "RfPulseShape.h"

class RfReceiverPhase :public RfPulseShape{

public:

   RfReceiverPhase  (double dPhase=0, string sName="RfReceiverPhase" ) {
	setName(sName);
	m_dPhase = dPhase;
  	setPhaseLock(true);
   };

  ~RfReceiverPhase  (){};

};

#endif


