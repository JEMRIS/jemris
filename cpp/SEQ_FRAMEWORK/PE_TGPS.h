/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _PE_TGPS_H_
#define _PE_TGPS_H_

#define MAXPESTEPS 4096

#include "TGPS.h"

enum PE_ORDER {LINEAR_UP, LINEAR_DN, CENTRIC_OUT, CENTRIC_IN};

class PE_TGPS : public TGPS{
public:

   //create with area, number of phase encode steps, ordering method, and name
   PE_TGPS(double dArea=0.0, int iPEsteps=0, PE_ORDER order=LINEAR_UP,
	   PulseAxis eAxis=AXIS_GY, string sName="PE_TGPS" ) {
 	setName(sName);
	setAxis(eAxis);
	m_order=order;
	m_dUserDuration=-1.0;
	Initialize(dArea,iPEsteps);
	m_bVerbose=false;
	Prepare(m_bVerbose);
   };

   ~PE_TGPS(){};

 void Initialize(double dArea, int iPEsteps){
	setArea(dArea);
  	m_iPEsteps=iPEsteps;
 };

  bool Prepare(bool verbose){
	m_bVerbose=verbose;
	setLimits();
	if ( getAreaMethod()==1 ) //along X axis
	{
		m_dArea = getFactor()*getAtomicSeq()->getRoot()->getParameter()->getKMAXx();
		m_iPEsteps = getAtomicSeq()->getRoot()->getParameter()->getNx();
	}
	if ( getAreaMethod()==2 ) //along Y axis
	{
		m_dArea = getFactor()*getAtomicSeq()->getRoot()->getParameter()->getKMAXy();
		m_iPEsteps = getAtomicSeq()->getRoot()->getParameter()->getNy();
	}
	if ( getAreaMethod()==3 ) //along Z axis
	{
		m_dArea = getFactor()*getAtomicSeq()->getRoot()->getParameter()->getKMAXz();
		m_iPEsteps = getAtomicSeq()->getRoot()->getParameter()->getNz();
	}
	setPEtable(getArea(), m_iPEsteps, m_order);
	
	m_iLastLoopCounter=0;
	//set repetitions of the looping sequence
	Sequence* sSeq=getAtomicSeq();
	for (int j=0;j< getTreeSteps(); ++j) {sSeq = sSeq->getParent();}
	if (sSeq !=NULL && sSeq != getAtomicSeq() ) ((ConcatSequence*)sSeq)->setRepetitions(m_iPEsteps);
	return true;
  };

   void setPEtable(double dArea, int iPEsteps, PE_ORDER order){

	for (int i=0;i<iPEsteps; i++)
	{
		switch (order)
		{
		  case LINEAR_UP  : m_dPEtable[i] = 2*i*dArea/iPEsteps - dArea ;		break;
		  case LINEAR_DN  : m_dPEtable[i] = dArea - 2*i*dArea/iPEsteps;			break;
		  case CENTRIC_OUT: m_dPEtable[i] = pow(-1.0,i+1)*i*dArea/(iPEsteps-1);		break;
		  case CENTRIC_IN : m_dPEtable[i] = pow(-1.0,i+1)*(iPEsteps-1-i)*dArea/(iPEsteps-1);	break;
		  default: cout << "?? unkown PE_ORDER method ??" << endl; return;
		}
	}
	//initialise first PE step
	setArea(dArea);
	if (m_dUserDuration<0)
		m_dTotalDuration=getDuration();
	else
		m_dTotalDuration=m_dUserDuration;
	setArea(m_dPEtable[0]);
	setDuration(m_dTotalDuration);
   };

   void NewDuration(double val){m_dUserDuration=val;};
   //this function is called from TGPS::getValue()
   void CheckLoop(const int iLoopCounter){
	if (iLoopCounter == m_iLastLoopCounter) return;
	m_iLastLoopCounter = iLoopCounter;
	setArea(m_dPEtable[iLoopCounter]);
	setDuration(m_dTotalDuration);
	if (getAtomicSeq()!=NULL)  { getAtomicSeq()->setListOfTimepoints(true); }
   };

private:
   double	m_dPEtable[MAXPESTEPS];
   double	m_dTotalDuration;
   double	m_dUserDuration;
   int		m_iLastLoopCounter;
   int		m_iPEsteps;
   PE_ORDER	m_order;
};

#endif
