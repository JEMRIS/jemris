/** @file GradPulse.cpp
 *  @brief Implementation of JEMRIS GradPulse
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

#include "GradPulse.h"
#include "AtomicSequence.h"
#include "EddyPulse.h"


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

	//SetExceptionalAttrib("NLG_field");

	//get defaults from the single instance of Parameters
	Parameters* P = Parameters::instance();
	if ( P->IsPrepared() )
	{
		m_slew_rate = *((double*) P->GetAttribute("GradSlewRate")->GetAddress());
		m_max_ampl  = *((double*) P->GetAttribute("GradMaxAmpl")->GetAddress());
	}

	m_eddy_pulse	= NULL;
	m_eddy_currents = false;
	m_eddy_val		= 0.0;
	m_eddy_time		= 0.0;
	m_ec_area		= 0.0;
	m_ec_length     = 100;


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
		stringstream sX; sX << "a" << m_obs_attribs.size();
		ReplaceString(val,"X",sX.str());
		HIDDEN_ATTRIBUTE("NLG_posY", m_nlg_py  );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_posY", mode == PREP_VERBOSE);
		stringstream sY; sY << "a" << m_obs_attribs.size();
		ReplaceString(val,"Y",sY.str());
		HIDDEN_ATTRIBUTE("NLG_posZ", m_nlg_pz  );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_posZ", mode == PREP_VERBOSE);
		stringstream sZ; sZ << "a" << m_obs_attribs.size();
		ReplaceString(val,"Z",sZ.str());
		HIDDEN_ATTRIBUTE("NLG_value",m_nlg_val );
		Observe(GetAttribute("NLG_field"),GetName(),"NLG_value", mode == PREP_VERBOSE);
		stringstream sG; sG << "a" << m_obs_attribs.size();
		ReplaceString(val,"G",sG.str());
		//set the GiNaC expression and mark this gradient as nonlinear
		m_non_lin_grad = GetAttribute("NLG_field")->SetMember(val, m_obs_attribs, mode == PREP_VERBOSE);
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
		stringstream sEC; sEC << "a" << m_obs_attribs.size();
		ReplaceString(val,"T",sEC.str());
		//set the GiNaC expression and mark this gradient as nonlinear
		m_eddy_currents = GetAttribute("EddyCurrents")->SetMember(val, m_obs_attribs, mode == PREP_VERBOSE);
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

	bool btag = (m_axis == AXIS_GX || m_axis == AXIS_GY ||  m_axis == AXIS_GZ) ;
        if (!btag && mode == PREP_VERBOSE)
		cout << GetName() << ": error in GradPulse::Prepare(). Wrong Axis for this gradient pulse." << endl;

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


