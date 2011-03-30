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

/***********************************************************/
ExternalRFPulse::ExternalRFPulse  (const ExternalRFPulse& hrfp) {

    m_scale=1.0;
    m_fname="";
    m_fname_old="";

};

/***********************************************************/
bool ExternalRFPulse::Prepare  (PrepareMode mode) {

	bool btag = true;
	m_bw  = 1e16;

	ATTRIBUTE("Filename", m_fname );
	ATTRIBUTE("Scale"   , m_scale );


        //read data if filename changed
	if (m_fname != m_fname_old)
	{
		//read TPOIs and B1-vaules from file
	    ifstream fin(m_fname.c_str(), ios::binary);
	    
	    if (fin.is_open()) {

		m_fname_old = m_fname;
		
		double dTP, dVal;
		fin.read((char *)(&(dTP)), sizeof(double));
		int iNumberOfTimePoints = ((int) dTP);

		m_times.clear();
		m_phases.clear();
		m_magnitudes.clear();
		m_tpoi.Reset();

		for (int i=0; i<iNumberOfTimePoints; ++i) {

		    fin.read((char *)(&dTP), sizeof(double));
			m_times.push_back( dTP );
			m_tpoi + TPOI::set(dTP,           -1.0);

		    fin.read((char *)(&dVal), sizeof(double));
		    m_magnitudes.push_back( dVal);

		    fin.read((char *)(&dVal), sizeof(double));
		    m_phases.push_back( dVal );
		}
		SetDuration(dTP);
		fin.close();
		//cout << "Success in Module " << GetName() << ": ExternalRFPulse::Prepare read RF binary file " << m_fname << endl;
	    }
	    else {
	      btag = false;
	      if (mode == PREP_UPDATE)
		cout << "Error in Module " << GetName() << ": ExternalRFPulse::Prepare can not read RF binary file " << m_fname << endl;
	    }
	}

	//attributes not needed in XML 
	if (mode != PREP_UPDATE) { HideAttribute ("Bandwidth",false); HideAttribute ("Duration"); }

	btag = (RFPulse::Prepare(mode) && btag); 
	if (!btag && mode == PREP_VERBOSE)
		cout << "\n warning in Prepare(1) of ExternalRFPulse " << GetName() << endl;

	return btag;

};

/***********************************************************/
inline void ExternalRFPulse::SetTPOIs  () {

    m_tpoi.Reset();

    for (unsigned int i=0; i<m_times.size(); ++i)
		m_tpoi + TPOI::set(m_times.at(i), -1.0);

    for (unsigned i = 0; i < GetNADC(); i++)
		m_tpoi + TPOI::set(i*GetDuration()/GetNADC(), (Pulse::m_phase_lock?World::instance()->PhaseLock:0.0) );

};

/***********************************************************/
double    ExternalRFPulse::GetMagnitude  (double time ) {

	unsigned i = ((unsigned) ( time * m_times.size() / GetDuration() ) );

	if (i<m_magnitudes.size()) {
		SetInitialPhase ((180.0/PI)*m_phases.at(i));
		return m_scale*m_magnitudes.at(i);
	}

	return 0.0;

};

/***********************************************************/
string          ExternalRFPulse::GetInfo() {

	stringstream s;
	s << RFPulse::GetInfo() << " , (Filename,Energy,TPOIs) = (" << m_fname << "," << GetIntegralNumeric((int)GetDuration()*1000) << "," << m_tpoi.GetSize() <<")";

	return s.str();

};
