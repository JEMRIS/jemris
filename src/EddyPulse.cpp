/** @file EddyPulse.cpp
 *  @brief Implementation of JEMRIS EddyPulse
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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
	 m_area_gen_pulse = 0.0;
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

	if ( !Insert(mode) ) return true; //was already inserted (?)

	//prepare eddy currents
	m_prepared       = Convolve() ;

	//use NLGs from the generating pulse
    //cout << GetName() << " : " << m_gen_pulse->HasDOMattribute("NLG_field") << " : " << HasDOMattribute("NLG_field") << endl;
    if ( mode !=PREP_UPDATE) {
    	if ( m_gen_pulse->HasDOMattribute("NLG_field") && !HasDOMattribute("NLG_field") ) {
    		AddDOMattribute("NLG_field",m_gen_pulse->GetDOMattribute("NLG_field"));
    		m_non_lin_grad = true;
    	}
    		//cout << " !! " << GetDOMattribute("NLG_field") << endl;
    		// -> nonsense? Observe ( GetAttribute("Area"), m_gen_pulse->GetName(),"Area", mode == PREP_VERBOSE);
    }

	//prepare call of base class
	m_prepared = (GradPulse::Prepare(mode) && m_prepared);

    return m_prepared;
}

/*****************************************************************/
bool  EddyPulse::Convolve () {

	// Check if this eddy convolution has to be recalculated, assuming that nothing
	// has to be done if area is set and are of generating pulse is unchanged.
	double gp_area = m_gen_pulse->GetAreaNumeric(1000);
	if (fabs(m_area) > 0.0 && m_area_gen_pulse == gp_area) return true;

	m_area_gen_pulse = gp_area;
	double t = 0.0, d=0.0, e=0.0, s=0.0;
	int imax = 256000; // max. number of points for eddy kernel
	m_kernel.clear();
    m_eddy.clear();


	m_dt = m_gen_pulse->GetDuration()/m_length;

	if (m_dt < 1e-16) return true;

	//step 1: find significant kernel size ( assuming a decaying IRF in steps 10*m_dt ! )
	for (int i=0; i<imax/10; ++i) {
		s=0.0;
		for (int j=0;j<10; j++) {
			d = m_gen_pulse->GetAttribute("EddyCurrents")->EvalCompiledExpression((i*10+j)*m_dt,"EddyTime");
			s += pow(d,2.0);
		}
		e += s;
		if ( pow(s,2.0) < 1e-6*e/(i+1) ) { imax=i*10; break; }
	}

	//step 2: store IRF
	for (int i=0; i<imax; ++i) {
		t = (i+1)*m_dt;
		d = m_gen_pulse->GetAttribute("EddyCurrents")->EvalCompiledExpression(i*m_dt,"EddyTime");
		m_kernel.push_back(d);
	}

	//step 3: compute eddy current by convolution
	for (size_t n = 0; n < m_length + m_kernel.size() - 1; n++) {
	   	m_eddy.push_back(0.0);
	   	size_t kmin = (n >= m_kernel.size() - 1) ? n - (m_kernel.size() - 1) : 0           ;
	   	size_t kmax = (n < m_length - 1)         ? n                         : m_length - 1;
	       for (size_t k = kmin; k < kmax; k++)
	    	   m_eddy[n] -= m_kernel[n-k] *( m_gen_pulse->GetGradient((k+1)*m_dt)-m_gen_pulse->GetGradient(k*m_dt) );//m_dt ;
	}

    //double norm = 0.0; for (int i=0; i<m_eddy.size(); i++) { norm += m_eddy[i]*m_eddy[i]; } norm=sqrt(norm);
    //cout << "EDDY " << GetName() << ": " << m_length << "," << m_kernel.size() << "," << m_dt << "," << norm << endl;


	// final steps:
    // - set duration of this EddyCurrent
	// - if longer than parent atom, add this EddyCurrent to world multimap lingering
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

    m_area = GetAreaNumeric(1000);


    return true;

}
/*****************************************************************/
bool  EddyPulse::Insert (PrepareMode mode) {

	if (m_prepared || mode==PREP_UPDATE) return true;

	if (m_parent->GetPrototypeByAttributeValue("Name",GetName()) != NULL)  return false;  //already inserted

	//insert DOM-node in parent
	DOMElement* node = m_parent->GetSeqTree()->GetDOMDocument()->createElement(StrX("EDDYCURRENT").XMLchar());
	if (node==NULL) return false;
	m_parent->GetNode()->appendChild (node);
	m_parent->GetSeqTree()->GetModuleMap()->insert(pair<DOMNode*, Module*> (node, this));
	SetSeqTree(m_parent->GetSeqTree());
	Initialize(node);
	m_prepared = true;
	return true;

}

/*****************************************************************/
inline void  EddyPulse::SetTPOIs () {

	  Pulse::SetTPOIs();
/*
	    double dt = 0.0;

	  	if (m_parent==NULL)
	  		dt = GetDuration()/(m_length+1);
	  	else
	  		dt = m_parent->GetDuration()/(m_length+1);

	    for (unsigned i = 1; i < m_length; i++)
	    	m_tpoi + TPOI::set((i+1)*dt, -1.0 );
*/
}

/***********************************************************/
void EddyPulse::GetValue  (double * dAllVal, double const time){
	//if (time < 0.0 || time > m_parent->GetDuration() + m_linger_time ) { return ; }

    dAllVal[1+m_axis] += GetGradient(time);

return;
}

/***********************************************************/
double EddyPulse::GetGradient  (double const time){

	size_t  n = time/m_dt;
	return  ( (n<m_eddy.size() ) ? m_eddy[n] : 0.0 );

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
	s << " , kernel size = " << m_kernel.size();
	s << " , linger time = " << m_linger_time;
	return s.str();
}
