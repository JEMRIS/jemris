/** @file ConstantGradPulse.cpp
 *  @brief Implementation of JEMRIS ConstantGradPulse
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

#include "ConstantGradPulse.h"

/***********************************************************/
ConstantGradPulse::ConstantGradPulse  (const ConstantGradPulse&) {

	m_amplitude = 0.0;
};

/***********************************************************/
bool ConstantGradPulse::Prepare  (PrepareMode mode) {

	bool btag = true;

	// Automatically set a GiNaC expression for GetValue(T)
	ATTRIBUTE("Amplitude"    , &m_amplitude       );

	// Base class Prepare; the GiNaC expression for Shape is set from Pulse::Prepare
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

};

/***********************************************************/
inline double ConstantGradPulse::GetGradient (double const time) {

	return m_amplitude;

};

/***********************************************************/
string          ConstantGradPulse::GetInfo() {

	string val;
	GetAttribute(val,"Shape");

	stringstream s;
	s << GradPulse::GetInfo() << " , Shape = ";

	if ( HasDOMattribute("Diff") ) s << " d/DT ";

	s <<  val;

	return s.str();

};
