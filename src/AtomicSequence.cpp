/** @file AtomicSequence.cpp
 *  @brief Implementation of JEMRIS AtomicSequence
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2022  Tony Stoecker
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

#include "AtomicSequence.h"
#include "GradPulse.h"
#include "TrapGradPulse.h"
#include "EddyPulse.h"
#include "RFPulse.h"

/***********************************************************/
AtomicSequence::AtomicSequence  (const AtomicSequence& as) :
	m_eddy(0), m_alpha(0.), m_phi(0.), m_theta(0.), m_non_lin_grad(false) {}

/***********************************************************/
bool    AtomicSequence::Prepare(const PrepareMode mode) {

	ATTRIBUTE("RotAngle"       , m_alpha );
	ATTRIBUTE("Inclination"    , m_theta );
	ATTRIBUTE("Azimuth"        , m_phi   );

	if (mode != PREP_UPDATE) m_type = MOD_ATOM;
	if (mode != PREP_UPDATE) GetDuration();
	bool tag = Sequence::Prepare(mode); //Prepare all pulses
	CollectTPOIs();  //of the pulses

	return tag;

}

/***********************************************************/
double AtomicSequence::GetDuration () {

	vector<Module*> children = GetChildren();
	double duration = 0.;

	for (unsigned int j=0; j<children.size() ; ++j) {
		if (children[j]->GetHardwareMode()<=0) {
			duration = fmax( duration, children[j]->GetDuration()+((Pulse*) children[j])->GetInitialDelay() );
		}
	}

	if (GetHardwareMode()>0)
		duration = 0;

	DEBUG_PRINT("  AtomicSequence::GetDuration() of " << GetName() <<
		    " calculates duration = " << dDuration << endl;)

	m_duration = duration;

	Notify(m_duration);

	return duration;
}

