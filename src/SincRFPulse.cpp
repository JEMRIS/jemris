/** @file SincRFPulse.cpp
 *  @brief Implementation of JEMRIS SincRFPulse
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

#include "SincRFPulse.h"

/*****************************************************************/
SincRFPulse::SincRFPulse  (const SincRFPulse& hrfp) {

   // Default values for the sinc
   m_bw         = 10.0;
   m_flip_angle = 90.0;
   m_zeros      = 3.0;
   m_alpha      = 0.5;

};

/*****************************************************************/
bool    SincRFPulse::Prepare  (PrepareMode mode) {

    //attributes of the sinc
    ATTRIBUTE("Zeros"       , m_zeros);
    ATTRIBUTE("Apodization" , m_alpha);

    //the duration of this sinc
    SetDuration((2.0*m_zeros)/m_bw);

    //Calculate maximum amplitude from flipangle and duration:
    //numerically integrates pulse shape over 10000 sampling points
    m_max_amplitude = 1;
    m_max_amplitude = (PI/180.0) * GetFlipAngle() / GetIntegralNumeric(10000);

    bool tag = RFPulse::Prepare(mode);

    //the duration is given by the bandwidth, not directly from XML !!
    if (mode != PREP_UPDATE) HideAttribute("Duration");

    return tag;
};

/*****************************************************************/
double    SincRFPulse::GetMagnitude  (double time ){

   double t0    = 1.0  / m_bw;
   double t     = time - m_zeros * t0;
   double sinct = (t==0.0 ? 1.0 : sin(PI*t/t0)/(PI*t/t0));

   return ( m_max_amplitude*(1.0-m_alpha+m_alpha*cos(PI*t/(m_zeros*t0)))*sinct ) ;

};

/*****************************************************************/
inline void  SincRFPulse::SetTPOIs () {

	//Reset and take care for ADCs
	Pulse::SetTPOIs();

	//add equidistantly TPOIs at 8 times the numer of zeros
	for (unsigned int i = 1; i < 8*m_zeros; i++)
		m_tpoi + TPOI::set(i*GetDuration()/(8*m_zeros), -1.0);

}
/***********************************************************/
string          SincRFPulse::GetInfo() {

	stringstream s;
	s << RFPulse::GetInfo() << " , (Zero-crossings,Apodization) = (" << m_zeros << "," << m_alpha << ")";

	return s.str();

};
