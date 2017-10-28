/** @file AnalyticPulseShape.cpp
 *  @brief Implementation of JEMRIS AnalyticPulseShape
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#include "AnalyticPulseShape.h"
#include "AnalyticRFPulse.h"
#include "AnalyticGradPulse.h"

/***********************************************************/
AnalyticPulseShape::AnalyticPulseShape ()  {

	m_prepared		= false;
	m_rfpulse		= false;
	//m_more_tpois		= 0;
	m_analytic_value	= 0.0;
	m_analytic_time		= 0.0;
	m_analytic_integral	= 0.0;

	for (int i=0;i<20;i++)	m_constant[i]  = 0.0;

};

/***********************************************************/
double            AnalyticPulseShape::GetShape (double const time)  {

	if (!m_prepared) return 0.0;

	double d = m_pulse->GetAttribute("Shape")->EvalCompiledExpression(time,"AnalyticTime");

	if ( m_rfpulse ) {
	  double imag = m_pulse->GetAttribute("Shape")->GetImaginary();
	  m_phase = atan2(imag,d);
	  return sqrt(pow(imag,2)+pow(d,2)) ;
	}

	return d;
}

/***********************************************************/
/*
void  AnalyticPulseShape::SetTPOIs () {

    m_pulse->Pulse::SetTPOIs();

    for (int i = 1; i < m_more_tpois; i++)
    	m_pulse->m_tpoi + TPOI::set((i+1)*m_pulse->GetDuration()/(m_more_tpois+1), -1.0 );


};
*/

/*****************************************************************/
double    AnalyticPulseShape::GetPhase  (Module* mod, double time ) {

	AnalyticPulseShape*  p = ( (AnalyticRFPulse*) mod)->GetPulseShape();
	return 180/PI*(p->m_phase) ;
};


/***********************************************************/
void  AnalyticPulseShape::PrepareInit (bool verbose) {

    if (m_pulse->GetAxis() == AXIS_RF) {
	m_rfpulse=true;
	( (AnalyticRFPulse*) m_pulse)->insertGetPhaseFunction( &AnalyticPulseShape::GetPhase );
    }

    //init observable and visible attributes: "Shape", "TPOIs"
    if (m_pulse->m_attributes.find("Shape")==m_pulse->m_attributes.end() )
	m_pulse->m_attributes.insert(pair<string,Attribute*>("Shape",new Attribute("Shape",m_pulse, true, true, m_analytic_value)));
    //if (m_pulse->m_attributes.find("TPOIs")==m_pulse->m_attributes.end() )
	//m_pulse->m_attributes.insert(pair<string,Attribute*>("TPOIs",new Attribute("TPOIs",m_pulse, true, true, m_more_tpois)));

    //init unobservable and visible attributes: "Diff" and "Constants"
    if (m_pulse->m_attributes.find("Diff")==m_pulse->m_attributes.end() )
	m_pulse->m_attributes.insert(pair<string,Attribute*>("Diff",new Attribute("Diff",m_pulse, false, false )));
    if (m_pulse->m_attributes.find("Constants")==m_pulse->m_attributes.end() )
	m_pulse->m_attributes.insert(pair<string,Attribute*>("Constants",new Attribute("Constants",m_pulse, false, false )));

    m_pulse->GetAttribute("Shape")->ResetCurrentFunctionPointer();	// if not in update, start to iterate compiled functions from beginning
    m_pulse->GetAttribute("Shape")->SetObservable(false);		// avoid Prototype::Prepare of the "Shape" attribute

};

/***********************************************************/
bool  AnalyticPulseShape::PrepareAnalytic (bool verbose) {

	m_pulse->GetAttribute("Shape")->SetObservable(true);

	//1. "Shape" observes "AnalyticTime": replace "T" with the appropriate attribute counter "a{i}"
		if (m_pulse->m_attributes.find("AnalyticTime")==m_pulse->m_attributes.end() )
			m_pulse->m_attributes.insert(pair<string,Attribute*>("AnalyticTime",new Attribute("AnalyticTime",m_pulse, false, true, m_analytic_time)));
		string val = m_pulse->GetDOMattribute("Shape");
		if (val.empty()) return true;
		m_pulse->Observe(m_pulse->GetAttribute("Shape"),m_pulse->GetName(),"AnalyticTime", verbose);
		m_pulse->ReplaceSymbolString(val,"T",m_pulse->GetName()+"_AnalyticTime");

	//2. "Shape" observes "Constants": replace "c{i}" with the appropriate attribute counter "a{i}"
		if (m_pulse->HasDOMattribute("Constants")) {
			string constants = m_pulse->GetDOMattribute("Constants");
			vector<string> vp = m_pulse->Tokenize(constants,",");
			for (unsigned int i=0;i<vp.size();i++) {
				m_constant[i] = atof(vp[i].c_str());
				stringstream C; C << "Constant" << i+1;
				if (m_pulse->m_attributes.find(C.str())==m_pulse->m_attributes.end() )
				    m_pulse->m_attributes.insert(pair<string,Attribute*>(C.str(),new Attribute(C.str(),m_pulse, false, true, m_constant[i])));
				m_pulse->Observe(m_pulse->GetAttribute("Shape"),m_pulse->GetName(),C.str(), verbose);
				stringstream c; c << "c" << i+1;
				m_pulse->ReplaceSymbolString(val,c.str(),m_pulse->GetName()+"_"+C.str());
			}
		}

	//3. analytic derivative and calculation of the integral
		if (m_pulse->HasDOMattribute("Diff")) {
			m_pulse->GetAttribute("Shape")->SetDiff(0);
			m_pulse->GetAttribute("Shape")->SetMember(val, m_pulse->m_obs_attribs, m_pulse->m_obs_attrib_keyword, verbose);
			string sdiff =  m_pulse->GetDOMattribute("Diff");
			int dif = atoi(sdiff.c_str());
			if (dif==1) {
				m_analytic_time      =  m_pulse->GetDuration();
				m_pulse->GetAttribute("Shape")->EvalExpression();
				m_analytic_integral  =  m_analytic_value;
				m_analytic_time      = 0.0;
				m_pulse->GetAttribute("Shape")->EvalExpression();
				m_analytic_integral -=  m_analytic_value;
			}
			m_pulse->GetAttribute("Shape")->SetDiff(dif,m_pulse->GetAttribute("AnalyticTime")->GetSymbol() );
		}

	//4. prepare GiNaC evaluation of the "Shape" attribute
		m_prepared =  m_pulse->GetAttribute("Shape")->SetMember(val,  m_pulse->m_obs_attribs, m_pulse->m_obs_attrib_keyword, verbose);

	return m_prepared;

};
