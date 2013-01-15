/** @file ExternalRFPulse.cpp
 *  @brief Implementation of JEMRIS ExternalRFPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

#include "ExternalRFPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalRFPulse::ExternalRFPulse  (const ExternalRFPulse& hrfp) {

    m_scale=1.0;
    m_fname="";

}

/***********************************************************/
bool ExternalRFPulse::Prepare  (PrepareMode mode) {

	m_bw  = 1e16;

	ATTRIBUTE("Filename", m_fname );
	ATTRIBUTE("Dataname", m_dname );
	ATTRIBUTE("Scale"   , m_scale );

        //read file only once !
	if (mode == PREP_INIT && !m_fname.empty() ) {

		BinaryContext bc;
		DataInfo      di;
		
		m_times.clear();
		m_phases.clear();
		m_magnitudes.clear();

		di.fname = m_fname;
		bc.SetInfo(di);
		
		di = bc.GetInfo (m_dname);
		
		m_samples = di.GetSize()/3;
		
		double* tmp = (double*) malloc (di.GetSize() * sizeof (double));
		
		bc.ReadData (tmp);
		
        for (int i = 0; i < m_samples; i++) {
			
            m_times.push_back      (tmp[i + 0 * m_samples]);
            m_magnitudes.push_back (tmp[i + 1 * m_samples]);
            m_phases.push_back     (tmp[i + 2 * m_samples]);
			
        }
		
		free (tmp);
		
		SetDuration(m_times.at(m_samples-1));
		
	}

	bool b = RFPulse::Prepare(mode);

	//remove Bandwidth from XML attributes
	if (mode != PREP_UPDATE) HideAttribute ("Bandwidth",false);

	return b;

}

/***********************************************************/
inline void ExternalRFPulse::SetTPOIs  () {

    m_tpoi.Reset();

    for (unsigned int i=0; i<m_times.size(); ++i)
		m_tpoi + TPOI::set(m_times.at(i), -1.0);

    for (unsigned i = 0; i < GetNADC(); i++)
		m_tpoi + TPOI::set(i*GetDuration()/GetNADC(), (Pulse::m_phase_lock?World::instance()->PhaseLock:0.0) );

}

/***********************************************************/
double    ExternalRFPulse::GetMagnitude  (double time ) {

	unsigned i = ((unsigned) ( time * m_times.size() / GetDuration() ) );

	if (i<m_magnitudes.size()) {
		SetInitialPhase ((180.0/PI)*m_phases.at(i));
		return m_scale*m_magnitudes.at(i);
	}

	return 0.0;

}

/***********************************************************/
string          ExternalRFPulse::GetInfo() {

	stringstream s;
	s << RFPulse::GetInfo() << " , (Filename,Energy,TPOIs) = (" << m_fname << "," << GetIntegralNumeric((int)GetDuration()*1000) << "," << m_tpoi.GetSize() <<")";

	return s.str();

}
