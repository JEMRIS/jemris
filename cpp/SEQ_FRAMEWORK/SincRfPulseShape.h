/*
	This file is part of the MR simulation project
	Date: 04/2007
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SINCRFPULSESHAPE_H_
#define _SINCRFPULSESHAPE_H_

#include "RfPulseShape.h"

 //! possible slice order acquisition schemes
enum SLICE_ORDER {LINEAR, INTERLEAVED};

//! symmetric apodized sinc-shaped RF pulse
/*! 
 *  The Sincpulse is defined by Flipangle, Phase, Bandwidth,
 *  number of zero crossings, and the apodization factor.
 *  Further, it has functionality for frequency modulation within a loop,
 *  in order to shift the slice-position for slice selective excitation.
 *  Formulas from Bernstein et al., Handbook of MRI Pulse Sequences
 */
class SincRfPulseShape :public RfPulseShape{

public:
   SincRfPulseShape (double dFlipAngle=90.0, double dPhase=0.0, double dBW=10.0, int iN=2,
		     double dalpha=0.5, SLICE_ORDER SO=LINEAR,
		     string sName="SincRfPulseShape" ) {
	setName(sName);
	setFlipAngle(dFlipAngle) ;
	setPhase (dPhase);
	setDuration( (2.0*iN)/dBW );
	m_iN=iN;
	m_dBW=dBW;
 	m_dalpha=dalpha;
 	m_doffset=0.0;
	m_SO = SO;
   };

  ~SincRfPulseShape(){};

//! perform amplitude calculation from flip angle
  bool Prepare(bool verbose){
	double t0=1.0/m_dBW, Sum = 0.0, DeltaT = getDuration()/10000.0 ;
	for (double t=-m_iN*t0; t<=m_iN*t0; t+=DeltaT)
	{
		double sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		Sum += (1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
	}
	m_dAmplitude = getFlipAngle()*PI/(DeltaT*Sum*180.0);
	setNLPs();
 	return true;
  };

 //! has to move to RfPulseShape.h !!!
 double getBandWidth(){return m_dBW;}

//! set the number of NLPs to eight times the number of zeros
 void setNLPs() {
  	m_iNumNLPs=8*m_iN+1;
	double t0=1.0/m_dBW;
	for (int i=0;i<4*m_iN;i++)
	{
		m_dArrayOfNLPs[i]=0.25*i*t0;
		m_dArrayOfNLPs[i+4*m_iN+1]=0.25*(i+4*m_iN+1)*t0;
	}
	m_dArrayOfNLPs[4*m_iN]=m_iN*t0;
 };
 
//! set the modulation freuqency for shifting a slice
/* !
 *  This function:
 *   1) gets information on slice thickness from SS_TGPS in the same Atom
 *   2) sets slice offset according to the loop counter and 
 */
 void setFrequencyModulation(int const iLoop) {
 };
 
  void getValue(double * dAllVal, double const time,int const iLoop){
	if (m_iTreeSteps != 0) setFrequencyModulation(iLoop);
	double dT=getDuration();
	if ( time >= 0 && time <= dT )
	{
		double t0=1.0/m_dBW;
		double t=time-m_iN*t0;
		double sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		double dVal = m_dAmplitude*(1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
		dAllVal[0] += fabs(dVal);
		dAllVal[1] += ( (dVal<0.0?PI:0.0) + getPhase()*PI/180.0 + m_doffset*t );
	}
  };

private:
 //! maximum amplitude of the sinc
 double m_dAmplitude;
 //! apodization factor (default 0.5 = Hamming window)
 double m_dalpha;
 //! bandwidth of the pulse
 double m_dBW;
 //! half the number of zero crossings of the pulse
 int m_iN;
 //! frequency offset ([kHz]) for slice selection
 double m_doffset;
 //! slice order acquisition scheme
 SLICE_ORDER m_SO;

};

#endif


