/*
	This file is part of the MR simulation project
	Date: 04/2007
	Author:  T. Stoecker
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SINCRFPULSESHAPE_H_
#define _SINCRFPULSESHAPE_H_

#include "RfPulseShape.h"
#include "AtomicSequence.h"
#include "SS_TGPS.h"

 //! possible slice order acquisition schemes
enum SLICE_ORDER {ASCENDING, DESCENDING, INTERLEAVED};

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
		     double dalpha=0.5, SLICE_ORDER SO=ASCENDING, double dgap=0.0,
		     string sName="SincRfPulseShape" ) {
	setName(sName);
	setFlipAngle(dFlipAngle) ;
	setPhase (dPhase);
	setDuration( (2.0*iN)/dBW );
	m_iN=iN;
	m_dBW=dBW;
	m_dgap=dgap;
 	m_dalpha=dalpha;
	m_SO = SO;
	m_iLastLoopCounter=0; m_doffset=0.0; m_pSS = NULL;
	for (int i=0; i<4096; i++) m_iSlicePosition[i]=0;
   };

  ~SincRfPulseShape(){};

//! perform amplitude calculation from flip angle
  bool Prepare(bool verbose){
	m_iLastLoopCounter=-1; m_doffset=0.0; m_pSS = NULL; m_iNumberOfSlices=1;
	// numerically integrate pulse shape over 10000 sampling points
	double t0=1.0/m_dBW, Sum = 0.0, DeltaT = getDuration()/10000.0 ;
	for (double t=-m_iN*t0; t<=m_iN*t0; t+=DeltaT)
	{
		double sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		Sum += (1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
	}
	m_dAmplitude = getFlipAngle()*PI/(DeltaT*Sum*180.0);
	setNLPs();
	//preparation of phase modulation for slice shifting
	AtomicSequence* pAS = ((AtomicSequence*) getAtomicSeq() );
	if (pAS!=NULL)
	{
		for ( int i=0;i<pAS->getNumberOfPulses() ; i++ )
		switch ( pAS->getPulse(i)->getAxis() )
		{
			case AXIS_GX :	m_iNumberOfSlices = getAtomicSeq()->getRoot()->getParameter()->getNx();
					m_pSS = ((SS_TGPS*) pAS->getPulse(i)); break;
			case AXIS_GY :	m_iNumberOfSlices = getAtomicSeq()->getRoot()->getParameter()->getNy();
					m_pSS = ((SS_TGPS*) pAS->getPulse(i)); break;
			case AXIS_GZ :	m_iNumberOfSlices = getAtomicSeq()->getRoot()->getParameter()->getNz();
					m_pSS = ((SS_TGPS*) pAS->getPulse(i)); break;
			default: continue;
		}
		if (getTreeSteps()==0) m_iNumberOfSlices = 0; //no multiple slices, if not connected to a loop!
		//multislice preparation
		if (m_iNumberOfSlices>1)
		{
			//set the loop counter of the corresponding ConcatSequence !
			Sequence* sSeq=getAtomicSeq();
			for (int j=0;j< getTreeSteps(); ++j) {sSeq = sSeq->getParent();}
			if (sSeq !=NULL && sSeq != getAtomicSeq() )
				((ConcatSequence*)sSeq)->setRepetitions(m_iNumberOfSlices);
			//set the slice position factors for linear ordering (ASCENDING is the default)
			int itmp[4096];
			int iNh = (m_iNumberOfSlices%2==0?m_iNumberOfSlices/2:(m_iNumberOfSlices-1)/2);
			for (int i=0;i<m_iNumberOfSlices; i++)
			{
				int j = ( m_SO==DESCENDING ? m_iNumberOfSlices-i-1 : i );
				m_iSlicePosition[j]=i-iNh;
				itmp[i]=m_iSlicePosition[j]; 
			}
			if (m_SO==INTERLEAVED)
			{
				for (int i=0;i<iNh+m_iNumberOfSlices%2; i++)
					m_iSlicePosition[i]=itmp[2*i];
				for (int i=iNh+m_iNumberOfSlices%2; i<m_iNumberOfSlices; i++)
					m_iSlicePosition[i]=itmp[2*(i-iNh)+1-2*(m_iNumberOfSlices%2)];
			}
			//for (int i=0;i<m_iNumberOfSlices; i++)
				//cout << " !! " << i << " : " << m_iSlicePosition[i] << endl;
		}
	}
 	return true;
  };

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
 
//! set the modulation frequency for shifting a slice
/* !
 *  This function:
 *   1) gets information on amplitude and slice thickness from SS_TGPS in the same Atom
 *   2) sets slice offset according to the loop counter and 
 */
 void setFrequencyModulation(int const iLoop) {
	if (iLoop!= m_iLastLoopCounter && m_iNumberOfSlices>1) 
	{
		m_iLastLoopCounter = iLoop;
		if (m_pSS != NULL)
		{
			double dpos_shift = (1.0+m_dgap/100.0)*m_pSS->getSliceThickness()*m_iSlicePosition[iLoop];
 			m_doffset= dpos_shift * m_pSS->getAmplitude() ;
			//cout	<< "!!! " << m_doffset << " " << m_pSS->getAmplitude() << " "
				//<< m_pSS->getSliceThickness()*iLoop << endl;
		}
	}
 };
 
//! returns values for RF amplitude and phase
  void getValue(double * dAllVal, double const time,int const iLoop){
	setFrequencyModulation(iLoop);
	double dT=getDuration();
	if ( time >= 0 && time <= dT )
	{
		double t0=1.0/m_dBW;
		double t=time-m_iN*t0;
		double sinct = ( t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0) );
		double dVal = m_dAmplitude*(1.0-m_dalpha+m_dalpha*cos(PI*t/(m_iN*t0)))*sinct;
		dAllVal[0] += fabs(dVal);
		dAllVal[1] += fmod( (dVal<0.0?PI:0.0) + getPhase()*PI/180.0 + m_doffset*t , 2*PI);
	}
  };

private:
 //! maximum amplitude of the sinc
 double m_dAmplitude;
 //! apodization factor (default 0.5 = Hamming window)
 double m_dalpha;
 //! half the number of zero crossings of the pulse
 int m_iN;
 //! frequency offset ([kHz]) for slice selection
 double m_doffset;
 //! gap between slices in percent
 double m_dgap;
 //! Number of Slices
 int m_iNumberOfSlices;
 //! Slice position factor
 int m_iSlicePosition[4096];
 //! slice order acquisition scheme
 SLICE_ORDER m_SO;
 //! remember loop counter
 double m_iLastLoopCounter;
 //! pointer to corresonponding slice selection gradient
 SS_TGPS* m_pSS;

};

#endif


