/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SS_TGPS_H_
#define _SS_TGPS_H_

#include "TGPS.h"
#include "AtomicSequence.h"

//! slice selection trapezoidal gradient pulse shape
class SS_TGPS :public TGPS{
public:
 //create with area, flat-top-time, number of ADCs, Axis, and name
 SS_TGPS(PulseAxis eAxis=AXIS_GZ, double slicethickness=1000.0, string sName="SS_TGPS" ) {
 	setName(sName);
	setAxis(eAxis);
	m_dSliceThickness=slicethickness;
	m_bVerbose=false;
	Prepare(m_bVerbose);
 };

 ~SS_TGPS(){};


  bool Prepare(bool verbose){
        m_bVerbose=verbose;
	double duration=0.0, bw=0.0;
	AtomicSequence* pAS = ((AtomicSequence*) getAtomicSeq() );
	if (pAS!=NULL)
	{
		if (m_dSliceThickness<0.0)
		switch ( getAxis() )
		{
			case AXIS_GX : m_dSliceThickness = getAtomicSeq()->getRoot()->getParameter()->getDx(); break;
			case AXIS_GY : m_dSliceThickness = getAtomicSeq()->getRoot()->getParameter()->getDy(); break;
			case AXIS_GZ : m_dSliceThickness = getAtomicSeq()->getRoot()->getParameter()->getDz(); break;
			default: cout << "?? wrong axis ??" << endl; return false;
		}
		for ( int i=0;i<pAS->getNumberOfPulses() ; i++ )
			if ( pAS->getPulse(i)->getAxis() == AXIS_RF )
				{
					duration = pAS->getPulse(i)->getDuration();
					bw = ((SincRfPulseShape*) pAS->getPulse(i))->getBandWidth();
					break;
				}
	}
	setArea(PI*bw*duration/m_dSliceThickness);
	setDuration(duration);
//cout << "SS_TGPS: " << getDuration() << " " << duration << " " << m_dSliceThickness << endl;
	return (fabs(duration-getDuration())<0.0001);
 };

private: 
	double m_dSliceThickness;

};//end of Class

#endif
