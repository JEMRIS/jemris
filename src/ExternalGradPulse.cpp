/** @file ExternalGradPulse.cpp
 *  @brief Implementation of JEMRIS ExternalGradPulse
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

#include "ExternalGradPulse.h"
#include "BinaryContext.h"

/***********************************************************/
double            ExternalGradPulse::GetGradient (double const time)  {

    unsigned i = ((unsigned) (time * m_times.size() / GetDuration()));

    if (i<m_magnitudes.size())
        return (m_gain*m_magnitudes.at(i));

    return 0.0;
}

/***********************************************************/
bool              ExternalGradPulse::Prepare     (PrepareMode mode)   {

    bool          btag = true;
	BinaryContext bc;
	DataInfo      di;


    ATTRIBUTE("Filename" , m_fname);
    ATTRIBUTE("Dataname" , m_dname);
    ATTRIBUTE("Scale"    , m_gain);

    //read file only once !
    if (mode == PREP_INIT && !m_fname.empty() ) {

        m_times.clear();
        m_magnitudes.clear();
		
		di.fname = m_fname;
		bc.SetInfo(di);
		
		di = bc.GetInfo (m_dname);
		
		m_samples = di.GetSize()/2;
		
		double* tmp = (double*) malloc (di.GetSize() * sizeof (double));
		
		bc.ReadData (tmp);
		
        for (int i = 0; i < m_samples; i++) {
			
            m_times.push_back      (tmp[i]            );
            m_magnitudes.push_back (tmp[i + m_samples]);
			
        }
		
		free (tmp);
		
		SetDuration(m_times.at(m_samples-1));
		
    }
	
    m_area = GetAreaNumeric(m_tpoi.GetSize());
	
    btag = (GradPulse::Prepare(mode) && btag);
	
    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;
	
    return btag;
	
}

/***********************************************************/
inline void  ExternalGradPulse::SetTPOIs () {

    m_tpoi.Reset();

    for (unsigned int i=0; i<m_times.size(); ++i)
	m_tpoi + TPOI::set(m_times.at(i), -1.0);

    for (unsigned int i = 0; i < GetNADC(); i++)
        m_tpoi + TPOI::set(i*GetDuration()/GetNADC(), (Pulse::m_phase_lock?World::instance()->PhaseLock:0.0));

}

/***********************************************************/
string          ExternalGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , binary file = " << m_fname ;

	return s.str();

}
