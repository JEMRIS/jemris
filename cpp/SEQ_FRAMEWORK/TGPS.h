/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _TGPS_H_
#define _TGPS_H_ 

#include <cmath>

#include "GradientPulseShape.h"

//trapezoidal gradient pulse shape (e.g. for cartesian k-space imaging)
class TGPS :public GradientPulseShape{
	
 public:
	TGPS (double dArea=0.0, PulseAxis eAxis=AXIS_GX, string sName="TGPS") {
		setName(sName);
		setAxis(eAxis);
		m_dArea     =  dArea;
		m_bNoRamps  =  false;
    	m_dAsymSR   =  0.0;
		m_dSlopeUp  =  getSlewRate();
		m_dSlopeDn  = -1.0*getSlewRate();
		m_bVerbose  =  false;
		Prepare(m_bVerbose);
	};

	bool Prepare(bool verbose){
        m_bVerbose  =  verbose;
		setLimits();
		GradientPulseShape* pLinkedPulse = NULL;

		if (getAtomicSeq() != NULL) 
			pLinkedPulse = (GradientPulseShape*)getAtomicSeq()->getRoot()->FindPulse(LinkToPulse());
		if (pLinkedPulse   != NULL)
			m_dArea = getFactor()*pLinkedPulse->getArea();
		if (getAreaMethod()>0)
			m_dArea = getFactor()*getAtomicSeq()->getRoot()->getParameter()->getArea(getAreaMethod());

		setArea(m_dArea);
		
		if (NewDuration()  >  0) {
			setDuration(NewDuration());
			if (getNumOfADCs() > 0) setNumOfADCs(getNumOfADCs());
		}
		return true;
	};
	
	~TGPS(){};
	
	
	//  
	void  getValue         (double * dAllVal, double const time, int const iLoop);
	void  setNLPs          ();
	
	//virtual empty method CheckLoop() which is called from getValue();
	//overload CheckLoop() for derived classes which connect the TGPS
	//to a loop counter, e.g. for phase encode tables
	virtual void CheckLoop (const int iLoop){};
	
	//return parameters describing the TGPS
	double getAmplitude    () {return m_dAmplitude;}; 
	double getFlatTopArea  () {return m_dFlatTopArea;}; 
	double getFlatTopTime  () {return m_dFlatTopTime;}; 
	double getRampUpTime   () {return m_dRampUpTime;}; 
	double getRampDnTime   () {return m_dRampDnTime;}; 
	double getSlopeUp      () {return m_dSlopeUp;}; 
	double getSlopeDn      () {return m_dSlopeDn;}; 
	double getRampDnStart  () {return (m_dRampUpTime+m_dFlatTopTime);}; 
	
	void    NoRamps         () {m_bNoRamps=true;};
	double  getAsymSR       () {return m_dAsymSR;};
	void    setAsymSR       (double value) {m_dAsymSR=value;};
	
	void    setArea         (double value); //prepares in minimal possible duration
	void    setFlatTopArea  (double value); //prepares in minimal possible duration
	void    setDuration     (double value); //preserves total area
	void    setFlatTopTime  (double value); //preserves flat top area
	
 protected:
	double 	m_dAmplitude;
	double 	m_dFlatTopArea;
	double 	m_dFlatTopTime;
	double 	m_dRampUpTime;
	double 	m_dRampDnTime;
	double 	m_dSlopeUp;
	double 	m_dSlopeDn;
	double 	m_dAsymSR;
	bool    m_bNoRamps;
	bool	m_bVerbose;

};//end of Class

#endif

