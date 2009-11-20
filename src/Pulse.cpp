/** @file Pulse.cpp
 *  @brief Implementation of JEMRIS Pulse
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
	ATTRIBUTE("Axis"        , m_axis          );
	ATTRIBUTE("ADCs"        , m_adc           );
    ATTRIBUTE("PhaseLock"   , m_phase_lock    );
    ATTRIBUTE("InitialDelay", m_initial_delay );

    // avoid Prototype::Prepare of the "Shape" attribute
	if (mode !=PREP_UPDATE && HasDOMattribute("Shape")) GetAttribute("Shape")->SetObservable(false);
	bool b = Module::Prepare(mode);
	//prepare the "Shape" attribute of analytic pulses
	if (mode !=PREP_UPDATE && HasDOMattribute("Shape"))  b = ( PrepareAnalytic(mode) && b);

	return b;
};

/***********************************************************/
bool Pulse::PrepareAnalytic  (PrepareMode mode) {

	GetAttribute("Shape")->SetObservable(true);

	//add "Shape" attribute observations only once
	if (!m_analytic) {

		//1. analytic time: replace "T" with the appropriate attribute counter "a{i}"
		HIDDEN_ATTRIBUTE("AnalyticTime",m_analytic_time);
		string val = GetDOMattribute("Shape");
		Observe(GetAttribute("Shape"),GetName(),"AnalyticTime", mode == PREP_VERBOSE);
		stringstream a;	a << "a" << m_obs_attribs.size();
		ReplaceString(val,"T",a.str());

		//2. constants: replace "c{i}" with the appropriate attribute counter "a{i}"
		if (HasDOMattribute("Constants")) {
			string constants = GetDOMattribute("Constants");
			vector<string> vp = Tokenize(constants,",");
			for (int i=0;i<vp.size();i++) {
				m_constant[i] = atof( vp[i].c_str() );
				stringstream C; C << "Constant" << i+1;
				HIDDEN_ATTRIBUTE(C.str() , m_constant[i]);
				Observe(GetAttribute("Shape"),GetName(),C.str(), mode == PREP_VERBOSE);
				stringstream c; c << "c" << i+1;
				stringstream a; a << "a" << m_obs_attribs.size();
				ReplaceString(val,c.str(),a.str());
			}
		}
		if (HasDOMattribute("Diff")){
			GetAttribute("Shape")->SetDiff(0);
			m_analytic = GetAttribute("Shape")->SetMember(val, m_obs_attribs, mode == PREP_VERBOSE);
			string sdiff = GetDOMattribute("Diff");
			int dif = atoi(sdiff.c_str());
			if (dif == 1) {
				try {
					m_analytic_time      = GetDuration();
					GetAttribute("Shape")->EvalExpression();
					m_analytic_integral  = m_analytic_value;
					m_analytic_time      = 0.0;
					GetAttribute("Shape")->EvalExpression();
					m_analytic_integral -= m_analytic_value;
				} catch (exception &p) {
				    if( mode == PREP_VERBOSE ) cout << "Error evaluating integral\n";
				}
				GetAttribute("Shape")->SetDiff(dif,GetAttribute("AnalyticTime")->GetSymbol() );
			}
		}
		m_analytic = GetAttribute("Shape")->SetMember(val, m_obs_attribs, mode == PREP_VERBOSE);
	}

	//set the GiNaC Expression and evaluate GetValue
	if (m_analytic) {
		//test GiNaC evaluation of the Shape expression
		try {
			GetAttribute("Shape")->EvalExpression();
		} catch (exception &p) {

			if (mode == PREP_VERBOSE) {
				cout	<< "Warning in " << GetName() << ": attribute Shape"
					    << " can not evaluate its GiNaC expression"
    					<< " Reason: " << p.what() << endl;
			}
		}
	}
	return m_analytic;
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
