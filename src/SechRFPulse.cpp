/** @file SechRFPulse.cpp
 *  @brief Implementation of JEMRIS SechRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#include "SechRFPulse.h"

SechRFPulse::SechRFPulse  (const SechRFPulse&) {
	   m_flip_angle = 210.0; // siemens ...
	   m_duration   = 5.12;  // ... Values (?)
}

/***********************************************************/
bool SechRFPulse::Prepare  (PrepareMode mode) {

    m_max_amplitude = 1.0;
    m_sech_phase    = 0.0;
    m_max_amplitude = (PI/180.0) * GetFlipAngle() / GetIntegralNumeric(10000) ;

    if (mode != PREP_UPDATE)
        insertGetPhaseFunction( &SechRFPulse::GetSechPhase );

    bool b = RFPulse::Prepare(mode);
	if (mode != PREP_UPDATE) {
	    HideAttribute("Bandwidth");
	    HideAttribute("Frequency");
	}

    return b;
}

/*****************************************************************/
double    SechRFPulse::GetMagnitude  (double time ){

   double t  = time-GetDuration()/2;
   double fr=(1/cosh(t))*cos(4*log(1/cosh(t)));
   double fi=(1/cosh(t))*sin(4*log(1/cosh(t)));
   m_sech_phase = 27.0+atan2(fi,fr)*180.0/PI;
   return ( m_max_amplitude*sqrt(pow(fr,2)+pow(fi,2)) ) ;

}

/*****************************************************************/
inline void  SechRFPulse::SetTPOIs () {

	//Reset and take care for ADCs
	Pulse::SetTPOIs();

	//add equidistantly 10 TPOIs
	for (unsigned int i = 1; i < 20; i++)
		m_tpoi + TPOI::set(i*GetDuration()/20, -1.0);

}


