/** @file ConstantGradPulse.cpp
 *  @brief Implementation of JEMRIS ConstantGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
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

#include "ConstantGradPulse.h"

/***********************************************************/
ConstantGradPulse::ConstantGradPulse  (const ConstantGradPulse&) {

	m_amplitude = 0.0;
}

/***********************************************************/
bool ConstantGradPulse::Prepare  (const PrepareMode mode) {

	bool btag = true;

	ATTRIBUTE("Amplitude"    , m_amplitude       );

	btag = ( GradPulse::Prepare(mode) && btag );

	SetArea( m_amplitude*GetDuration() );

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;

    if (mode != PREP_UPDATE) {
		HideAttribute("Area"           );
		HideAttribute("MaxAmpl",  false);
		HideAttribute("SlewRate", false);
	}

	return btag;

}

/***********************************************************/
inline double ConstantGradPulse::GetGradient (double const time) {

	return m_amplitude;

}

/***********************************************************/
string          ConstantGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , Amplitude = " << m_amplitude ;
	return s.str();

}
