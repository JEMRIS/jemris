/** @file BiotSavartLoop.cpp
 *  @brief Implementation of JEMRIS BiotSavartLoop
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#include "BiotSavartLoop.h"

#ifdef HAVE_BOOST
    #include <boost/math/special_functions/ellint_2.hpp>
    #include <boost/math/special_functions/ellint_1.hpp>
#endif

bool BiotSavartLoop::Prepare (PrepareMode mode) {

	bool success = true;

    ATTRIBUTE("Radius" , &m_radius);
    success   = Coil::Prepare(mode);
	m_maxsens = 0.0;
	m_havemax = false;
	DumpSensMap("");
	m_havemax = true;

	return success;

}

double BiotSavartLoop::GetSensitivity(double* position) {

    if (m_havemax) {
		if (round(position[ZC]/m_space_res[ZC])+m_matrx_res[ZC]/2 > 0 && round(position[ZC]/m_space_res[ZC])+m_matrx_res[ZC]/2 < m_matrx_res[ZC]) {
			return m_sens_map [(int)round(position[ZC]/m_space_res[ZC])+m_matrx_res[ZC]/2]
							  [(int)round(position[YC]/m_space_res[YC])+m_matrx_res[YC]/2]
							  [(int)round(position[XC]/m_space_res[XC])+m_matrx_res[XC]/2]/m_maxsens;
		} else return 0.0;
    } else {

        double a     = m_radius;

        // distance to sample point
        double dist  = sqrt(pow(position[XC]-m_position[XC],2)
                     +      pow(position[YC]-m_position[YC],2)
                     +      pow(position[ZC]-m_position[ZC],2));

        double q = 5.0;

        if (abs(position[XC]) < m_radius/q)
            position[XC] = ((position[XC]<0) ? -m_radius:m_radius)/q;

        if (abs(position[YC]) < m_radius/q)
            position[YC] = ((position[YC]<0) ? -m_radius:m_radius)/q;

        if (abs(position[ZC]) < m_radius/q)
            position[ZC] = ((position[ZC]<0) ? -m_radius:m_radius)/q;


        // Calculate off axis angle

        // scalar product of negative position vector and sample point
        double scapr = position[XC]*m_direction[XC]
                     + position[YC]*m_direction[YC]
                     + position[ZC]*m_direction[ZC];

        // absolute values of the vetors
        double abspt = sqrt (pow(  position[XC],2) + pow(  position[YC],2) + pow(  position[ZC],2));
        double absrd = sqrt (pow(m_position[XC],2) + pow(m_position[YC],2) + pow(m_position[ZC],2));

        // angle off axis
        double angle = acos (scapr/(abspt*absrd));

        // distance off axis
        double r     = dist * sin(angle);
        // distance on axis
        double x     = dist * cos(angle);
        //cout << "r: " << r << " x:" << x << endl;

        double alpha = r/a;
        double beta  = x/a;
        double gamma = x/r;
        double Q     = pow  ((1.0+alpha),2) + pow (beta,2);
        double k     = 1/pow(sin(sqrt(4.0*alpha/Q)),2);
        if (k >= 1.0) k = 0.99;

        double Kk    = 0.0;
		double Ek    = 0.0;
		
		#ifdef HAVE_BOOST
            // Complete elliptical integral function of first  kind
		    Kk = boost::math::ellint_1(k, boost::math::policies::ignore_error);
            // Complete elliptical integral function of second kind
            Ek = boost::math::ellint_2(k,  boost::math::policies::ignore_error);
		#endif

        double Bx    = (Ek * (1.0 - pow(alpha,2) - pow(beta,2)) / (Q-4.0*a) + Kk)         / (PI * sqrt(Q));
        double Br    = (Ek * (1.0 + pow(alpha,2) + pow(beta,2)) / (Q-4.0*a) - Kk) * gamma / (PI * sqrt(Q));
        double B1    = sqrt(pow(Bx,2)+pow(Br,2))/pow(dist,0.5);

        if (B1 > m_maxsens)
            m_maxsens = B1;
        return B1;
	}


}
