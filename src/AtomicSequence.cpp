/** @file AtomicSequence.cpp
 *  @brief Implementation of JEMRIS AtomicSequence
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

#include "AtomicSequence.h"
#include "GradPulse.h"
#include "EddyPulse.h"
#include "RFPulse.h"

/***********************************************************/
AtomicSequence::AtomicSequence  (const AtomicSequence& as) :
	m_eddy(0), m_alpha(0.), m_phi(0.), m_theta(0.), m_non_lin_grad(false) {}

/***********************************************************/
bool    AtomicSequence::Prepare(const PrepareMode mode) {

	ATTRIBUTE("RotAngle"       , m_alpha );
	ATTRIBUTE("Inclination"    , m_theta );
	ATTRIBUTE("Azimut"         , m_phi   );

	if (mode != PREP_UPDATE) m_type = MOD_ATOM;

	bool tag = Sequence::Prepare(mode); //Prepare all pulses
	CollectTPOIs();  //of the pulses

	CalcDuration();
	return tag;

}

/***********************************************************/
double AtomicSequence::CalcDuration () {

	vector<Module*> children = GetChildren();
	m_duration = 0.;

	for (unsigned int j=0; j<children.size() ; ++j)
		m_duration = fmax( m_duration, children[j]->GetDuration()+((Pulse*) children[j])->GetInitialDelay() );

	DEBUG_PRINT("  AtomicSequence::GetDuration() of " << GetName() <<
		    " calculates duration = " << dDuration << endl;)

	Notify(m_duration);

}

/***********************************************************/
inline void      AtomicSequence::GetValue (double * dAllVal, double const time) {

    if (time < 0.0 || time > m_duration) { return ; }

    if (m_non_lin_grad) World::instance()->NonLinGradField = 0.0;
    vector<Module*> children = GetChildren();

    for (unsigned int j=0; j<children.size() ; ++j) {

    	//special case: dead time
    	if ( ((Pulse*) children[j])->GetAxis()==AXIS_VOID ) continue;

    	//special case: out of time support region
    	double pulse_time = time - ((Pulse*) children[j])->GetInitialDelay();
    	if (pulse_time < 0.0) continue;

    	//special case: non linear gradients
    	if (m_non_lin_grad && ((Pulse*) children[j])->GetAxis()!=AXIS_RF
    					   && ((Pulse*) children[j])->GetAxis()!=AXIS_VOID
    			           && ((GradPulse*) children[j])->HasNonLinGrad()) {
    	    ((GradPulse*) children[j])->SetNonLinGradField(pulse_time);
    	    continue;
    	}

    	//special case: apply RF pulse on every transmit coil, if
    	if ( ((Pulse*) children[j])->GetAxis()==AXIS_RF     		&&	// 1.) the pulse is an RF Pulse
    		 ((RFPulse*) children[j])->GetCoilArray()!=NULL 		&&	// 2.) it has a coil array
    		 ((RFPulse*) children[j])->GetCoilArray()->GetSize()>1  &&	// 3.) the array has multiple coils
    		 !children[j]->HasDOMattribute("Channel") ) {				// 4.) the RF pulse has no channel explicitly specified
    		for (unsigned k=0; k<((RFPulse*) children[j])->GetCoilArray()->GetSize(); k++) {
    			((RFPulse*) children[j])->SetChannel(k);
    			children[j]->GetValue(dAllVal,pulse_time);
    		}
    		((RFPulse*) children[j])->SetChannel(0);
    		continue;
    	}

    	//standard case
        children[j]->GetValue(dAllVal,pulse_time);

    }

    Rotation(&dAllVal[2]);

}

