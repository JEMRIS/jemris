/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _EXTERNALPULSESHAPE_H_
#define _EXTERNALPULSESHAPE_H_

#include "PulseShape.h"

class ExternalPulseShape : public PulseShape{

public:
   ExternalPulseShape (string filename,PulseAxis eAxis=AXIS_RF, double scale=1.0, string sName="ExternalPulseShape" ) {
	m_fname = filename;
	m_dScale = scale;
	setName(sName);
 	setAxis(eAxis);
	ReadPulseShape();
   };

  ~ExternalPulseShape (){};

  bool ReadPulseShape(){
	ifstream fin(m_fname.c_str(), ios::binary);
	double dNumOfTPs;
	fin.read((char *)(&(dNumOfTPs)), sizeof(double));
	m_iNumberOfTimePoints = ((int) dNumOfTPs);
	//cout << "  : Nt = " << m_iNumberOfTimePoints << endl;
	for (int i=0; i<m_iNumberOfTimePoints; ++i)
	{
		fin.read((char *)(&(m_dArrayOfNLPs[i])), sizeof(double));
		fin.read((char *)(&(m_dValArray1[i])), sizeof(double));
		m_dValArray1[i] *= m_dScale;
		if (getAxis() == AXIS_RF)	//an RF pulse shape => read phase as well
			fin.read((char *)(&(m_dValArray2[i])), sizeof(double));
	//cout << " : " << m_dArrayOfNLPs[i] << " : " << m_dValArray1[i] << " : " <<m_dValArray2[i] << endl;
	}
	fin.close();
	setDuration(m_dArrayOfNLPs[m_iNumberOfTimePoints-1]);
	setNLPs();
	//cout << "  : Duration = " << m_dArrayOfNLPs[m_iNumberOfTimePoints-1] << endl;
  };

  void setNLPs() {m_iNumNLPs=m_iNumberOfTimePoints; }; //setting of timepoints in  ReadPulseShape()

  double* getValArray1(){return &m_dValArray1[0];};
  double* getValArray2(){return &m_dValArray2[0];};

  void getValue(double* dAllVal, double const time,int const iLoopCounter){

	if ( time < 0 || time > getDuration() ) return;

	int i; double d2, d1=getDuration();
	for (i=0; i<m_iNumNLPs; ++i)
	{
		d2 = fabs(time-m_dArrayOfNLPs[i]);
		if (d2 > d1) break;
		d1 = d2;
	}

	//cout << " time=" << time << "  , i=" << i << " NLPs=" << m_iNumNLPs << endl;
	if (getAxis() == AXIS_RF)	//an RF pulse shape
	{
		dAllVal[0] += m_dValArray1[i-1];
		dAllVal[1] += m_dValArray2[i-1];
	}
	else				//a gradient pulse shape
		dAllVal[getAxis() + 1] += m_dValArray1[i-1];
  };

private:
	string m_fname;
	int m_iNumberOfTimePoints;
	double m_dScale;
	double m_dValArray1[PULSESHAPE_MAXPOINTS];
	double m_dValArray2[PULSESHAPE_MAXPOINTS];
};

#endif


