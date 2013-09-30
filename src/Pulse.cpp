/** @file Pulse.cpp
 *  @brief Implementation of JEMRIS Pulse
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

#include "Pulse.h"
#include "SequenceTree.h"

/***********************************************************/
Pulse::Pulse  () {

	m_axis              = AXIS_VOID;
	m_adc               = 0;
	m_initial_delay     = 0.0;
	m_phase_lock        = false;

}

/***********************************************************/
bool Pulse::Prepare  (PrepareMode mode) {

	m_type = MOD_PULSE;

	//every Pulse might has Axis, Duration, ADCs, and an initial delay
	ATTRIBUTE ("Axis"        , m_axis         );
	ATTRIBUTE ("ADCs"        , m_adc          );
	ATTRIBUTE ("PhaseLock"   , m_phase_lock   );
	ATTRIBUTE ("InitialDelay", m_initial_delay);

	return Module::Prepare(mode);

}


/***********************************************************/
inline void  Pulse::SetTPOIs () {

    m_tpoi.Reset();
    if (GetDuration() == 0.0) return;

    m_tpoi + TPOI::set(TIME_ERR_TOL, -1.0);
    m_tpoi + TPOI::set(GetDuration()-TIME_ERR_TOL, -1.0);

    for (unsigned i = 0; i < GetNADC(); i++)
    	m_tpoi + TPOI::set((i+1)*GetDuration()/(GetNADC()+1), (m_phase_lock?World::instance()->PhaseLock:0.0) );

    if ( GetParent() != NULL && GetParent()->GetDuration()>GetDuration() )
    	m_tpoi + TPOI::set(GetDuration()+TIME_ERR_TOL, -1.0);

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

	return s.str();

}