/***********************************************************/
inline void AtomicSequence::Rotation (double * Grot) {

    if (m_alpha == 0.0)
    	return;

    double Gx = Grot[0];
    double Gy = Grot[1];
    double Gz = Grot[2];

    double pi_180 = PI/180.0;
    double alpha = m_alpha * pi_180;
    double theta = m_theta * pi_180;
    double phi   = m_phi   * pi_180;

    double cos_theta   = cos(theta);
    double sin_theta   = sin(theta);
    double cos_2_theta = cos_theta * cos_theta;
    double sin_2_theta = sin_theta * sin_theta;
    double cos_alpha   = cos(alpha);
    double sin_alpha   = sin(alpha);
    double cos_phi     = cos(phi);
    double sin_phi     = sin(phi);
    
    Grot[0] =
        ((cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)*
        	cos_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*sin_phi)*Gx+
        	(-(cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)*
        	sin_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*cos_phi)*Gy+
        	(cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+sin_phi*sin_theta*sin_alpha)*Gz;

    Grot[1] =
        ((-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)*
        	cos_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*sin_phi)*Gx+
        	(-(-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)*
        	sin_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*cos_phi)*Gy+
        	(-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+cos_phi*sin_theta*sin_alpha)*Gz;

    Grot[2] = (cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-sin_phi*sin_theta*sin_alpha)*Gx+
    		(-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-cos_phi*sin_theta*sin_alpha)*Gy+
    		(sin_2_theta*cos_alpha+cos_2_theta)*Gz;

    return;

}
/***********************************************************/
void AtomicSequence::CollectTPOIs() {

	vector<Module*> children = GetChildren();
	Pulse* p;
	double d;

	m_tpoi.Reset();

	for (size_t j = 0; j < children.size(); ++j) {

		p = ((Pulse*)children[j]);
		d = p->GetInitialDelay();
		p->SetTPOIs();


		for (size_t i = 0; i < p->GetNumOfTPOIs(); ++i)  {

			m_tpoi	+ TPOI::set(d + p->GetTPOIs()->GetTime(i),
					p->GetTPOIs()->GetPhase(i), p->GetTPOIs()->GetMask(i));

			//one TPOI prior to the pulse in case of intial delay phase == -2.0 -> ReInit CVode
			if (d>TIME_ERR_TOL)
				m_tpoi + TPOI::set(d-TIME_ERR_TOL/2, -2.0, 0);

		}

	}

	m_tpoi.Sort();
	m_tpoi.Purge();

/*
	  cout << GetName() << ",  D=" << GetDuration() << " TPOIS inserted!" << endl;
		for (int i=0; i < GetNumOfTPOIs(); ++i)
			cout << " " << m_tpoi.GetTime(i);
		cout << endl << endl;
*/

}

/***********************************************************/
string          AtomicSequence::GetInfo() {

	stringstream s;
	if ( m_alpha!=0.0  || m_theta!=0.0 || m_phi!=0.0 )
		s << " RotMtx(alpha,theta,phi) = (" << m_alpha << "," << m_theta << "," << m_phi << ")";

	return s.str();

}


/***********************************************************/
void AtomicSequence::CollectSeqData(NDData<double>& seqdata, double& t, long& offset) {

	bool rem  = this->HasNonLinGrad();
	this->SetNonLinGrad(false);
	for (int i=0; i < GetNumOfTPOIs(); ++i) {
		//cout << GetName() << " " << t << " " << m_tpoi.GetTime(i)+t << endl;
		seqdata(0,offset+i+1) = m_tpoi.GetTime(i) + t;
		seqdata(1,offset+i+1) = m_tpoi.GetPhase(i);
		GetValue(&seqdata(2,offset+i+1), m_tpoi.GetTime(i));
        GetValueLingeringEddyCurrents(&seqdata(2,offset+i+1), m_tpoi.GetTime(i));
		seqdata(7,offset+i+1) = m_tpoi.GetMask(i);
	}

    this->UpdateEddyCurrents();
    this->PrepareEddyCurrents();
    this->SetNonLinGrad(rem);

    //increase sequence time and TPOI offset
	t      += GetDuration();
	offset += GetNumOfTPOIs();

}


