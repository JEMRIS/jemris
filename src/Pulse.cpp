/** @file Pulse.cpp
 *  @brief Implementation of JEMRIS Pulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
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

#include "Pulse.h"
#include "SequenceTree.h"

/***********************************************************/
Pulse::Pulse  () {

	m_axis              = AXIS_VOID;
	m_adc               = 0;
	m_adc_flag			= 1;
	m_initial_delay     = 0.0;
	m_phase_lock        = false;

}

/***********************************************************/
bool Pulse::Prepare  (const PrepareMode mode) {

	m_type = MOD_PULSE;

	//every Pulse might has Axis, Duration, ADCs, and an initial delay
	ATTRIBUTE ("Axis"        , m_axis         );
	ATTRIBUTE ("ADCs"        , m_adc          );
	ATTRIBUTE ("ADCFlag"     , m_adc_flag     );
	ATTRIBUTE ("PhaseLock"   , m_phase_lock   );
	ATTRIBUTE ("InitialDelay", m_initial_delay);
	ATTRIBUTE ("InitialPhase", m_initial_phase);
	ATTRIBUTE ("Frequency"   , m_frequency    );

	bool btag = Module::Prepare(mode);

	if (mode == PREP_VERBOSE && m_adc<0) 
		   cout << "\n warning in Prepare(1) of PULSE " << GetName() << ": negative ADCs are no longer supported and will be ignored" << endl;

	m_adc=abs(m_adc);

	return btag;

}


/***********************************************************/
inline void  Pulse::SetTPOIs () {

    m_tpoi.Reset();

    m_tpoi + TPOI::set(TIME_ERR_TOL, -1.0, 0);
    m_tpoi + TPOI::set(GetDuration()-TIME_ERR_TOL, -1.0, 0);

    double p = (m_phase_lock?World::instance()->PhaseLock:0.0);

	if (m_adc_flag == 0) p=-1.0;

    for (int i = 0; i < m_adc; i++)
    	m_tpoi + TPOI::set((i+0.5)*GetDuration()/m_adc, p, m_adc_flag );

	// cout << GetName() << " m_adc_flag = " << m_adc_flag << ", N = " << m_adc << endl; m_tpoi.PrintMeta(2); cout << endl;

}

/***********************************************************/
void Pulse::SetDuration (double val) {

    if (val<0.0) return;

    m_duration=val;

}


/***********************************************************/
string          Pulse::GetInfo() {

	string ret;
	switch (m_axis) {
		case AXIS_RF : ret=" Axis = RF "; break;
		case AXIS_GX : ret=" Axis = GX "; break;
		case AXIS_GY : ret=" Axis = GY "; break;
		case AXIS_GZ : ret=" Axis = GZ "; break;
		default: ret=" Axis = none ";
	}

	stringstream s;
	s << ret;
	if (m_initial_delay>0.0) s << " , InitDelay = " << m_initial_delay;
	if (m_adc_flag>0 && m_adc>0){
		s << " , ADC type ADC|IMG|ACS|PC|NOISE = "
	  	  << m_tpoi.IsADC(2) << "|" << m_tpoi.IsImg(2) << "|"<< m_tpoi.IsACS(2) << "|"<< m_tpoi.IsPC(2) << "|"<< m_tpoi.IsNoise(2);
	}
	return s.str();

}
