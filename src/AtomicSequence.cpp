/** @file AtomicSequence.cpp
 *  @brief Implementation of JEMRIS AtomicSequence
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

#include "AtomicSequence.h"
#include "GradPulse.h"

/***********************************************************/
AtomicSequence::AtomicSequence  (const AtomicSequence& as) {

	m_alpha         = 0.0;
	m_theta         = 0.0;
	m_phi           = 0.0;
	m_non_lin_grad  = false;

};

/***********************************************************/
bool    AtomicSequence::Prepare(PrepareMode mode) {

	ATTRIBUTE("RotAngle"       , &m_alpha );
	ATTRIBUTE("Inclination"    , &m_theta );
	ATTRIBUTE("Azimut"         , &m_phi   );

	if (mode != PREP_UPDATE) m_type = MOD_ATOM;

	bool tag = Sequence::Prepare(mode); //Prepare all pulses
	CollectTPOIs();  //of the pulses

	return tag;

};

/***********************************************************/
double     AtomicSequence::GetDuration  () {

	double dDuration = 0.0;
	vector<Module*> children = GetChildren();

	for (unsigned int j=0; j<children.size() ; ++j)
		dDuration = fmax( dDuration, children[j]->GetDuration()+((Pulse*) children[j])->GetInitialDelay() );

	m_duration = dDuration;
	DEBUG_PRINT("  AtomicSequence::GetDuration() of " << GetName() <<
		    " calculates duration = " << dDuration << endl;)

	Notify(&m_duration);

	return( dDuration );

};

/***********************************************************/
void      AtomicSequence::GetValue (double * dAllVal, double const time) {

    if (time < 0.0 || time > GetDuration()) { return ; }

    if (m_non_lin_grad) m_world->NonLinGradField = 0.0;
    vector<Module*> children = GetChildren();

    for (unsigned int j=0; j<children.size() ; ++j) {

    	if ( ((Pulse*) children[j])->GetAxis()==AXIS_VOID ) continue;
    	double pulse_time = time - ((Pulse*) children[j])->GetInitialDelay();
    	if (pulse_time < 0.0) continue;
    	if (m_non_lin_grad && ((Pulse*) children[j])->GetAxis()!=AXIS_RF && ((GradPulse*) children[j])->HasNonLinGrad())
    	    ((GradPulse*) children[j])->SetNonLinGradField(pulse_time);
    	else
            children[j]->GetValue(dAllVal,pulse_time);
    }

    Rotation(&dAllVal[2]);

};

/***********************************************************/
void AtomicSequence::Rotation (double * Grot) {

    if ( m_alpha == 0.0 ) return;

    double Gx = Grot[0];
    double Gy = Grot[1];
    double Gz = Grot[2];

    double alpha = m_alpha * PI/180.0;
    double theta = m_theta * PI/180.0;
    double phi   = m_phi   * PI/180.0;

    Grot[0] = ((cos(phi)*(pow(cos(theta),2.0)*cos(alpha)+pow(sin(theta),2.0))-sin(phi)*cos(theta)*sin(alpha))*cos(phi)+(cos(phi)*cos(theta)*sin(alpha)+sin(phi)*cos(alpha))*sin(phi))*Gx+(-(cos(phi)*(pow(cos(theta),2.0)*cos(alpha)+pow(sin(theta),2.0))-sin(phi)*cos(theta)*sin(alpha))*sin(phi)+(cos(phi)*cos(theta)*sin(alpha)+sin(phi)*cos(alpha))*cos(phi))*Gy+(cos(phi)*(-cos(theta)*cos(alpha)*sin(theta)+sin(theta)*cos(theta))+sin(phi)*sin(theta)*sin(alpha))*Gz;

    Grot[1] = ((-sin(phi)*(pow(cos(theta),2.0)*cos(alpha)+pow(sin(theta),2.0))-cos(phi)*cos(theta)*sin(alpha))*cos(phi)+(-sin(phi)*cos(theta)*sin(alpha)+cos(phi)*cos(alpha))*sin(phi))*Gx+(-(-sin(phi)*(pow(cos(theta),2.0)*cos(alpha)+pow(sin(theta),2.0))-cos(phi)*cos(theta)*sin(alpha))*sin(phi)+(-sin(phi)*cos(theta)*sin(alpha)+cos(phi)*cos(alpha))*cos(phi))*Gy+(-sin(phi)*(-cos(theta)*cos(alpha)*sin(theta)+sin(theta)*cos(theta))+cos(phi)*sin(theta)*sin(alpha))*Gz;

    Grot[2] = (cos(phi)*(-cos(theta)*cos(alpha)*sin(theta)+sin(theta)*cos(theta))-sin(phi)*sin(theta)*sin(alpha))*Gx+(-sin(phi)*(-cos(theta)*cos(alpha)*sin(theta)+sin(theta)*cos(theta))-cos(phi)*sin(theta)*sin(alpha))*Gy+(pow(sin(theta),2.0)*cos(alpha)+pow(cos(theta),2.0))*Gz;

    return;

}
/***********************************************************/
void AtomicSequence::CollectTPOIs() {

	vector<Module*> children = GetChildren();

	m_tpoi.Reset();

	for (unsigned int j = 0; j < children.size(); ++j) {

		Pulse* p = ((Pulse*)children[j]);
		p->SetTPOIs();
		for (unsigned int i=0; i<p->GetNumOfTPOIs(); ++i)  {

			double d = p->GetInitialDelay();
			m_tpoi	+ TPOI::set(d+p->GetTPOIs()->GetTime(i), p->GetTPOIs()->GetPhase(i));
			//one TPOI prior to the pulse in case of intial delay
			//phase == -2.0 -> ReInit CVode
			if (d>TIME_ERR_TOL) m_tpoi + TPOI::set(d-TIME_ERR_TOL/2, -2.0);
		}

	}

	m_tpoi.Sort();
	m_tpoi.Purge();

};
/***********************************************************/
string          AtomicSequence::GetInfo() {

	stringstream s;
	if ( m_alpha!=0.0  || m_theta!=0.0 || m_phi!=0.0 )
		s << " RotMtx(alpha,theta,phi) = (" << m_alpha << "," << m_theta << "," << m_phi << ")";

	return s.str();

};
