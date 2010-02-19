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

	// distance to sample point
	double dist  = sqrt(pow(position[XC]-m_position[XC],2)
				 +      pow(position[YC]-m_position[YC],2)
				 +      pow(position[ZC]-m_position[ZC],2));

	// scalar product of negative position vector and sample point
	double scapr = position[XC]*(-m_position[XC])
				 + position[YC]*(-m_position[YC])
				 + position[ZC]*(-m_position[ZC]);

	// absolute values of the vectors
	double abspt = sqrt (pow(  position[XC],2) + pow(  position[YC],2) + pow(  position[ZC],2));
	double absrd = sqrt (pow(m_position[XC],2) + pow(m_position[YC],2) + pow(m_position[ZC],2));

	// angle off axis
	double angle = acos (scapr/(abspt*absrd));

	// distance off axis
	double r     = dist * sin (angle);
	// distance on axis
	double x     = dist * cos (angle);

	double alpha = r/a;
	double beta  = x/a;
	double gamma = x/r;
	double Q     = pow  ((1.0+alpha),2) + pow (beta,2);

	double k     = sqrt(4*alpha/Q);
	k            = (isnan(k)?0.0:k);

	double Kk    = 1.0;
	double Ek    = 1.0;

	#ifdef HAVE_BOOST
	// Complete elliptical integrals
	Kk = boost::math::ellint_1(k);
	Ek = boost::math::ellint_2(k);
	#endif

	double Bx    = (Ek * (1.0 - pow(alpha,2) - pow(beta,2)) / (Q-4.0*a) + Kk)         / (PI * sqrt(Q));
	double Br    = (Ek * (1.0 + pow(alpha,2) + pow(beta,2)) / (Q-4.0*a) - Kk) * gamma / (2 * pow(PI,2) * sqrt(Q)) ;

	// TODO: Only xy component is needed!
	double B1    = sqrt(pow(Bx,2)+pow(Br,2));

	B1 = (isnan(B1)? 0.5:B1);
	B1 = (dist > m_radius*6.28? 0.0: B1);
	return B1;

}
