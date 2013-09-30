/** @file TriangleGradPulse.cpp
 *  @brief Implementation of TriangleGradPulse
 *
 * Author: tstoecker
 * Date  : Apr 6, 2009
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


#include "TriangleGradPulse.h"

/***********************************************************/
TriangleGradPulse::TriangleGradPulse  (const TriangleGradPulse&) {

	m_amplitude		= 0.0;
	m_triangle_type	= TRIANGLE_UP;
}

/***********************************************************/
bool TriangleGradPulse::Prepare  (const PrepareMode mode) {

	bool btag = true;

	ATTRIBUTE("Amplitude", m_amplitude );

	UNOBSERVABLE_ATTRIBUTE("TriangleType");

	if ( mode != PREP_UPDATE) {
		if (!HasDOMattribute("Duration") ) {
			m_duration  = ( fabs(m_amplitude)>0.0 ? fabs(m_amplitude)/m_slew_rate : 1e-5 );
		} else {
			if (m_duration  < (m_amplitude/m_slew_rate) && mode == PREP_VERBOSE)
				cout << "Warning in in TRIANGLEGRADPULSE " << GetName()
					 << "duration too short (slew-rate conflict)" << endl;
		}
	}

	SetArea( 0.5*m_amplitude*GetDuration() );

	btag = ( GradPulse::Prepare(mode) && btag );

	if ( mode != PREP_UPDATE) {
		if (HasDOMattribute("TriangleType") ) {
			string type = GetDOMattribute("TriangleType");
			if (type == "UP") m_triangle_type = TRIANGLE_UP;
			if (type == "DN") m_triangle_type = TRIANGLE_DN;
		} else {
			AddDOMattribute("TriangleType","UP");
		}
	}

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
inline double TriangleGradPulse::GetGradient (double const time) {

	if (m_triangle_type == TRIANGLE_UP) {
		return ( m_amplitude * time / GetDuration() );
	}

	if (m_triangle_type == TRIANGLE_DN) {
		return ( m_amplitude * ( 1.0 - time / GetDuration() ) );
	}

	return 0.0;

}

/***********************************************************/
string          TriangleGradPulse::GetInfo() {

	stringstream s;
	string type = "";
	if (m_triangle_type == TRIANGLE_UP) type = "UP";
	if (m_triangle_type == TRIANGLE_DN) type = "DN";
	s << GradPulse::GetInfo() << " , Amplitude = " << m_amplitude << " , Type = " << type ;
	return s.str();

}
