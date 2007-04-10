/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SPHERE2D_3SUB_H_
#define _SPHERE2D_3SUB_H_

#include "Sample.h"

class Sphere2D_3sub  :public Sample{
public:
  //construct
  Sphere2D_3sub(double dDeltX ,double dDeltY, double dRadius,
		double dM0_1=0.6,double dT1_1=50,double dT2_1=9,
		double dM0_2=0.8,double dT1_2=100,double dT2_2=10,
		double dM0_3=1.0,double dT1_3=300,double dT2_3=200)
  {
    m_dDeltX =  dDeltX;
    m_dDeltY =  dDeltY;
    m_dRadius = dRadius;
    m_dM0[0] = dM0_1; m_dM0[1] = dM0_2; m_dM0[2] = dM0_3;
    m_dT1[0] = dT1_1; m_dT1[1] = dT1_2; m_dT1[2] = dT1_3;
    m_dT2[0] = dT2_1; m_dT2[1] = dT2_2; m_dT2[2] = dT2_3;
    double X,Y;
    long N=0;
    int i;
    for ( X = -m_dRadius; X <= m_dRadius; X += m_dDeltX )
    	for ( Y = -m_dRadius; Y <= m_dRadius;  Y += m_dDeltY )
               if ( (X*X+Y*Y) <= (m_dRadius*m_dRadius) )
		{
			if (X<0) {i=0;} else {if (Y<0) {i=1;} else {i=2;} }; 
               		sSample.Xcoord[N] = X;
                        sSample.Ycoord[N]= Y;
                        sSample.M0[N] = m_dM0[i];
                        sSample.RT1[N] = m_dT1[i];
                        sSample.RT2[N] = m_dT2[i];
                        N++;
		};
 sSample.NumberOfPoints = N;
 };

   ~Sphere2D_3sub(){};

    double getDeltX(){return m_dDeltX;};
    double getDeltY(){return m_dDeltY;};
private:
       double m_dDeltX;
       double m_dDeltY;
       double m_dRadius;
       double m_dM0[3];
       double m_dT1[3];
       double m_dT2[3];
};//end Class  Sphere2D

#endif
