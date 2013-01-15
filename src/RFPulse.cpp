/** @file RFPulse.cpp
 *  @brief Implementation of JEMRIS RFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
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

#include "RFPulse.h"
#include "Coil.h"

/*****************************************************************/
RFPulse::RFPulse  () {

    m_axis          = AXIS_RF;
    m_coil_array    = NULL;
    m_channel       =0;
    m_flip_angle    =0.0;
    m_bw            =0.0;
    m_initial_phase =0.0;

}

/*****************************************************************/
bool RFPulse::Prepare  (PrepareMode mode) {

	//every RFPulse might have FlipAngle, InitialPhase, Bandwidth, and Frequncy offset
	ATTRIBUTE("FlipAngle"    , m_flip_angle   );
	ATTRIBUTE("InitialPhase" , m_initial_phase);
	ATTRIBUTE("Bandwidth"    , m_bw           );
	ATTRIBUTE("Frequency"    , m_frequency    );
	ATTRIBUTE("Channel"      , m_channel      );

    if (mode != PREP_UPDATE)
        insertGetPhaseFunction( &TxRxPhase::getLinearPhase );

    // Prepare up the chain
    bool b = Pulse::Prepare(mode);

    if (mode != PREP_UPDATE) {
        HideAttribute("Axis",false);
    	HideAttribute("PhaseLock",false);
    }

	return b;

}

/*****************************************************************/
void    RFPulse::insertGetPhaseFunction(double (*FGetPhase)(Module*, double)) {

    if (find( m_GetPhaseFunPtrs.begin(), m_GetPhaseFunPtrs.end(), FGetPhase) == m_GetPhaseFunPtrs.end())
	    m_GetPhaseFunPtrs.push_back(FGetPhase);

    return;

}

/*****************************************************************/
void RFPulse::GetValue (double * dAllVal, double const time) {

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
		}
		else
			cout << GetName() << " warning: my channel" << m_channel << "is not in the TxCoilArray\n";

	}

	// Get Magntidue and Phase of this RF pulse
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
	s << Pulse::GetInfo() << " , (Flipangle,Phase,Bandwidth,Channel) = (" << m_flip_angle << "," << GetInitialPhase() << "," << m_bw << "," << m_channel << ") ";
	return s.str();

}
