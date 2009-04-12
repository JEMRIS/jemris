/** @file Pulse.cpp
 *  @brief Implementation of JEMRIS Pulse
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

#include "Pulse.h"
#include "SequenceTree.h"

/***********************************************************/
Pulse::Pulse  () {

	m_axis              = AXIS_VOID;
	m_adc               = 0;
	m_initial_delay     = 0.0;
	m_phase_lock        = false;
	m_more_tpois        = 0;
	m_analytic          = false;
	m_analytic_value    = 0.0;
	m_analytic_time     = 0.0;
	m_analytic_integral = 0.0;

	for (int i=0;i<20;i++) m_constant[i]  = 0.0;

};

/***********************************************************/
bool Pulse::Prepare  (PrepareMode mode) {

	m_type = MOD_PULSE;

	//every Pulse might has Axis, Duration, ADCs, and an initial delay
	ATTRIBUTE("Axis"        , &m_axis          );
	ATTRIBUTE("ADCs"        , &m_adc           );
    ATTRIBUTE("PhaseLock"   , &m_phase_lock    );
    ATTRIBUTE("InitialDelay", &m_initial_delay );

	//UNOBSERVABLE attributes needed for analytic pulses.
	//For other pulses they do not hurt, except of taking a few memory.
    UNOBSERVABLE_ATTRIBUTE("AnalyticTime", &m_analytic_time );

	for (int i=0;i<20;i++) {
	 	stringstream s; s << "Constant" << i+1;
        UNOBSERVABLE_ATTRIBUTE(s.str() , &m_constant[i]);
	}

	//clear the observation lists, if this is an analytic pulse without observations
	if (mode !=PREP_UPDATE && HasDOMattribute("Shape") && !HasDOMattribute("Observe")) {
		m_observed_modules.clear();
		m_observed_attribs.clear();
	}

	bool b = Module::Prepare(mode);

	//set the Shape expression of analytic pulses
	if (mode !=PREP_UPDATE && HasDOMattribute("Shape")) {
		//apend an attribute to which the current time is copied
		m_observed_modules.push_back(this);
		m_observed_attribs.push_back("AnalyticTime");

		//get the expression, and replace "T" with the appropriate attribute counter "a{i}"
		string       val;
		GetAttribute(val,"Shape");

	 	stringstream s;
	 	s << "a" << m_observed_modules.size();
		ReplaceString(val,"T",s.str());

		//link constants, and replace "c{i}" with the appropriate attribute counter "a{i}"
		if (HasDOMattribute("Constants")) {

			string constants;
			GetAttribute(constants,"Constants");
			vector<string> vp = Tokenize(constants,",");

			for (int i=0;i<vp.size();i++) {

				m_constant[i] = atof( vp[i].c_str() );
	 			stringstream C; C << "Constant" << i+1;
				m_observed_modules.push_back(this);
				m_observed_attribs.push_back(C.str());
	 			stringstream c; c << "c" << i+1;
	 			stringstream a; a << "a" << m_observed_modules.size();
				ReplaceString(val,c.str(),a.str());

			}

		}

		//set the GiNaC Expression and evaluate GetValue
		m_analytic = SetExpression("Shape",val,mode);

		if (m_analytic) {

			if (HasDOMattribute("Diff")) {

				string sdiff;
				GetAttribute(sdiff,"Diff");

				int dif = atoi(sdiff.c_str());

				//evaluate expression before differentiation, to store the integral
				if (dif == 1) {

					try {
						m_analytic_time      = GetDuration();
						EvalExpressions("Shape",mode);
						m_analytic_integral  = m_analytic_value;
						m_analytic_time      = 0.0;
						EvalExpressions("Shape",mode);
						m_analytic_integral -= m_analytic_value;
					} catch (exception &p) {
					    if( mode == PREP_VERBOSE ) cout << "Error evaluating integral\n";
					}

				}

				//set the order of derrivative
        		if (m_attrib_expr_dif.find("Shape") != m_attrib_expr_dif.end())
            		m_attrib_expr_dif.erase("Shape");

        		m_attrib_expr_dif.insert(pair<string,int> ("Shape", dif));

				//map<string,GiNaC::ex>::iterator i = m_attrib_expr_str.find("Shape");
				//if (i != m_attrib_expr_str.end() )
				//i->second = (i->second).diff( GetAttribSymbol("AnalyticTime"),dif );
				//DEBUG_PRINT("\n (d/dt)^"<< dif << " E = " << i->second << endl << endl;)

			}

			try {
			    EvalExpressions("Shape",mode);
			} catch (exception &p) {

				if (mode == PREP_VERBOSE) {
					cout	<< "Warning in " << GetName() << ": attribute Shape"
						    << " can not evaluate its GiNaC expression";
        			map<string,string>::iterator iex = m_attrib_expr_str.find("Shape");

					if (iex != m_attrib_expr_str.end())
					    cout << " E = " << iex->second  << ".";

					cout   << " Reason: " << p.what() << endl;
				}

				b = m_analytic = false;

			}
		}
	}

	return b;
};

/***********************************************************/
inline void  Pulse::SetTPOIs () {

    m_tpoi.Reset();
// removed tpoi at beginning; could purge TPOI with phase == -2.0
    //    m_tpoi + TPOI::set(TIME_ERR_TOL              , -1.0);

    m_tpoi + TPOI::set(GetDuration(), -1.0);

    for (unsigned i = 1; i < m_more_tpois; i++)
    	m_tpoi + TPOI::set((i+1)*GetDuration()/(m_more_tpois+1), -1.0 );

    for (unsigned i = 0; i < GetNADC(); i++)
    	m_tpoi + TPOI::set((i+1)*GetDuration()/(GetNADC()+1), (m_phase_lock?m_world->PhaseLock:0.0) );

};

/***********************************************************/
void Pulse::SetDuration (double val) {
    if (val<0.0) return;
    m_duration=val;
};

/***********************************************************/
string          Pulse::GetInfo() {

	string ret;
	switch (m_axis) {
		case AXIS_RF : ret=" Axis = RF "; break;
		case AXIS_GX : ret=" Axis = GX "; break;
		case AXIS_GY : ret=" Axis = GY "; break;
		case AXIS_GZ : ret=" Axis = GZ "; break;
		default: ret=" Axis = none ";
	}

	stringstream s;
	s << ret;
	if (m_initial_delay>0.0) s << " , InitDelay = " << m_initial_delay;

	return s.str();

};