/***********************************************************/
inline void      AtomicSequence::GetValue (double * dAllVal, double const time) {

    if (time < 0.0 || time > m_duration) { return ; }

    if (m_non_lin_grad) World::instance()->NonLinGradField = 0.0;
    vector<Module*> children;
	children = ( m_eddy ? GetChildrenDynamic() : GetChildren() );

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
		if (p->GetHardwareMode()<=0) {
			d = p->GetInitialDelay();
			p->SetTPOIs();


			for (size_t i = 0; i < p->GetNumOfTPOIs(); ++i)  {

				m_tpoi	+ TPOI::set(d + p->GetTPOIs()->GetTime(i),
						p->GetTPOIs()->GetPhase(i), p->GetTPOIs()->GetMask(i));

				//one TPOI prior to the pulse in case of initial delay phase == -2.0 -> ReInit CVode
				if (d>TIME_ERR_TOL)
					m_tpoi + TPOI::set(d-TIME_ERR_TOL/2, -2.0, 0);

			}
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


/***********************************************************/
void AtomicSequence::CollectSeqData(NDData<double>& seqdata, double& t, long& offset) {

	World* pW = World::instance();

	//turn off nonlinear gradients for sequence diagram calculation
	SetNonLinGrad(false);
	CollectTPOIs();

    PrepareEddyCurrents();

	for (int i=0; i < GetNumOfTPOIs(); ++i) {
		seqdata(0,offset+i+1) = m_tpoi.GetTime(i) + t;
		seqdata(1,offset+i+1) = m_tpoi.GetPhase(i);
		GetValue(&seqdata(2,offset+i+1), m_tpoi.GetTime(i));
		//cout << GetName() << " " << setw(9) << setfill(' ') << m_tpoi.GetTime(i)+t << setw(9) << setfill(' ') << " " << seqdata(5,offset+i+1) << endl;
		if (pW->pStaticAtom != NULL) pW->pStaticAtom->GetValue( &seqdata(2,offset+i+1), m_tpoi.GetTime(i) + t );
        GetValueLingeringEddyCurrents(&seqdata(2,offset+i+1), m_tpoi.GetTime(i));
		seqdata(MAX_SEQ_VAL+1+2,offset+i+1) = m_tpoi.GetMask(i);

		if (seqdata.Dim(0) > MAX_SEQ_VAL+1+3){
			// set kspace counters
			seqdata(MAX_SEQ_VAL+1+3,offset+i+1) = pW->m_slice;
			seqdata(MAX_SEQ_VAL+1+4,offset+i+1) = pW->m_shot;
			seqdata(MAX_SEQ_VAL+1+5,offset+i+1) = pW->m_partition;
			seqdata(MAX_SEQ_VAL+1+6,offset+i+1) = pW->m_set;
			seqdata(MAX_SEQ_VAL+1+7,offset+i+1) = pW->m_contrast;
			seqdata(MAX_SEQ_VAL+1+8,offset+i+1) = pW->m_average;
		}

	}

	UpdateEddyCurrents();

    //increase sequence time and TPOI offset
	t      += GetDuration();
	offset += GetNumOfTPOIs();

}

/***********************************************************/
void AtomicSequence::CollectSeqData(OutputSequenceData *seqdata) {

	vector<Module*> children = GetChildren();
	Pulse* p;
	double d;

	vector<Event*> events;

	for (size_t j = 0; j < children.size(); ++j) {

		p = ((Pulse*)children[j]);
		d = p->GetInitialDelay();
		if (p->GetHardwareMode()>=0)
			p->GenerateEvents(events);

	}

	if (m_alpha!=0 || m_theta!=0 || m_phi!=0){
		// special case: for rotated gradients we need new arbitrary gradient events
		GradEvent *grad_x = new GradEvent();
		grad_x->m_channel = 0;
		grad_x->m_amplitude = std::numeric_limits<double>::min();
		GradEvent *grad_y = new GradEvent();
		grad_y->m_channel = 1;
		grad_y->m_amplitude = std::numeric_limits<double>::min();
		GradEvent *grad_z = new GradEvent();
		grad_z->m_channel = 2;
		grad_z->m_amplitude = std::numeric_limits<double>::min();
		int num_samples = round(GetDuration()/10.0e-3);
		for (int i=0; i<num_samples; i++){
			grad_x->m_samples.push_back(0.0);
			grad_y->m_samples.push_back(0.0);
			grad_z->m_samples.push_back(0.0);
		}
		double t;
		double gp_dur;
		GradPulse* gp;
		double grad_raster_time = 10.0e-3;

		// Each gradient is rotated on its own at the relative time point, all rotated gradients are added up
		for (size_t j = 0; j < children.size(); ++j) {
			p = ((Pulse*)children[j]);
			t = -1.0 * p->GetInitialDelay();
			if (p->GetAxis() > 0 && p->GetAxis() <= 3){
				gp = ((GradPulse*) p);
				gp_dur = gp->GetDuration();
				TrapGradPulse *tgp = dynamic_cast<TrapGradPulse*>(gp);
				if (tgp!=NULL) t += grad_raster_time/2; // For trapezoidal pulses we have to shift by half a raster time (5us) to maintain correct shape

				for (int i=0; i<num_samples; i++){
					double Grot[3] = {0,0,0};
					if (t>=0 && t<=gp_dur){
						// Rotation
						Grot[p->GetAxis()-AXIS_GX] = gp->GetGradient(t);
						Rotation(&Grot[0]);
						grad_x->m_samples[i] += Grot[0];
						grad_y->m_samples[i] += Grot[1];
						grad_z->m_samples[i] += Grot[2];

						// Update maximum amplitude
						grad_x->m_amplitude = (abs(grad_x->m_samples[i]) > abs(grad_x->m_amplitude)) ? grad_x->m_samples[i] : grad_x->m_amplitude;
						grad_y->m_amplitude = (abs(grad_y->m_samples[i]) > abs(grad_y->m_amplitude)) ? grad_y->m_samples[i] : grad_y->m_amplitude;
						grad_z->m_amplitude = (abs(grad_z->m_samples[i]) > abs(grad_z->m_amplitude)) ? grad_z->m_samples[i] : grad_z->m_amplitude;
					}
					t += grad_raster_time;
				}
			}
		}
		// compress shapes
		transform( grad_x->m_samples.begin(), grad_x->m_samples.end(), grad_x->m_samples.begin(), bind2nd( divides<double>(), grad_x->m_amplitude ) );
		transform( grad_y->m_samples.begin(), grad_y->m_samples.end(), grad_y->m_samples.begin(), bind2nd( divides<double>(), grad_y->m_amplitude ) );
		transform( grad_z->m_samples.begin(), grad_z->m_samples.end(), grad_z->m_samples.begin(), bind2nd( divides<double>(), grad_z->m_amplitude ) );

		// Delete old gradient events to not crowd the sequence file and add new events
		for(int i = 0; i < events.size(); ++i){
			GradEvent *grad = dynamic_cast<GradEvent*>(events[i]);
			if (grad!=NULL && grad->m_channel<3)
				events.erase(events.begin()+i--);
		}
		events.push_back(grad_x);
		events.push_back(grad_y);
		events.push_back(grad_z);

	}

	seqdata->AddEvents(events, GetDuration());
	for(int i = 0; i < events.size(); ++i)
	   delete events[i];

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

		if ( iter->second < 1e-16 )
			continue;

		if (HasNonLinGrad() && iter->first->HasNonLinGrad()) {
			iter->first->SetNonLinGradField(  iter->first->GetParentDuration()
											+ iter->first->GetLingerTime()
											- iter->second
											+ time  );
    	    continue;
    	}

		double d[MAX_SEQ_VAL+3] = {0.0};	/* Extra two values (time, rx phase) */

		iter->first->GetValue(d,    iter->first->GetParentDuration()
										+ iter->first->GetLingerTime()
										- iter->second
										+ time );

		iter->first->GetParentAtom()->Rotation(&d[GRAD_X]);
		for (int i=GRAD_X;i<=MAX_SEQ_VAL;++i) dAllVal[i] += d[i];
	}


}

/***********************************************************/
void AtomicSequence::PrepareEddyCurrents() {

	if (!m_eddy) return;

	World* pW = World::instance();
	multimap<EddyPulse*,double>::iterator iter;
	vector<Module*> children = GetChildrenDynamic();

	// find my eddies
	for (unsigned int j=0; j<children.size() ; ++j) {
		PulseAxis ax = ((Pulse*) children[j])->GetAxis();
		if ( ax==AXIS_RF || ax==AXIS_VOID ) continue;
		iter = pW->m_eddies.find(((EddyPulse*) children[j])); //OK - finds the first inserted EddyPulse
		if (iter == pW->m_eddies.end() ) continue;
		iter->first->Convolve();
	}

}
/***********************************************************/
void AtomicSequence::UpdateEddyCurrents() {

	// reduce linger time for all eddies
	World* pW = World::instance();
	multimap<EddyPulse*,double>::iterator iter;
	for (iter = pW->m_eddies.begin(); iter != pW->m_eddies.end(); iter++ )
		iter->second -= GetDuration();

	// set linger time for my eddies
	if (!m_eddy) return;
	vector<Module*> children = GetChildrenDynamic();

	for (unsigned int j=0; j<children.size() ; ++j) {
		PulseAxis ax = ((Pulse*) children[j])->GetAxis();
		if ( ax==AXIS_RF || ax==AXIS_VOID ) continue;
		iter = pW->m_eddies.find(((EddyPulse*) children[j])); //OK - finds the first inserted EddyPulse
		if (iter == pW->m_eddies.end() ) continue;
		iter->second = iter->first->GetLingerTime();
	}

}
