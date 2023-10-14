/** @file AnalyticRFPulse.cpp
 *  @brief Implementation of JEMRIS AnalyticRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
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

#include "AnalyticRFPulse.h"

/***********************************************************/
AnalyticRFPulse::AnalyticRFPulse  (const AnalyticRFPulse& hrfp) {

	m_pulse_shape.SetPulse(this);

}

/***********************************************************/
bool AnalyticRFPulse::Prepare  (PrepareMode mode) {


    //set attributes "Shape", "Diff", "Constants" and initialize GiNaC evaluation
    if (mode != PREP_UPDATE) m_pulse_shape.PrepareInit(mode==PREP_VERBOSE);

    // Base class Prepare *before* analytic prepare of pulse shape
    bool btag = ( RFPulse::Prepare(mode) && m_pulse_shape.PrepareAnalytic(mode==PREP_VERBOSE) );

    //Calculate flip angle
    if (btag) m_flip_angle = (180.0/PI)*( (HasDOMattribute("Diff") && GetDOMattribute("Diff")=="1") ? m_pulse_shape.m_analytic_integral : GetIntegralNumeric(2000) );

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of AnalyticRFPulse " << GetName() << endl;

    if (mode != PREP_UPDATE) {
      HideAttribute("FlipAngle");
      HideAttribute("Bandwidth");
    }

    return btag;

}


/***********************************************************/
string          AnalyticRFPulse::GetInfo() {

	string val = GetDOMattribute("Shape");

	stringstream s;
	s << RFPulse::GetInfo() << " , Shape = ";

	if ( HasDOMattribute("Diff") ) s << " d/DT ";

	s <<  val;

	return s.str();
}
