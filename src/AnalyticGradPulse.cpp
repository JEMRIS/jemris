/** @file AnalyticGradPulse.cpp
 *  @brief Implementation of JEMRIS AnalyticGradPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#include "AnalyticGradPulse.h"

/***********************************************************/
AnalyticGradPulse::AnalyticGradPulse  (const AnalyticGradPulse&) {

	//SetExceptionalAttrib("Shape");

};

/***********************************************************/
bool AnalyticGradPulse::Prepare  (PrepareMode mode) {

	bool btag = true;

	// Automatically set a GiNaC expression for GetValue(T)
	//
	// Expression syntax example: a1*exp(-c1*T)*sin(c2*T)+a2
	// T stands for the time, c1, c2,... for the constants Const1, ..., Const5 defined
	// for this AnalyticGradPulse, and a1, a2, etc for further linked attributes from
	// other modules, as usual.
	ATTRIBUTE("Shape"    , m_analytic_value );
	ATTRIBUTE("TPOIs"    , m_more_tpois     ); // Number of TPOIs along the analytical expression.
	UNOBSERVABLE_ATTRIBUTE("Diff"     ); // Number of TPOIs along the analytical expression.
	UNOBSERVABLE_ATTRIBUTE("Constants");

	// Base class Prepare; the GiNaC expression for Shape is set from Pulse::Prepare
	btag = (GradPulse::Prepare(mode) && btag && m_analytic);

	//Calculate area
 	if (m_analytic) {

		bool numericArea = true;

		if (HasDOMattribute("Diff")) {

				// special case: diff=1
				// => area equals the anti-derivative computed from Pulse::prepare
				string sdiff = GetDOMattribute("Diff");

				if (sdiff=="1") {
					m_area      = m_analytic_integral;
					numericArea = false;
				}

		}

		//standard case: compute numerical integral with 2000 sampling points
		if ( numericArea )  m_area = GetAreaNumeric(2000);

	}

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
inline double AnalyticGradPulse::GetGradient (double const time) {

 	if (!m_analytic) return 0.0;
	m_analytic_time = time;
	GetAttribute("Shape")->EvalExpression();

	return m_analytic_value;

};

/***********************************************************/
string          AnalyticGradPulse::GetInfo() {

	string val=GetDOMattribute("Shape");

	stringstream s;
	s << GradPulse::GetInfo() << " , Shape = ";

	if ( HasDOMattribute("Diff") ) s << " d/dT ";

	s <<  val;

	return s.str();

};
