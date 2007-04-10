/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SS_TGPS_H_
#define _SS_TGPS_H_

#include "TGPS.h"

class SS_TGPS :public TGPS{
public:
 //create with area, flat-top-time, number of ADCs, Axis, and name
 SS_TGPS(PulseShape* pPulse, PulseAxis eAxis=AXIS_GZ, string sName="SS_TGPS" ) {
 	setName(sName);
	setAxis(eAxis);
	m_bVerbose=false;
	Prepare(m_bVerbose);
 };

 ~SS_TGPS(){};


  bool Prepare(bool verbose){
	return true;
 };

};//end of Class

#endif
