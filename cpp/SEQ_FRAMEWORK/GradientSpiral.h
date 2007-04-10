/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _GRADIENTSPIRAL_H_
#define _GRADIENTSPIRAL_H_

#include "PulseShape.h"

//a very simple spiral: cosine in case of GX and sine in case of GY
//does not care about scanner limits!
class GradientSpiral : public GradientPulseShape{

public:
   GradientSpiral (double ddur, double dturns, double dtune, double dres=-1.0,PulseAxis eAxis=AXIS_GX, string sName="GradientSpiral" ) {
	setDuration(ddur);
	m_dturns = dturns;
	m_dtune  = dtune; //spiral tuning parameter (see getValue() , or Boernert et al. MAGMA 1999;9:29-41.)
	m_dres   = dres; //if resolution < 0, it will be taken from the parameters in Prepare()
	setName(sName);
 	setAxis(eAxis);
	Prepare(false);
   };

  ~GradientSpiral (){};

  bool Prepare(bool verbose){
	if (m_dres<0.0 && getAtomicSeq()!=NULL)
	{
		if (getAxis() == AXIS_GX) m_dres = getAtomicSeq()->getRoot()->getParameter()->getDx();
		if (getAxis() == AXIS_GY) m_dres = getAtomicSeq()->getRoot()->getParameter()->getDy();
	}
	setArea(m_dres);
	setNLPs();
	return true;
//cout << "Spiral:  " << getDuration() << " ," <<m_dturns << " ," <<m_dtune << " ," <<m_dres << " ," << getArea() << endl;
  };

  void setNLPs(){
	m_iNumNLPs=((long) (20*m_dturns));
	for (int i=0;i<m_iNumNLPs;i++) { m_dArrayOfNLPs[i] = (i*getDuration())/(m_iNumNLPs-1); }
  };


  void setArea(double value){ //change the resolution-encoding of the spiral !
	m_dres=value; 
	double T = 1.0/sqrt(m_dtune + (1.0-m_dtune)) ;
	if (getAxis() == AXIS_GX)
		m_dArea = (PI/m_dres)*T*cos(2.0*PI*T*m_dturns);
	if (getAxis() == AXIS_GY)
		m_dArea = (PI/m_dres)*T*sin(2.0*PI*T*m_dturns);
  };

  void getValue(double* dAllVal, double const time,int const iLoopCounter){

	if ( time < 0.0 || time > getDuration() ) return;

	double t = time/getDuration();
	if (getAxis() == AXIS_GX)
		dAllVal[2] += (PI/(m_dres*getDuration())) * ( 1/(sqrt(m_dtune+t-m_dtune*t))*cos(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)-t/sqrt(pow(m_dtune+t-m_dtune*t,3.0))*cos(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)*(1.0-m_dtune)/2.0-t/sqrt(m_dtune+t-m_dtune*t)*sin(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)*(2.0*PI/sqrt(m_dtune+t-m_dtune*t)*m_dturns-PI*t/sqrt(pow(m_dtune+t-m_dtune*t,3.0))*m_dturns*(1.0-m_dtune)) );

	if (getAxis() == AXIS_GY)
		dAllVal[3] += (PI/(m_dres*getDuration())) * ( 1/(sqrt(m_dtune+t-m_dtune*t))*sin(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)-t/sqrt(pow(m_dtune+t-m_dtune*t,3.0))*sin(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)*(1.0-m_dtune)/2.0+t/sqrt(m_dtune+t-m_dtune*t)*cos(2.0*PI*t/sqrt(m_dtune+t-m_dtune*t)*m_dturns)*(2.0*PI/sqrt(m_dtune+t-m_dtune*t)*m_dturns-PI*t/sqrt(pow(m_dtune+t-m_dtune*t,3.0))*m_dturns*(1.0-m_dtune)) );
  };

private:
	string m_fname;
	double m_dturns;
	double m_dtune;
	double m_dres;
};

#endif


