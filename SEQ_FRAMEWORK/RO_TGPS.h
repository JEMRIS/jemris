/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _RO_TGPS_H_
#define _RO_TGPS_H_

#include "TGPS.h"

class RO_TGPS :public TGPS{
public:
 //create with area, flat-top-time, number of ADCs, Axis, and name
 RO_TGPS(double dAFT=0.0, double dTFT=0.0, int iADCs=0,
	   PulseAxis eAxis=AXIS_GX, string sName="RO_TGPS" ) {
 	setName(sName);
	setAxis(eAxis);
	Initialize(dAFT,dTFT,iADCs);
	m_bVerbose=false;
	Prepare(m_bVerbose);
 };

 ~RO_TGPS(){};

 void Initialize(double dAFT, double dTFT, int iADCs){
	m_dFlatTopArea=dAFT;
	m_dTFT=dTFT;
	m_iNumADCs=iADCs;
 };

  bool Prepare(bool verbose){
        m_bVerbose=verbose;
	if ( getAreaMethod()==1 ) //along X axis
	{
		m_dFlatTopArea = getFactor()*2.0*getAtomicSeq()->getRoot()->getParameter()->getKMAXx();
		m_iNumADCs = getAtomicSeq()->getRoot()->getParameter()->getNx();
	}
	if ( getAreaMethod()==2 ) //along Y axis
	{
		m_dFlatTopArea = getFactor()*2.0*getAtomicSeq()->getRoot()->getParameter()->getKMAXy();
		m_iNumADCs = getAtomicSeq()->getRoot()->getParameter()->getNy();
	}
	if ( getAreaMethod()==3 ) //along Z axis
	{
		m_dFlatTopArea = getFactor()*2.0*getAtomicSeq()->getRoot()->getParameter()->getKMAXz();
		m_iNumADCs = getAtomicSeq()->getRoot()->getParameter()->getNz();
	}

	double dTMP = 0;
	if (getAtomicSeq()!=NULL) dTMP = getAtomicSeq()->getRoot()->getParameter()->getReadBW();
	if ( dTMP > 0 ) m_dTFT = 1./dTMP;

	setFlatTopArea(m_dFlatTopArea);
	setFlatTopTime(m_dTFT);
	setNumOfADCs(m_iNumADCs, getRampUpTime(), getRampDnStart() );
	return true;
 };

private: 
	double m_dTFT;
};//end of Class

#endif
