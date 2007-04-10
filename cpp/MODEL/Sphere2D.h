/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SPHERE2D_H_
#define _SPHERE2D_H_

#include "Sample.h"

//! A 2D spherical sample
class Sphere2D  :public Sample{
public:
  //! construct a 2D sphere with resolution, radius, and physical properties (M0, T1, T2)
  Sphere2D(double dDeltX ,double dDeltY,double dRadius,double dM0=1,double dT1=10,double dT2=5, double dCS=0)
  {
    long N=0;
    m_dDeltX =  dDeltX;
    m_dDeltY =  dDeltY;
    m_dRadius = dRadius;
    double X,Y;
    //certer of circle is (0,0)
//cout << " SPHERE: " << m_dRadius << " , " << m_dDeltX << " , " << m_dDeltY << endl;
    for ( X = -m_dRadius; X <= m_dRadius; X += m_dDeltX )
    	for ( Y = -m_dRadius; Y <= m_dRadius;  Y += m_dDeltY )
               if ( (X*X+Y*Y) <= (m_dRadius*m_dRadius) )
		{
               		sSample.Xcoord[N] = X;
                        sSample.Ycoord[N]= Y;
                        sSample.M0[N] = dM0;
                        sSample.RT1[N] = dT1;
                        sSample.RT2[N] = dT2;
                        sSample.CS[N] = dCS;
                        N++;
		};
     sSample.NumberOfPoints = N;
    };

   ~Sphere2D(){};

    double getDeltX(){return m_dDeltX;};
    double getDeltY(){return m_dDeltY;};
    double getRadius(){return m_dRadius;};

private:
       double m_dDeltX;
       double m_dDeltY;
       double m_dRadius;

};//end Class  Sphere2D

#endif