/***********************************************************/
long  AtomicSequence::GetNumOfADCs () {

	int iADC = GetNumOfTPOIs();
	for (int i=0; i<GetNumOfTPOIs(); ++i)
		if (m_tpoi.GetPhase(i) < 0.0)
			iADC--;
	return iADC;

}

/***********************************************************/
void    AtomicSequence::GetValueLingeringEddyCurrents (double * dAllVal, double const time) {

	World* pW = World::instance();
	multimap<EddyPulse*,double>::iterator iter;

	for(iter = pW->m_eddies.begin(); iter != pW->m_eddies.end(); iter++) {

		if ( iter->second < 0.0 )
			continue;

		if (HasNonLinGrad() && iter->first->HasNonLinGrad()) {
			iter->first->SetNonLinGradField(  iter->first->GetParentDuration()
											+ iter->first->GetLingerTime()
											- iter->second
											+ time  );
    	    continue;
    	}

		iter->first->GetValue(dAllVal,    iter->first->GetParentDuration()
										+ iter->first->GetLingerTime()
										- iter->second
										+ time );
	}

	Rotation(&dAllVal[2]);

}

/***********************************************************/
void AtomicSequence::UpdateEddyCurrents() {

	World* pW = World::instance();
	multimap<EddyPulse*,double>::iterator iter;

	//find eddy currents which are still alive
	multimap<EddyPulse*,double>	m_still_alive;
	for (iter = pW->m_eddies.begin(); iter != pW->m_eddies.end(); iter++ ) {
		if ( iter->second > GetDuration() )
			m_still_alive.insert(pair<EddyPulse*,double>(iter->first, iter->second - GetDuration() ));
		iter->second = -iter->first->GetLingerTime();
	}

	//check for nonlinear gradients
	bool b = HasNonLinGrad();
	for (iter = pW->m_eddies.begin(); iter != pW->m_eddies.end(); iter++ ) {
		if ( iter->first->HasNonLinGrad() ) { b=true; break;}
	}
	SetNonLinGrad(b);

	//add eddies which are still alive to the multimap
	for (iter = m_still_alive.begin(); iter != m_still_alive.end(); iter++)
		pW->m_eddies.insert(pair<EddyPulse*,double>(iter->first, iter->second) );


	//*/remove exact double entries from multimap (?)
	multimap<EddyPulse*,double>::iterator it1 = pW->m_eddies.begin();
	multimap<EddyPulse*,double>::iterator it2 = pW->m_eddies.begin();
	multimap<EddyPulse*,double>::iterator it3 = pW->m_eddies.begin();
	vector <multimap<EddyPulse*,double>::iterator> del;

	for (it1 = pW->m_eddies.begin(); it1 != pW->m_eddies.end(); it1++ ) {
		it3 = it1; it3++;
		if (it3 == pW->m_eddies.end() ) break;
		for (it2 = it3; it2!=pW->m_eddies.end(); it2++) {
			if ( it1->first == it2->first && it1->second == it2->second )	{
				bool b = true;
				for (unsigned int j=0; j<del.size() ; ++j)
					if ( del[j] == it1 ) {b=false; break;}
				if (b) del.push_back(it1);
			}
		}
	}
	for (unsigned int j=0; j<del.size() ; ++j) pW->m_eddies.erase(del[j]);


	//cout << "!!! " << pW->m_eddies.size() << endl << endl;
}

/***********************************************************/
void AtomicSequence::PrepareEddyCurrents() {

	if (!m_eddy) return;

	World* pW = World::instance();
	multimap<EddyPulse*,double>::iterator iter;
	vector<Module*> children = GetChildren();

	for (unsigned int j=0; j<children.size() ; ++j) {
		PulseAxis ax = ((Pulse*) children[j])->GetAxis();
		if ( ax==AXIS_RF || ax==AXIS_VOID ) continue;
		iter = pW->m_eddies.find(((EddyPulse*) children[j])); //OK - finds the first inserted EddyPulse
		if (iter == pW->m_eddies.end() ) continue;
		iter->second = iter->first->GetLingerTime();
	}

}


