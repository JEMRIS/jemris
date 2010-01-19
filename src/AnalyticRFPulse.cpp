/** @file AnalyticRFPulse.cpp
 *  @brief Implementation of JEMRIS AnalyticRFPulse
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

#include "AnalyticRFPulse.h"

/***********************************************************/
AnalyticRFPulse::AnalyticRFPulse  (const AnalyticRFPulse& hrfp) {

	m_analytic_phase = 0.0;
	//SetExceptionalAttrib("Shape");

};

/***********************************************************/
bool AnalyticRFPulse::Prepare  (PrepareMode mode) {

	bool btag = true;

	//automatically set a GiNaC expression for GetValue(T)
	//
	//expression syntax example: a1*exp(-c1*T)*sin(c2*T)+a2
	//T stands for the time, c1, c2,... for the constants Const1, ..., Const5 defined
	//for this AnalyticRFPulse, and a1, a2, etc for further linked attributes from
	//other modules, as usual.
	ATTRIBUTE("Shape"    , m_analytic_value );
	ATTRIBUTE("TPOIs"    , m_more_tpois     ); // Number of TPOIs along the analytical expression.
	UNOBSERVABLE_ATTRIBUTE("Diff"     ); // Number of TPOIs along the analytical expression.
	UNOBSERVABLE_ATTRIBUTE("Constants");

	//base class Prepare; the GiNaC expression for Shape is set from Pulse::Prepare
	btag = (RFPulse::Prepare(mode) && btag && m_analytic);

	//Calculate area
 	if (m_analytic) {

		bool numericArea = true;

		if (HasDOMattribute("Diff")) {

            // special case: diff=1
            // => area equals the anti-derivative computed from Pulse::prepare
            string sdiff = GetDOMattribute("Diff");

            if (sdiff=="1") {
                m_flip_angle = (180.0/PI)*m_analytic_integral;
                numericArea  = false;
            }
		}
	}

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;

    if (mode != PREP_UPDATE) {
		insertGetPhaseFunction( &AnalyticRFPulse::getAnalyticPhase );
		HideAttribute("FlipAngle");
	}

	return btag;

};

/***********************************************************/
inline double  AnalyticRFPulse::GetMagnitude  (double const time){

	if (!m_analytic) return 0.0;
	m_analytic_time = time;
	GetAttribute("Shape")->EvalExpression();

	if ( GetAttribute("Shape")->IsComplex() )
	{
		double imag = GetAttribute("Shape")->GetImaginary();
		m_analytic_phase = atan2(imag,m_analytic_value);
		return sqrt(pow(imag,2)+pow(m_analytic_value,2)) ;
	}

	return m_analytic_value;
};

/***********************************************************/
string          AnalyticRFPulse::GetInfo() {

	string val = GetDOMattribute("Shape");

	stringstream s;
	s << RFPulse::GetInfo() << " , Shape = ";

	if ( HasDOMattribute("Diff") ) s << " d/DT ";

	s <<  val;

	return s.str();
};
