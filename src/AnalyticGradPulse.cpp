/** @file AnalyticGradPulse.cpp
 *  @brief Implementation of JEMRIS AnalyticGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#include "AnalyticGradPulse.h"

/***********************************************************/
AnalyticGradPulse::AnalyticGradPulse  (const AnalyticGradPulse&) {

	m_pulse_shape.SetPulse(this);

}

/***********************************************************/
bool AnalyticGradPulse::Prepare  (const PrepareMode mode) {

    //set attributes "Shape", "Diff", "Constants" and initialize GiNaC evaluation
    if (mode != PREP_UPDATE) m_pulse_shape.PrepareInit(mode==PREP_VERBOSE);

    // Base class Prepare *before* analytic prepare of pulse shape
    bool btag = ( GradPulse::Prepare(mode) && mode != PREP_UPDATE && m_pulse_shape.PrepareAnalytic(mode==PREP_VERBOSE) );

    //Calculate area
    if (HasDOMattribute("Diff") && GetDOMattribute("Diff")=="1")
    	m_area = m_pulse_shape.GetAnalyticIntegral(mode==PREP_VERBOSE) ;
    else
    	m_area = GetAreaNumeric((int) (10000*GetDuration()));

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of AnalyticGradPulse " << GetName() << endl;

    if (mode != PREP_UPDATE) {
		HideAttribute("Area"           );
		HideAttribute("MaxAmpl",  false);
		HideAttribute("SlewRate", false);
	}

    return btag;

}


/***********************************************************/
string          AnalyticGradPulse::GetInfo() {

	string val=GetDOMattribute("Shape");

	stringstream s;
	s << GradPulse::GetInfo() << " , Shape = ";

	if ( HasDOMattribute("Diff") ) s << " d/dT ";

	s <<  val;

	return s.str();

}
