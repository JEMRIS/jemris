/** @file GaussianRFPulse.cpp
 *  @brief Implementation of JEMRIS GaussianRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
 *                                  
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

#include "GaussianRFPulse.h"

/***********************************************************/
bool GaussianRFPulse::Prepare  (PrepareMode mode) {

    m_max_amplitude = 1;
    m_max_amplitude = (PI/180.0) * GetFlipAngle() / GetIntegralNumeric(10000) ;

    bool b = RFPulse::Prepare(mode);

    return b;
}

/*****************************************************************/
double    GaussianRFPulse::GetMagnitude  (double time ){

   double t0 = 1.0/m_bw;
   double t  = time-GetDuration()/2;
   return ( m_max_amplitude*exp(-pow(t,2)/(2*PI*pow(t0,2)) ) ) ;

}

/*****************************************************************/
inline void  GaussianRFPulse::SetTPOIs () {

	//Reset and take care for ADCs
	RFPulse::SetTPOIs();

	//Add equidistant TPOIs
	//TODO: choose number of points dynamically (based on BW and duration)
	//      to ensure accurate shape but save computation where possible
	const unsigned int nPoints = 50;
	for (unsigned int i = 1; i < nPoints; i++)
		m_tpoi + TPOI::set(i*GetDuration()/nPoints, -1.0);
}


