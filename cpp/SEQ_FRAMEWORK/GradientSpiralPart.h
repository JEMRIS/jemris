/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _GRADIENTSPIRALPART_H_
#define _GRADIENTSPIRALPART_H_

#include "PulseShape.h"

class GradientSpiralPart : public GradientPulseShape{

public:
   GradientSpiralPart (int iparts, int isegm, double ddur, double dturns, double dtune, double dres=-1.0,PulseAxis eAxis=AXIS_GX, string sName="GradientSpiralPart" ) {
	m_iparts = iparts;
	m_dturns = dturns;
	m_isegment = isegm;
	setDuration(ddur/iparts);
	setName(sName);
 	setAxis(eAxis);
	m_gspir = new GradientSpiral(ddur, dturns, dtune, dres, eAxis, "_GSPIR_");
	Prepare(false);
   };

  ~GradientSpiralPart (){ delete m_gspir; };

  bool Prepare(bool verbose){ setNLPs(); setArea(52); return true; };

  void setArea(double value){ m_dArea = m_gspir->getArea(); };
  GradientSpiral* getGS(){return m_gspir;};

  void setNLPs(){
	m_iNumNLPs=((long) (20*m_dturns/m_iparts));
	for (int i=0;i<m_iNumNLPs;i++) { m_dArrayOfNLPs[i] = (i*getDuration())/(m_iNumNLPs-1); }
  };

  void getValue(double* dAllVal, double const time,int const iLoopCounter){

	if ( time < 0.0 || time > getDuration() ) return;
	m_gspir->getValue(dAllVal,time + (m_isegment-1)*getDuration(),iLoopCounter);
  };

private:
	int m_iparts, m_isegment;
	double m_dturns;
	string m_fname;
	GradientSpiral* m_gspir;
};

#endif


