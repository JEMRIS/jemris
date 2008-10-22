/** @file GradPulse.cpp
 *  @brief Implementation of JEMRIS GradPulse
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

#include "GradPulse.h"
#include "AtomicSequence.h"

GradPulse::GradPulse  () {

	m_axis          = AXIS_GX;
	m_area          = 0.0;
	m_non_lin_grad  = false;
	m_nlg_field	= 0.0;
	m_nlg_px	= 0.0;
	m_nlg_py	= 0.0;
	m_nlg_pz	= 0.0;
	m_nlg_val	= 0.0;

	SetExceptionalAttrib("NLG_field");

	//get defaults from the single instance of Parameters
	Parameters* P = Parameters::instance();
	if ( P->IsPrepared() )
	{
		m_slew_rate = *((double*) P->GetAttribAddress("GradSlewRate"));
		m_max_ampl  = *((double*) P->GetAttribAddress("GradMaxAmpl"));
	}

};

GradPulse::~GradPulse  () {};


/***********************************************************/
bool GradPulse::Prepare  (PrepareMode mode) {

	bool btag = (m_axis == AXIS_GX || m_axis == AXIS_GY ||  m_axis == AXIS_GZ) ;
        if (!btag && mode == PREP_VERBOSE)
		cout << GetName() << ": error in GradPulse::Prepare(). Wrong Axis for this gradient pulse." << endl;

	ATTRIBUTE("SlewRate"       , &m_slew_rate);
	ATTRIBUTE("MaxAmpl"        , &m_max_ampl);
	ATTRIBUTE("Area"           , &m_area);

	//attributes for nonlinear-gradient (NLG) field evaluation
        ATTRIBUTE             ("NLG_field"   , &m_nlg_field );
        UNOBSERVABLE_ATTRIBUTE("NLG_posX"    , &m_nlg_px    );
        UNOBSERVABLE_ATTRIBUTE("NLG_posY"    , &m_nlg_py    );
        UNOBSERVABLE_ATTRIBUTE("NLG_posZ"    , &m_nlg_pz    );
        UNOBSERVABLE_ATTRIBUTE("NLG_value"   , &m_nlg_val   );

	//set the LNG expression 
	if ( mode !=PREP_UPDATE && HasDOMattribute("NLG_field") )
	{
		string val; 
		GetAttribute(val,"NLG_field");
		//apend attribute to which the current spin X-position is copied
		m_observed_modules.push_back(this);
		m_observed_attribs.push_back("NLG_posX");
		stringstream sX; sX << "a" << m_observed_modules.size();
		ReplaceString(val,"X",sX.str());
		//apend attribute to which the current spin Y-position is copied
		m_observed_modules.push_back(this);
		m_observed_attribs.push_back("NLG_posY");
		stringstream sY; sY << "a" << m_observed_modules.size();
		ReplaceString(val,"Y",sY.str());
		//apend attribute to which the current spin Z-position is copied
		m_observed_modules.push_back(this);
		m_observed_attribs.push_back("NLG_posZ");
		stringstream sZ; sZ << "a" << m_observed_modules.size();
		ReplaceString(val,"Z",sZ.str());
		//apend an attribute to which the current gradient value is copied
		m_observed_modules.push_back(this);
		m_observed_attribs.push_back("NLG_value");
		stringstream sG; sG << "a" << m_observed_modules.size();
		ReplaceString(val,"G",sG.str());
		//set the GiNaC expression and mark this gradient as nonlinear
		m_non_lin_grad = SetExpression("NLG_field",val,mode);
		//test evaluation
		try { EvalExpressions("NLG_field",mode); }
		catch (exception &p)
		{
			if ( mode == PREP_VERBOSE )
			{
				cout	<< "Warning in " << GetName() << ": attribute NLG_field" 
					<< " can not evaluate its GiNaC expression";
        			map<string,string>::iterator iex = m_attrib_expr_str.find("NLG_field"); 
				if (iex != m_attrib_expr_str.end() ) cout << " E = " << iex->second  << ".";
				cout   << " Reason: " << p.what() << endl;
			}
			btag = m_non_lin_grad = false;
		}
		//mark the parent AtomicSequence of this gradient as nonlinear
		if (GetParent() != NULL ) ((AtomicSequence*) GetParent())->SetNonLinGrad(m_non_lin_grad);
		
	}

	btag = (Pulse::Prepare(mode) && btag);

        if (!btag && mode == PREP_VERBOSE)
                cout << "\n warning in Prepare(1) of GRADPULSE " << GetName() << endl;

	return btag;
};

/***********************************************************/
void GradPulse::GetValue (double * dAllVal, double const time) {

	if (time < 0.0 || time > GetDuration()) { return ; }

        dAllVal[1+m_axis] += GetGradient(time);

	return;
}

/***********************************************************/
void GradPulse::SetNonLinGradField(double const time){
	//x,y,z position of current spin
	m_nlg_px = m_world->Values[0];
	m_nlg_py = m_world->Values[1];
	m_nlg_pz = m_world->Values[2];
	//current gradient value for analytic evaluation of the NLG field
	m_nlg_val = GetGradient(time);
	//evaluate NLG field and add it to the World
	EvalExpressions("NLG_field",PREP_UPDATE);
	m_world->NonLinGradField += m_nlg_field;
	return;
}
/***********************************************************/
void GradPulse::SetArea (double val) {
	m_area = val;
	return;
};

/***********************************************************/
double GradPulse::GetAreaNumeric (int steps){

    double Sum = 0.0;
    double DeltaT = GetDuration()/steps;
    for (int i=0; i<steps; ++i )
		Sum += GetGradient(i*DeltaT) ;

    return (Sum*DeltaT) ;

};

/***********************************************************/
string          GradPulse::GetInfo() {

	stringstream s;
	s << Pulse::GetInfo() << " , Area = " << m_area;
	if ( HasDOMattribute("NLG_field") )
	{
		string val; 
		GetAttribute(val,"NLG_field");
		s << " , " << "NLG_field = " << val;
	}

	return s.str();
};
