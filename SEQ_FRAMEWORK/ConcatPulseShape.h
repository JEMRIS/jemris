/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _CONCATPULSESHAPE_H_
#define _CONCATPULSESHAPE_H_


#include "PulseShape.h"


class ConcatPulseShape : public PulseShape{
public:

   //create with area, number of phase encode steps, ordering method, and name
   ConcatPulseShape(PulseShape* pPS1=NULL, PulseShape* pPS2=NULL, string sName="ConcatPulseShape" ) {
 	setName(sName);
	m_pPS1=pPS1;
	m_pPS2=pPS2;
	setDuration( m_pPS1->getDuration() + m_pPS2->getDuration() );
	setAllTPs();
   };

   ~ConcatPulseShape(){delete m_pPS1; delete m_pPS2; }; //need to clean up the inserted pulses
  
  bool Prepare(bool verbose){
	bool bret = ( m_pPS1->Prepare(verbose) &&  m_pPS2->Prepare(verbose) ) ;
	setDuration( m_pPS1->getDuration() + m_pPS2->getDuration() );
	return bret;
  };   

   void getValue(double * dAllVal, double const time,int const iLoopCounter) {
	if (iLoopCounter != m_iLastLoopCounter)
	{
		m_iLastLoopCounter = iLoopCounter;
		setAllTPs();
	}

	double dT1=m_pPS1->getDuration();
	if ( time >= 0 && time <= dT1 ) { m_pPS1->getValue(dAllVal,time,iLoopCounter); return; }
	double dT2=m_pPS1->getDuration();
	if ( time >= dT1 && time <= dT1 + dT2 ) { m_pPS2->getValue(dAllVal,time-dT1,iLoopCounter); return; }
   };
   
   void setAllTPs(){
		int iN1, iN2;
		double* pdArray;
		double dT1=m_pPS1->getDuration();
		//ADCs
		iN1 = m_pPS1->getNumOfADCs();
		pdArray = m_pPS1->getADCarray();
		for (int i=0;i<iN1;++i){ m_dArrayOfADCs[i]=pdArray[i]; }
		iN2 = m_pPS2->getNumOfADCs();
		pdArray = m_pPS2->getADCarray();
		for (int i=0;i<iN2;++i){ m_dArrayOfADCs[i+iN1]=pdArray[i]+dT1; }
		m_iNumADCs = iN1+iN2 ;
		//NLPs
		iN1 = m_pPS1->getNumOfNLPs();
		pdArray = m_pPS1->getNLParray();
		for (int i=0;i<iN1;++i){ m_dArrayOfNLPs[i]=pdArray[i]; }
		iN2 = m_pPS2->getNumOfNLPs();
		pdArray = m_pPS2->getNLParray();
		for (int i=0;i<iN2;++i){ m_dArrayOfNLPs[i+iN1]=pdArray[i]+dT1; }
		m_iNumNLPs = iN1+iN2 ;
   };

private:
	PulseShape* m_pPS1;
	PulseShape* m_pPS2;
	int	m_iLastLoopCounter;
};

#endif
