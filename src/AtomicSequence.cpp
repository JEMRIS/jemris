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
#include "RFPulse.h"

/***********************************************************/
AtomicSequence::AtomicSequence  (const AtomicSequence& as) {

	m_alpha         = 0.0;
	m_theta         = 0.0;
	m_phi           = 0.0;
	m_non_lin_grad  = false;

}

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
    	if (m_non_lin_grad && ((Pulse*) children[j])->GetAxis()!=AXIS_RF && ((GradPulse*) children[j])->HasNonLinGrad()) {
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

    if ( m_alpha == 0.0 ) return;

    double Gx = Grot[0];
    double Gy = Grot[1];
    double Gz = Grot[2];

    double pi_180 = PI/180.0;
    double alpha = m_alpha * pi_180;
    double theta = m_theta * pi_180;
    double phi   = m_phi   * pi_180;

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    double cos_2_theta = cos_theta * cos_theta;
    double sin_2_theta = sin_theta * sin_theta;
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);
    
    Grot[0] =
        ((cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)*cos_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*sin_phi)*Gx+
        (-(cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)*sin_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*cos_phi)*Gy+
        (cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+sin_phi*sin_theta*sin_alpha)*Gz;

    Grot[1] =
        ((-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)*cos_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*sin_phi)*Gx+
        (-(-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)*sin_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*cos_phi)*Gy+
        (-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+cos_phi*sin_theta*sin_alpha)*Gz;

    Grot[2] = (cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-sin_phi*sin_theta*sin_alpha)*Gx+
        (-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-cos_phi*sin_theta*sin_alpha)*Gy+
        (sin_2_theta*cos_alpha+cos_2_theta)*Gz;

    return;

}
/***********************************************************/
void AtomicSequence::CollectTPOIs() {

	vector<Module*> children = GetChildren();

	m_tpoi.Reset();

	for (unsigned int j = 0; j < children.size(); ++j) {

		Pulse* p = ((Pulse*)children[j]);
		p->SetTPOIs();
		for (int i=0; i<p->GetNumOfTPOIs(); ++i)  {

			double d = p->GetInitialDelay();
			m_tpoi	+ TPOI::set(d+p->GetTPOIs()->GetTime(i), p->GetTPOIs()->GetPhase(i));
			//one TPOI prior to the pulse in case of intial delay
			//phase == -2.0 -> ReInit CVode
			if (d>TIME_ERR_TOL) m_tpoi + TPOI::set(d-TIME_ERR_TOL/2, -2.0);
		}

	}

	m_tpoi.Sort();
	m_tpoi.Purge();

}
/***********************************************************/
string          AtomicSequence::GetInfo() {

	stringstream s;
	if ( m_alpha!=0.0  || m_theta!=0.0 || m_phi!=0.0 )
		s << " RotMtx(alpha,theta,phi) = (" << m_alpha << "," << m_theta << "," << m_phi << ")";

	return s.str();

}
