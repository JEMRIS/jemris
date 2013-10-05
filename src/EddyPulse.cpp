/** @file EddyPulse.cpp
 *  @brief Implementation of JEMRIS EddyPulse
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

#include "EddyPulse.h"
#include "SequenceTree.h"
#include "XMLIO.h"


/***********************************************************/
void	EddyPulse::Init           () {
	 m_axis = AXIS_VOID;
	 m_parent = NULL;
	 m_gen_pulse = NULL;
	 m_prepared = false;
	 m_length = 500;		/* TMP !!! needs to be user-defined */
	 m_dt	  = 0.0;
	 m_linger_time = 0.0;
}
/***********************************************************/
bool EddyPulse::Prepare  (PrepareMode mode) {

	//if  (m_prepared ) return true;

    m_axis = AXIS_VOID;
    SetTPOIs ();

    //parent of generating pulse is an AtomicSequence
    m_parent = ( (AtomicSequence*) (m_gen_pulse->GetParent() ) );
    if (m_parent == NULL) return false;
    m_parent->SetEddyCurrents(true);

    if (m_gen_pulse == NULL ) return false;

    if (m_gen_pulse->HasDOMattribute("EddyConvLength"))
    	m_length = *((int*) m_gen_pulse->GetAttribute("EddyConvLength")->GetAddress());

    SetAxis     ( m_gen_pulse->GetAxis()       );
    SetDuration ( m_gen_pulse->GetDuration()   );
    SetName     ( "EC_"+m_gen_pulse->GetName() );
    m_initial_delay = m_gen_pulse->GetInitialDelay() ;

	if ( !m_prepared ) 	m_prepared = Insert(mode);
	if ( !m_prepared ) return false; //should never happen

	//prepare for GetValue of decaying eddies outside the atom
	ConvKernel() ;

    m_area = GetAreaNumeric(1000);

	//use NLGs from the generating pulse
	if ( m_parent->HasDOMattribute("NLG_field") && !HasDOMattribute("NLG_field") ) {
		AddDOMattribute("NLG_field",m_parent->GetDOMattribute("NLG_field"));
	}

	//prepare call of base class
	m_prepared = (GradPulse::Prepare(mode) && m_prepared);

    return m_prepared;
}

/*****************************************************************/
bool  EddyPulse::ConvKernel () {

	double t = 0.0, d=0.0, e=0.0, s=0.0;
	int imax = 0, n = 50;
	m_kernel.clear();

	m_dt = m_gen_pulse->GetDuration()/m_length;

	//step 1: total energy of IRF (impulse response function == convolution kernel)
	for (int i=0; i<n*m_length; ++i) {
		t = (i+1)*m_dt;
		d = m_gen_pulse->GetAttribute("EddyCurrents")->EvalCompiledExpression(t,"EddyTime");
		e += pow(d,2.0);
	}
	//step 2: find significant length of IRF
	for (int i=0; i<n*m_length; ++i) {
		t = (i+1)*m_dt;
		d = m_gen_pulse->GetAttribute("EddyCurrents")->EvalCompiledExpression(t,"EddyTime");
		s += pow(d,2.0);
		if ( s > 0.99*e ) { imax=i; break; }
	}
	//step 3: store IRF
	for (int i=0; i<imax; ++i) {
		t = (i+1)*m_dt;
		d = m_gen_pulse->GetAttribute("EddyCurrents")->EvalCompiledExpression(t,"EddyTime");
		m_kernel.push_back(d);
	}

	//cout << " CS (" << t << ") = " << s << " at " << imax << endl;

	// - set duration of this EddyCurrent
	// - if longer than parent, add this EddyCurrent to world multimap lingering
	d = m_gen_pulse->GetDuration() + m_kernel.size()*m_dt;
	//cout << "! MD = " << m_parent->GetDuration() << " : " << m_gen_pulse->GetDuration() << " : "<< m_kernel.size() << endl;
	if (d>m_parent->GetDuration()) {
		World* pW = World::instance();
		m_linger_time = d - m_parent->GetDuration() ;
		multimap<EddyPulse*,double>::iterator iter = pW->m_eddies.find(this);
		if( iter == pW->m_eddies.end() )
			pW->m_eddies.insert(pair<EddyPulse*,double>(this, -m_linger_time));
		else
			iter->second = -m_linger_time;
		d = m_parent->GetDuration();
		//cout << "!! " << m_linger_time << endl;
	}

    SetDuration ( d );

	return true;
}
/*****************************************************************/
bool  EddyPulse::Insert (PrepareMode mode) {

	if (mode==PREP_UPDATE) return true;

	//insert DOM-node in parent
	DOMElement* node = SequenceTree::instance()->GetDOMDocument()->createElement(StrX("EDDYCURRENT").XMLchar());
	if (node==NULL) return false;
	m_parent->GetNode()->appendChild (node);
	SequenceTree::instance()->GetModuleMap()->insert(pair<DOMNode*, Module*> (node, this));
	Initialize(node);

	return true;

}

/*****************************************************************/
inline void  EddyPulse::SetTPOIs () {

	  Pulse::SetTPOIs();
	    double dt = 0.0;

	  	if (m_parent==NULL)
	  		dt = GetDuration()/(m_length+1);
	  	else
	  		dt = m_parent->GetDuration()/(m_length+1);

	    for (unsigned i = 1; i < m_length; i++)
	    	m_tpoi + TPOI::set((i+1)*dt, -1.0 );

}

/***********************************************************/
void EddyPulse::GetValue  (double * dAllVal, double const time){
	if (time < 0.0 || time > m_parent->GetDuration() + m_linger_time ) { return ; }

    dAllVal[1+m_axis] += GetGradient(time);

return;
}

/***********************************************************/
double EddyPulse::GetGradient  (double const time){

	double t=0.0, d=0.0;

	bool hide = *((bool*) m_gen_pulse->GetAttribute("Hide")->GetAddress()) ;
	*((bool*) m_gen_pulse->GetAttribute("Hide")->GetAddress()) = false;

	//convolve dG/dt with kernel
	for (int i=0; i<m_kernel.size(); ++i) {
		t = time - i*m_dt;
		if ( t > m_gen_pulse->GetDuration() ) continue;
		if ( t < m_dt          				) break;
		d += ( m_gen_pulse->GetGradient(t) - m_gen_pulse->GetGradient(t-m_dt) ) * m_kernel.at(i);
	}

	*((bool*) m_gen_pulse->GetAttribute("Hide")->GetAddress()) = hide;

	return d;
	//cout << "EC GetValue : " << time << " : " << v << " : " << d << " : " << d-v << endl;
}

/***********************************************************/
double EddyPulse::GetAreaNumeric (int steps){

    double Sum = 0.0, DeltaT = GetDuration()/steps;

    for (int i=0; i<steps; ++i ) Sum += GetGradient(i*DeltaT);

    return (Sum*DeltaT) ;

}

/***********************************************************/
string          EddyPulse::GetInfo() {

	stringstream s;

	s << GradPulse::GetInfo();
	s << " , Impulse Response = " << m_gen_pulse->GetDOMattribute("EddyCurrents");
	s << " , linger time = " << m_linger_time;
	return s.str();
}
