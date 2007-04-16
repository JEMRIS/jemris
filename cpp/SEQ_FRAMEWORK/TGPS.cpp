/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/


#include <iostream>
using namespace std;

#include "TGPS.h"

void  TGPS::setArea(double value) {
	m_dArea=value;
	if (m_dArea==0.0) return;
	double dAbsArea = fabs(m_dArea);
	double dSign = m_dArea/dAbsArea;
	double Gmax = getMaxAmpl();
	m_dSlopeUp = dSign*getSlewRate();
	m_dSlopeDn = -1.0*dSign*getSlewRate();
	if (getAsymSR() > 0.0) m_dSlopeUp *= getAsymSR();
	if (getAsymSR() < 0.0) m_dSlopeDn *= fabs(getAsymSR());
	double dC = 1.0/fabs(2.0*getSlopeUp()) + 1.0/fabs(2.0*getSlopeDn());

	if (dAbsArea <= Gmax*Gmax*dC )	//triangle shape (no flat top)
	{
		m_dFlatTopArea  = 0.0;
		m_dAmplitude  = dSign*sqrt( dAbsArea / dC ) ;
	}
	else				//trapezoidal shape (with flat top)
	{
		m_dFlatTopArea= dSign* ( dAbsArea - Gmax*Gmax*dC );
		m_dAmplitude  = dSign*Gmax;
	}
	m_dFlatTopTime= fabs(m_dFlatTopArea/m_dAmplitude);
       	m_dRampUpTime = fabs(m_dAmplitude/m_dSlopeUp);
       	m_dRampDnTime = fabs(m_dAmplitude/m_dSlopeDn);
       	m_dDuration   = m_dRampUpTime + m_dFlatTopTime + m_dRampDnTime;
	//set the nonlinear points of the gradient shape
	setNLPs();
};

void  TGPS::setNLPs() {
	m_iNumNLPs=4;
	m_dArrayOfNLPs[0]=0.0;
	m_dArrayOfNLPs[1]=getRampUpTime();
	m_dArrayOfNLPs[2]=getRampDnStart();
	m_dArrayOfNLPs[3]=getDuration();
};

void  TGPS::setFlatTopArea(double value) {
	if (value==0.0) return;
	double dC = 2.0/fabs(2.0*getSlewRate());
	setArea( value *( 1.0 + getMaxAmpl()*getMaxAmpl()*dC / fabs(value) ) );
};

void TGPS::setDuration(double dT) {
	if (m_dArea==0.0) { m_dDuration = dT; return; }
	setArea(m_dArea); //calculate in shortest possible time
	if (dT < getDuration())
	{
		if (m_bVerbose)
		{
                	cout << "warning: duration too short for TGPS: " << getName() ;
                	if (getAtomicSeq()!=NULL)
                     		cout << " (in AtomicSequence: " << getAtomicSeq()->getName() << ")";
                	cout << endl;
		}
		return;
	}
	double dGmax = getMaxAmpl();
	double dC = 1.0/fabs(2.0*getSlopeUp()) + 1.0/fabs(2.0*getSlopeDn());
	m_dMaxAmpl = ( dT - sqrt(dT*dT - 4*fabs(m_dArea)*dC) )/(2.0*dC);
	setArea(m_dArea);   //recalculate the gradient shape
	m_dMaxAmpl = dGmax; //reset maximum amplitude
};

void  TGPS::setFlatTopTime(double dTFT){
	setArea(m_dArea); //calculate in shortest possible time
	if (dTFT < getFlatTopTime())
	{
		if (m_bVerbose)
		{
                	cout << "warning: FlatTopTime too short for GradientPulseShape: " << getName() ;
                	if (getAtomicSeq()!=NULL)
                     		cout << " (in AtomicSequence: " << getAtomicSeq()->getName() << ")";
                	cout << endl;
		}
		return;
	}
	double dGmax = getMaxAmpl(), dAFT=getFlatTopArea();
	m_dMaxAmpl = fabs(dAFT/dTFT);
	setFlatTopArea(dAFT);
	m_dMaxAmpl = dGmax; //reset maximum amplitude
};

void TGPS::getValue(double * dAllVal, const double time, int const iLoop) {
	CheckLoop(iLoop);
	if ( m_bNoRamps ) { dAllVal[getAxis() + 1] += getAmplitude(); return ; }
        if (time <0.0 || time >getDuration() || getArea()==0.0) { return; }
	//on the ramp up ?
	double t=time, dT =getRampUpTime();
	if (t < dT ) { dAllVal[getAxis() + 1] += t * getSlopeUp(); return ; }
	t -= dT;
	//on the flat top ?
	dT =getFlatTopTime();
	if (t < dT ) { dAllVal[getAxis() + 1] += getAmplitude(); return ; }
	t -= dT;
	//on the ramp down!
	dAllVal[getAxis() + 1] += getAmplitude() + t * getSlopeDn();
};

