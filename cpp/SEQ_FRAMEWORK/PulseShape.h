/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _PULSESHAPE_H_
#define _PULSESHAPE_H_

#define PI 3.14159265358979
#define PULSESHAPE_MAXPOINTS 32768

#include "Sequence.h"

enum PulseAxis {AXIS_RF,AXIS_GX,AXIS_GY,AXIS_GZ,AXIS_VOID};

/*ABC for all pulse shapes: RF and Gradients*/
class PulseShape 
{
public:

  PulseShape(){
	m_pAtomicSeq=NULL;
	m_sName="PulseShape";
	m_dDuration=0.0;
	m_iNumADCs = 0;
	m_iNumNLPs = 0;
	m_iTreeSteps=0;
	m_eAxis=AXIS_VOID;
  };
  virtual ~PulseShape(){ /*cout << getName() << " deleted" << endl;*/ }; //overloaded in ConcatPulseShape

  virtual void getValue(double * dAllVal, double const time,int const iLoop)=0;

  virtual bool Prepare(bool verbose){return true;}; //is called when the root seq prepares

  virtual void setNLPs(){	//Must be overloaded by non-constant pulse shapes!
	m_iNumNLPs=2;		//Further, the function needs to be called every
	m_dArrayOfNLPs[0]=0.0;	//time the PulseShape changes its timing!!!
	m_dArrayOfNLPs[1]=getDuration();
  };

  PulseAxis  getAxis() { return m_eAxis; };
  virtual void setAxis(PulseAxis eAxis ) { m_eAxis = eAxis; }; //will be overloaded

  double getDuration() { return m_dDuration; };
  virtual void setDuration(double val){
	if (val<0)
        {
		//cout << "warning: ignoring negative duration of PulseShape= " << getName() ;
		//if (getAtomicSeq()!=NULL)
		     //cout << " in AtomicSequence= " << getAtomicSeq()->getName();
		//cout << endl;
		return;
	}
	m_dDuration=val;
	setNLPs();
  };

  int getTreeSteps ( ){return m_iTreeSteps;}; 
  void setTreeSteps (int value ){m_iTreeSteps = value;};
  
  Sequence* getAtomicSeq ( ){return m_pAtomicSeq;}; 
  void setAtomicSeq (Sequence* value ){m_pAtomicSeq = value;};

  int getNumOfADCs() {return m_iNumADCs;};
  double* getADCarray() {return &m_dArrayOfADCs[0]; };

  int getNumOfNLPs() {return m_iNumNLPs;};
  double* getNLParray() {return &m_dArrayOfNLPs[0]; };

  string getName ( ) {return m_sName;};
  void setName (string value ) {m_sName=value;};

  //equidistantly setting ADCs
  void setNumOfADCs(int iNADC){
	//sample points *not* on the edges of the time intervall !
	m_iNumADCs=iNADC;
	double dT=getDuration();
	if (dT>0.0)
		for (int i=0;i<iNADC;i++) { m_dArrayOfADCs[i] = ((i+1)*getDuration())/(iNADC+1); }
	else
	{
		if (getAtomicSeq()!=NULL)
		{
			cout << "warning: nonpositive duration; cannot set ADCs of PulseShape= " << getName();
			cout << " in AtomicSequence= " << getAtomicSeq()->getName();
			cout << endl;
		}
		return;
	}
  };

  //equidistantly setting ADCs within an interval
  void setNumOfADCs(int iNADC, double t1, double t2){
	if (t1>=t2 || t1<0 || t2>getDuration())
        {
		cout << "warning: incorrect ADC interval for PulseShape= " << getName();
		if (getAtomicSeq()!=NULL)
		     cout << " in AtomicSequence= " << getAtomicSeq()->getName();
		cout << endl;
		return;
	}
	m_iNumADCs=iNADC;
	for (int i=0;i<iNADC;i++) { m_dArrayOfADCs[i] = t1 + ((i+1)*(t2-t1))/(iNADC+1); }
  };

protected: //all variables must be accessible from derived pulseshapes
  	PulseAxis  	m_eAxis;
	int		m_iTreeSteps;
	Sequence*	m_pAtomicSeq;
	double		m_dDuration;
	string		m_sName;
	double		m_dArrayOfNLPs[PULSESHAPE_MAXPOINTS];	//every PulseShape needs to have
	int		m_iNumNLPs;				//NLPs (non-linear points of interest)
	double		m_dArrayOfADCs[PULSESHAPE_MAXPOINTS];	//every PulseShape is allowed to
	int		m_iNumADCs;				//have ADCs
};//end Class PulseShape


#endif

