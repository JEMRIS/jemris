/** @file GradPulse.cpp
 *  @brief Implementation of JEMRIS GradPulse
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

#include "GradPulse.h"
#include "AtomicSequence.h"
#include "EddyPulse.h"
#include <limits>

GradPulse::GradPulse  () {

	m_axis          = AXIS_GX;
	m_area          = 0.0;
	m_non_lin_grad  = false;
	m_hide	    = false;
	m_nlg_field	= 0.0;
	m_nlg_px	= 0.0;
	m_nlg_py	= 0.0;
	m_nlg_pz	= 0.0;
	m_nlg_val	= 0.0;
	m_rise_time	= 0.0;
	m_slew_rate = 0.0;

	//SetExceptionalAttrib("NLG_field");

	//get defaults from the single instance of Parameters
	Parameters* P = Parameters::instance();
	if ( P->IsPrepared() )
	{
		m_slew_rate = *((double*) P->GetAttribute("GradSlewRate")->GetAddress());
		m_max_ampl  = *((double*) P->GetAttribute("GradMaxAmpl")->GetAddress());
		m_rise_time = *((double*) P->GetAttribute("GradRiseTime")->GetAddress());
	}

	m_eddy_pulse	= NULL;
	m_eddy_currents = false;
	m_eddy_val		= 0.0;
	m_eddy_time		= 0.0;
	m_ec_area		= 0.0;
	m_ec_length     = 0.0;


}

/***********************************************************/
GradPulse::~GradPulse  () {

}

/***********************************************************/
bool GradPulse::PrepareNLGfield  (PrepareMode mode) {

	GetAttribute("NLG_field")->SetObservable(true);

	if (!m_non_lin_grad) {
		string val=GetDOMattribute("NLG_field");
		//attributes for the current spin positions, and the gradient value
		HIDDEN_ATTRIBUTE("NLG_posX", m_nlg_px  );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_posX", mode == PREP_VERBOSE);
		ReplaceSymbolString(val,"X",GetName()+"_NLG_posX");
		HIDDEN_ATTRIBUTE("NLG_posY", m_nlg_py  );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_posY", mode == PREP_VERBOSE);
		ReplaceSymbolString(val,"Y",GetName()+"_NLG_posY");
		HIDDEN_ATTRIBUTE("NLG_posZ", m_nlg_pz  );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_posZ", mode == PREP_VERBOSE);
		ReplaceSymbolString(val,"Z",GetName()+"_NLG_posZ");
		HIDDEN_ATTRIBUTE("NLG_value",m_nlg_val );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_value", mode == PREP_VERBOSE);
		ReplaceSymbolString(val,"G",GetName()+"_NLG_value");
		//set the GiNaC expression and mark this gradient as nonlinear
		m_non_lin_grad = GetAttribute("NLG_field")->SetMember(val, m_obs_attribs, m_obs_attrib_keyword, mode == PREP_VERBOSE);
		//mark the parent AtomicSequence of this gradient as nonlinear
		if (GetParent() != NULL ) ((AtomicSequence*) GetParent())->SetNonLinGrad(m_non_lin_grad);
	}

	//test GiNaC evaluation of the Shape expression
	if (m_non_lin_grad) {
		try {
			GetAttribute("NLG_field")->EvalExpression();
		} catch (exception &p) {
			if (mode == PREP_VERBOSE) {
				cout	<< "Warning in " << GetName() << ": attribute NLG_field"
						<< " can not evaluate its GiNaC expression"
						<< " Reason: " << p.what() << endl;
			}
		}
	}

	return m_non_lin_grad;
}

/***********************************************************/
bool     GradPulse::PrepareEddyCurrents  (PrepareMode mode, int steps) {

	GetAttribute("EddyCurrents")->SetObservable(true);

	if (!m_eddy_currents) {
		string val=GetDOMattribute("EddyCurrents");
		//attributes for the current spin positions, and the gradient value
		HIDDEN_ATTRIBUTE("EddyTime", m_eddy_time  );
		Observe(GetAttribute("EddyCurrents"),GetName(),"EddyTime", mode == PREP_VERBOSE);
		// -> nonsense!! Observe(GetAttribute("EddyCurrents"),GetName(),"Area", mode == PREP_VERBOSE);
		ReplaceSymbolString(val,"T",GetName()+"_EddyTime");
		//set the GiNaC expression and mark this gradient as nonlinear
		m_eddy_currents = GetAttribute("EddyCurrents")->SetMember(val, m_obs_attribs, m_obs_attrib_keyword, mode == PREP_VERBOSE);
	}


	//test GiNaC evaluation of the Shape expression
	if (m_eddy_currents) {
		try {
			GetAttribute("EddyCurrents")->EvalExpression();
			//if (mode == PREP_VERBOSE) cout << GetName() << ": my Eddy has formula: "
			//		                       << GetAttribute("EddyCurrents")->GetFormula() << endl;

		} catch (exception &p) {
			if (mode == PREP_VERBOSE) {
				cout	<< "Warning in " << GetName() << ": attribute EddyCurrents"
						<< " can not evaluate its GiNaC expression"
						<< " Reason: " << p.what() << endl;
			}
		}
	}

	// prepare the eddy currents pusle
	if (m_eddy_currents) {
		if (m_eddy_pulse==NULL) m_eddy_pulse = new EddyPulse();
		m_eddy_pulse->SetGenPulse(this);
		m_eddy_currents = m_eddy_pulse->Prepare(mode);
		m_ec_area = m_eddy_pulse->GetAreaNumeric(1000);
	}

	return m_eddy_currents;

}

