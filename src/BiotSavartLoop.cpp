/** @file BiotSavartLoop.cpp
 *  @brief Implementation of JEMRIS BiotSavartLoop
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"

#include "BiotSavartLoop.h"

#ifdef HAVE_BOOST
    #include <boost/math/special_functions/ellint_2.hpp>
    #include <boost/math/special_functions/ellint_1.hpp>
#endif

bool BiotSavartLoop::Prepare (PrepareMode mode) {

	bool success = true;

    ATTRIBUTE("Radius" , m_radius);
    success   = Coil::Prepare(mode);
	DumpSensMap("");

	return success;

}

double BiotSavartLoop::GetSensitivity(double* position) {


       double a     = m_radius;
       double px = position[XC]-m_position[XC];
       double py = position[YC]-m_position[YC];
       double pz = position[ZC]-m_position[ZC];
       //if (pz==0.0) pz = m_extent/m_points;

       //rotate point in solenoid coordinate system and check distance to the loop
       double x1 =   px*(cos(m_polar) - pow(cos(m_azimuth),2)*(cos(m_polar) - 1))
                   - pz*sin(m_azimuth)*sin(m_polar) + py*cos(m_azimuth)*sin(m_azimuth)*(cos(m_polar) - 1);
       double x2 =   py*(cos(m_polar) - pow(sin(m_azimuth),2)*(cos(m_polar) - 1))
                   - pz*cos(m_azimuth)*sin(m_polar) + px*cos(m_azimuth)*sin(m_azimuth)*(cos(m_polar) - 1);
       if ( sqrt(abs(pow(x1,2)+pow(x2,2)-pow(a,2))) < 0.25*a ) return 0.0;

       //distance to solenoid center
       double dist = sqrt( abs(pow(px,2)+pow(py,2)+pow(pz,2)) );
       if ( dist < 0.01*a ) return 0.0;

        // angle between position vector and coil normal vector (given by m_azimuth & m_polar)
        double angle = acos ( ( px*cos(m_azimuth)*sin(m_polar) +
								py*sin(m_azimuth)*sin(m_polar) +
								pz*cos(m_polar)  ) / dist );

        // distance off axis
        double r     = abs(dist * sin(angle));

        // distance on axis
        double x     = abs(dist * cos(angle));
        r = (r>0.1*x?r:0.1*x);

        double alpha = r/a;
        double beta  = x/a;
        double gamma = x/r;
        double Q     = pow  ((1.0+alpha),2) + pow (beta,2);
        double k     = sqrt(4*alpha/Q);

        double Kk    = 1.0;
		double Ek    = 1.0;

		#ifdef HAVE_BOOST
            // Complete elliptical integral function of first  kind
		    Kk = boost::math::ellint_1(k, boost::math::policies::ignore_error);
            // Complete elliptical integral function of second kind
            Ek = boost::math::ellint_2(k,  boost::math::policies::ignore_error);
		#endif

        double Bx    = (Ek * (1.0 - pow(alpha,2) - pow(beta,2)) / (Q-4.0*a) + Kk)         / (PI * sqrt(Q));
        double Br    = (Ek * (1.0 + pow(alpha,2) + pow(beta,2)) / (Q-4.0*a) - Kk) * gamma / (PI * sqrt(Q));
        double B1    = sqrt(pow(Bx,2)+pow(Br,2))/pow(dist,0.5);

        //if (B1>1000/m_norm) return 0.0;
/*
        if (GetName()=="C1") cout <<  "(" << setw(4) << position[XC] << "," << setw(4) << position[YC]
                                  << "," << setw(4) << position[ZC] << ") || angle = "
                                  << angle*180.0/PI << " : Kk=" << Kk << " : Ek=" << Ek
                                  << " : r=" << r << " : x=" << x << " : B1=" << B1 << endl;
*/
        return B1;

}
