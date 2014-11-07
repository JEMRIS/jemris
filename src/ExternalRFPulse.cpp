/** @file ExternalRFPulse.cpp
 *  @brief Implementation of JEMRIS ExternalRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#include "ExternalRFPulse.h"

/***********************************************************/
ExternalRFPulse::ExternalRFPulse  (const ExternalRFPulse& hrfp) {

    m_scale=1.0;
    m_fname="";
    m_pulse_data.SetPulse(this);
    m_interp=false;

};

/***********************************************************/
bool ExternalRFPulse::Prepare  (const PrepareMode mode) {

	m_bw  = 1e16;

	ATTRIBUTE ("Scale"   , m_scale);
	ATTRIBUTE ("Filename", m_fname);
    ATTRIBUTE("Interpolate", m_interp);

	//read data
	bool btag = m_pulse_data.ReadPulseShape (m_fname, mode == PREP_UPDATE) ;

	m_pulse_data.SetInterp(m_interp);

	if (mode != PREP_UPDATE) insertGetPhaseFunction( &ExternalPulseData::GetPhase );

	btag = ( RFPulse::Prepare(mode) && btag);

	if (mode != PREP_UPDATE)
		HideAttribute ("Bandwidth", false);
    
	if (!btag && mode == PREP_VERBOSE)
		cout	<< "\n warning in Prepare(1) of ExternalRFPulse " << GetName()
				<< " : can not read binary file " << m_fname << endl;

	return btag;

};

/***********************************************************/
string          ExternalRFPulse::GetInfo() {

	stringstream s;
	s << RFPulse::GetInfo() << " , (Filename,Energy,TPOIs) = (" << m_fname << "," << GetIntegralNumeric((int)GetDuration()*1000) << "," << m_tpoi.GetSize() <<")";

	return s.str();

};