/***********************************************************/
bool GradPulse::Prepare  (PrepareMode mode) {

	bool btag = (m_axis >= AXIS_GX && m_axis <= AXIS_GZ);

  if (!btag && mode == PREP_VERBOSE)
		cout << GetName() << ": error in GradPulse::Prepare(). Wrong Axis for this gradient pulse:" << m_axis << endl;

	ATTRIBUTE("SlewRate"       , m_slew_rate);
	ATTRIBUTE("MaxAmpl"        , m_max_ampl);
	ATTRIBUTE("Area"           , m_area);
 	ATTRIBUTE("Hide"           , m_hide);


	//attribute for nonlinear-gradient (NLG) field evaluation
    ATTRIBUTE       ("NLG_field", m_nlg_field );

    //attribute for eddy current evaluation
	ATTRIBUTE("EddyCurrents"   , m_eddy_val  );
	ATTRIBUTE("EddyConvLength" , m_ec_length );
	HIDDEN_ATTRIBUTE("EC_Area" , m_ec_area   );


    // avoid Prototype::Prepare of the "NLG_field" attribute
	if (mode !=PREP_UPDATE && HasDOMattribute("NLG_field")) GetAttribute("NLG_field")->SetObservable(false);
    // avoid Prototype::Prepare of the "EddyCurrents" attribute
	if (mode !=PREP_UPDATE && HasDOMattribute("EddyCurrents")) GetAttribute("EddyCurrents")->SetObservable(false);

	//prepare call of base class
	btag = (Pulse::Prepare(mode) && btag);

	//prepare NLG_Field
	if ( mode !=PREP_UPDATE && HasDOMattribute("NLG_field")    && btag)	btag = PrepareNLGfield(mode);
	//prepare EddyCurrents
	if ( mode !=PREP_UPDATE && HasDOMattribute("EddyCurrents") && btag)	btag = PrepareEddyCurrents(mode);

    if (!btag && mode == PREP_VERBOSE)
                cout << "\n warning in Prepare(1) of GRADPULSE " << GetName() << endl;

	return btag;
}

/***********************************************************/
void GradPulse::GetValue (double * dAllVal, double const time) {

	if (time < 0.0 || time > GetDuration() || m_hide) { return ; }

	dAllVal[1+m_axis] += GetGradient(time);
	return;
}

/*****************************************************************/
inline void GradPulse::GenerateEvents(std::vector<Event*> &events) {
	GradEvent *grad = new GradEvent();
	double max_amplitude = std::numeric_limits<double>::min();
	int num_samples = round(GetDuration()/10.0e-3);
	for (int i=0; i<num_samples; i++)
	{
		double amp = GetGradient((i+1)*10.0e-3);

		if (amp>max_amplitude)
			max_amplitude=amp;

		grad->m_samples.push_back(amp);
	}
	transform( grad->m_samples.begin(), grad->m_samples.end(), grad->m_samples.begin(), bind2nd( divides<double>(), max_amplitude ) );

	grad->m_amplitude = max_amplitude;
	grad->m_channel = (int)(m_axis-AXIS_GX);
	grad->m_delay = round(GetInitialDelay()*1.0e3);

	events.push_back(grad);

	// Add ADCs (if any)
	int N = GetNADC();
	if (N>0) {
		ADCEvent *adc = new ADCEvent();
		adc->m_num_samples = N;
		adc->m_dwell_time = 1e6*GetDuration()/N;
		adc->m_delay = round(GetInitialDelay()*1.0e3);

		adc->m_phase_offset = 0;
		adc->m_freq_offset = 0;

		events.push_back(adc);
	}
}

/***********************************************************/
void GradPulse::SetNonLinGradField(double const time){

	if (m_hide) { return ; }

	//evaluate NLG field and add it to the World
	World::instance()->NonLinGradField += GetAttribute("NLG_field")->EvalCompiledNLGExpression
			( World::instance()->Values[0], World::instance()->Values[1], World::instance()->Values[2], GetGradient(time) );
	return;


}
/***********************************************************/
void GradPulse::SetArea (double val) {
	m_area = val;
	return;
}

/***********************************************************/
double GradPulse::GetAreaNumeric (int steps){

	if (m_hide) return 0.0;

	double Sum = 0.0;
    double DeltaT = GetDuration()/steps;
    for (int i=0; i<steps; ++i )
		Sum += GetGradient(i*DeltaT) ;

    return (Sum*DeltaT) ;

}

/***********************************************************/
string          GradPulse::GetInfo() {

	stringstream s;
	s << Pulse::GetInfo() << " , Area = " << m_area;

	if ( HasDOMattribute("NLG_field") )
		s << " , " << "NLG_field = " << GetDOMattribute("NLG_field");

	if ( m_hide )
		s << " , " << " hidden! " ;
	return s.str();
}


