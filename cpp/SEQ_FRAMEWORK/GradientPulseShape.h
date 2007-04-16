/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _GRADIENTPLUSESHAPE_H_
#define _GRADIENTPLUSESHAPE_H_ 

#include "PulseShape.h"
#include "Scanner.h"

class GradientPulseShape :public PulseShape{

public:
  GradientPulseShape (){
	setName("GradientPulseShape");
	setAxis(AXIS_GX); //if not specified, gradients at x-axis by default
	m_dFactor=0.0;
	m_getAreaMethod=0;
	m_sGradPulse="";
	m_dNewDuration=0.0;
	m_bSlewRate=true;
	m_bMaxAmpl=true;
	setLimits();
  };
  ~GradientPulseShape(){};

  virtual void getValue(double * dAllVal, double const time,int const iLoop)=0;
  virtual void setArea (double value )=0;

  //overload to prevent choosing RF axis
  void setAxis(PulseAxis eAxis) {
	if (eAxis==AXIS_RF)
	{
                cout << "warning: RF is not a valid axis for GradientPulseShape: " << getName() ;
                if (getAtomicSeq()!=NULL)
                     cout << " in sequence atom " << getAtomicSeq()->getName();
                cout << endl;
		return;
	}
	m_eAxis=eAxis;
  };

  void setLimits(){
	//set limits from scanner, if limits have not been locally changed
	if (m_bSlewRate) m_dSlewRate = m_scanner.getMaxSlewRate();
	if (m_bMaxAmpl)  m_dMaxAmpl  = m_scanner.getMaxGradientPower();
	//set limits from parameters, if available and limits have not been locally changed
	double dMG=-1.0 , dSR=-1.0;
	if (m_bMaxAmpl && getAtomicSeq()!=NULL) dMG=getAtomicSeq()->getRoot()->getParameter()->getMaxAmpl();
	if (dMG>0.0) m_dMaxAmpl=dMG;
	if (m_bSlewRate && getAtomicSeq()!=NULL) dSR=getAtomicSeq()->getRoot()->getParameter()->getSlewRate();
	if (dSR>0.0) m_dSlewRate=dSR;
  };

//setting limits, ignoring the scanner limits
void setSlewRate ( double val ){m_dSlewRate=val; m_bSlewRate=false; };
void setMaxAmpl  ( double val ){m_dMaxAmpl =val; m_bMaxAmpl=false; };

double getSlewRate ( ){return m_dSlewRate;}; 
double getMaxAmpl ( ){return m_dMaxAmpl;}; 
double getArea ( ){return m_dArea;}; 

//functions to set up gradient shapes during Prepare()
void setFactor(double val){m_dFactor=val;};
double getFactor(){return m_dFactor;};

void getAreaMethod(int val){m_getAreaMethod=val;};
int getAreaMethod(){return m_getAreaMethod;};

void LinkToPulse(string val){m_sGradPulse=val;};
string LinkToPulse(){return m_sGradPulse;};

void  NewDuration(double val){m_dNewDuration=val;};
double  NewDuration(){return m_dNewDuration;};
protected:
	Scanner m_scanner;
	double 	m_dSlewRate;
	double 	m_dMaxAmpl;
	bool 	m_bSlewRate;
	bool 	m_bMaxAmpl;
	double 	m_dArea;
	double 	m_dFactor;
	int	m_getAreaMethod;
	string	m_sGradPulse;
	double  m_dNewDuration;
};//end of Class

#endif


