/** @file RFPulse.cpp
 *  @brief Implementation of JEMRIS RFPulse
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

#include "RFPulse.h"
#include "Coil.h"

#include <limits>
/*****************************************************************/
RFPulse::RFPulse  () : m_refocusing(0), m_coil_array(0),
	m_channel(0), m_flip_angle(0.), m_bw(0.), m_symmetry(.5) {

	m_axis = AXIS_RF;
	m_initial_phase = 0.;

}

/*****************************************************************/
bool RFPulse::Prepare  (const PrepareMode mode) {

	//every RFPulse might have FlipAngle, InitialPhase, Bandwidth, and Frequency offset
	ATTRIBUTE("FlipAngle"    , m_flip_angle   );
	ATTRIBUTE("InitialPhase" , m_initial_phase);
	ATTRIBUTE("Bandwidth"    , m_bw           );
	ATTRIBUTE("Frequency"    , m_frequency    );
	ATTRIBUTE("Channel"      , m_channel      );
	ATTRIBUTE("Refocusing"   , m_refocusing   );
	ATTRIBUTE("Symmetry"     , m_symmetry     );

    if (mode != PREP_UPDATE)
        insertGetPhaseFunction( &TxRxPhase::getLinearPhase );

    // Prepare up the chain
    bool b = Pulse::Prepare(mode);

    if (mode != PREP_UPDATE) {
        HideAttribute("Axis",false);
    	HideAttribute("PhaseLock",false);
    }

    if (b && (m_symmetry <= 0. || m_symmetry > 1.)) {
    	cout << "Preparation of DelayAtomicSequence '" << GetName() << "' not successful. Symmetry = "
    			<< m_symmetry << ". Must be (0.,1.]." << endl;
    	b = false;
    }

	return b;

}

/*****************************************************************/
void RFPulse::insertGetPhaseFunction (double (*FGetPhase)(Module*, double)) {

    if (find( m_GetPhaseFunPtrs.begin(), m_GetPhaseFunPtrs.end(), FGetPhase) == m_GetPhaseFunPtrs.end())
	    m_GetPhaseFunPtrs.push_back(FGetPhase);

    return;

}

void RFPulse::SetTPOIs() {
	size_t bitmask = (m_refocusing) ? BIT(REFOCUS_T) : BIT(EXCITE_T);
	Pulse::SetTPOIs();
	m_tpoi + TPOI::set (m_symmetry * GetDuration(), -1., bitmask);
}
/*****************************************************************/
inline void RFPulse::GenerateEvents(std::vector<Event*> &events) {
	RFEvent *rf = new RFEvent();
	double p = GetInitialPhase()*PI/180.0;
	p = fmod( p, 2*PI );
	p = p<0.0 ? p+2*PI : p;
	p = round(p*1.0e5)/1.0e5;
	rf->m_phase_offset = p;
	rf->m_freq_offset = GetFrequency();

	// Fill the initial delay period with zeros
	int num_initial_samples = round(GetInitialDelay()*1.0e3);
	rf->m_magnitude.insert (rf->m_magnitude.begin(),num_initial_samples,0.0);
	rf->m_phase.insert (rf->m_phase.begin(),num_initial_samples,0.0);

	double max_magnitude = std::numeric_limits<double>::min();
	double magn, mag1, mag2;
	double phase = 0.0;

	// Linear interpolation allows courser sampling of the RF waveform.
	// Although the generated waveform remains sampled at 1us, the linear
	// segments allow compression when output for scanner hardware.
	int interpFactor=10;	// Linear segments of 10us (interpolate if factor > 1)

	// Calculate samples
	for (double time=0.5e-3; time<GetDuration(); )
	{
		// Linear interpolation between time points to reduce the size of the RF shape
		mag1 = GetMagnitude(time);
		mag2 = GetMagnitude(time + interpFactor*1.0e-3);
		for (int i=0; i<interpFactor && time<GetDuration(); i++, time+=1.0e-3)
		{
			magn  = ((interpFactor-i)*mag1 + i*mag2)/interpFactor;

			// Add additional phase from pulse shape, but not from linear phase due to frequency offset
			for (unsigned int i=0; i<m_GetPhaseFunPtrs.size(); ++i) {
				if (m_GetPhaseFunPtrs[i]!=&TxRxPhase::getLinearPhase)
					phase += m_GetPhaseFunPtrs[i](this,time)*PI/180.0;
			}

			// Check if magnitude is negative
			if (magn<0) {
				phase += PI;
				magn=-magn;
			}
			phase = fmod( phase, 2*PI );
			phase = (phase<0.0?phase+2*PI:phase);
			phase /= (2*PI);

			if (magn>max_magnitude)
				max_magnitude=magn;

			// Append samples
			rf->m_magnitude.push_back(magn);
			rf->m_phase.push_back(phase);
		}
	}

	// Normalize waveform to be in range [0,1]
	transform( rf->m_magnitude.begin(), rf->m_magnitude.end(), rf->m_magnitude.begin(), bind2nd( divides<double>(), max_magnitude ) );
	rf->m_amplitude = max_magnitude;

	events.push_back(rf);
}


/*****************************************************************/
void RFPulse::GetValue (double * dAllVal, double const time)  {

    if (time < 0.0 || time > GetDuration())
        return;

	// Get Magnitude and Phase from the B1 sensitivity map
	double magn  = 1.0;
	double phase = 0.0;

	if (m_coil_array != NULL) {

		Coil* coil=m_coil_array->GetCoil(m_channel);

		if (coil != NULL) {
			magn  = coil->GetSensitivity(World::instance()->total_time + time);
			phase = coil->GetPhase(World::instance()->total_time + time);
		} else
			cout << GetName() << " warning: my channel" << m_channel << "is not in the TxCoilArray\n";

	}

	// Get Magnitude and Phase of this RF pulse
	magn  *= GetMagnitude(time);
	phase += GetInitialPhase()*PI/180.0;

	for (unsigned int i=0; i<m_GetPhaseFunPtrs.size(); ++i)
		phase += m_GetPhaseFunPtrs[i](this,time)*PI/180.0;

	phase = fmod( phase, 2*PI );
	World::instance()->PhaseLock = (phase<0.0?phase+2*PI:phase);

	//add RFPulse to the B1 field
	double B1x =  (dAllVal[0]*cos(dAllVal[1]) + magn*cos(phase));
	double B1y =  (dAllVal[0]*sin(dAllVal[1]) + magn*sin(phase));

	dAllVal[0] = sqrt(pow(B1x,2.0) + pow(B1y,2.0));
	dAllVal[1] = atan2(B1y,B1x);

}

/***********************************************************/
double RFPulse::GetIntegralNumeric (int steps) {

    double Sum    = 0.0;
    double DeltaT = GetDuration()/steps;

    for (int i=0; i<steps; ++i )
		Sum += GetMagnitude(i*DeltaT) ;

    return (Sum*DeltaT) ;

}

/***********************************************************/
string          RFPulse::GetInfo () {

	stringstream s;
	s << Pulse::GetInfo() << " , (Flipangle,Phase,Bandwidth,Channel,Symmetry) = ("
			<< m_flip_angle << "," << GetInitialPhase() << "," << m_bw << "," << m_channel << "," << m_symmetry << ") ";
	return s.str();

}
