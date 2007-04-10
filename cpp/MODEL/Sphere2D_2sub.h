/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SPHERE2D_2SUB_H_
#define _SPHERE2D_2SUB_H_

#include "Sample.h"

class Sphere2D_2sub  :public Sample{
public:
  //construct
  Sphere2D_2sub(double dDeltX ,double dDeltY,
		double dRadius_1=100, double dM0_1=0.6,double dT1_1=50,double dT2_1=9, double dCS_1=0,
		double dRadius_2=50, double dM0_2=0.8,double dT1_2=100,double dT2_2=10, double dCS_2=0)
  {
    m_dDeltX =  dDeltX;
    m_dDeltY =  dDeltY;
    m_dRadius = fmax(dRadius_1,dRadius_2);
    m_dM0[0] = dM0_1; m_dM0[1] = dM0_2;
    m_dT1[0] = dT1_1; m_dT1[1] = dT1_2;
    m_dT2[0] = dT2_1; m_dT2[1] = dT2_2;
    m_dCS[0] = dCS_1; m_dCS[1] = dCS_2;
    double X,Y;
    long N=0;
    int i;
    for ( X = -m_dRadius; X <= m_dRadius; X += m_dDeltX )
    	for ( Y = -m_dRadius; Y <= m_dRadius;  Y += m_dDeltY )
               if ( (X*X+Y*Y) <= (m_dRadius*m_dRadius) )
		{
			if (dRadius_1<dRadius_2)
				{ if ( (X*X+Y*Y) <= (dRadius_1*dRadius_1)) i=0; else i=1; }
			else
				{ if ( (X*X+Y*Y) <= (dRadius_2*dRadius_2)) i=1; else i=0; }
               		sSample.Xcoord[N] = X;
                        sSample.Ycoord[N]= Y;
                        sSample.M0[N] = m_dM0[i];
                        sSample.RT1[N] = m_dT1[i];
                        sSample.RT2[N] = m_dT2[i];
                        sSample.CS[N] = m_dCS[i];
                        N++;
		};
 sSample.NumberOfPoints = N;
 };

   ~Sphere2D_2sub(){};

    double getDeltX(){return m_dDeltX;};
    double getDeltY(){return m_dDeltY;};
private:
       double m_dDeltX;
       double m_dDeltY;
       double m_dRadius;
       double m_dM0[2];
       double m_dT1[2];
       double m_dT2[2];
       double m_dCS[2];
};//end Class  Sphere2D

#endif
