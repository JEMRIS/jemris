/** @file ExternalRFPulse.cpp
 *  @brief Implementation of JEMRIS ExternalRFPulse
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

#include "ExternalRFPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalRFPulse::ExternalRFPulse  (const ExternalRFPulse& hrfp) {

    m_scale=1.0;
    m_fname="";

};

/***********************************************************/
bool ExternalRFPulse::Prepare  (const PrepareMode mode) {

	bool btag;
	m_bw  = 1e16;

	ATTRIBUTE ("Scale"   , m_scale);

	ATTRIBUTE ("Filename", m_fname);
	ATTRIBUTE ("DataPath", m_dpath);

	btag = RFPulse::Prepare(mode);

	if (!m_dpath.length())
		m_dpath = "/";

	if (!m_fname.length()) {
		cout << "\n warning in Prepare(1) of ExternalRFPulse " << GetName()
			 << " : can not read binary file  with empty name" << endl;
		return false;
	}

	BinaryContext bc (m_fname, IO::IN);
	NDData<double> data;

	if (bc.Status() != IO::OK) {
		cout << "\n warning in Prepare(1) of ExternalRFPulse " << GetName()
			 << " : can not read binary file " << m_fname << endl;
		return false;
	}

	if (!m_dpath.length())
		m_dpath = "/";
	m_dname = "mag";
	if (bc.Read(data, m_dname, m_dpath) != IO::OK) {
		printf ("Couldn't read %s from file %s\n", URI(m_dpath, m_dname).c_str(), m_fname.c_str());
		return false;
	}
	m_magnitudes = data.Data();
	m_dname = "pha";
	if (bc.Read(data, m_dname, m_dpath) != IO::OK) {
		printf ("Couldn't read %s from file %s\n", URI(m_dpath, m_dname).c_str(), m_fname.c_str());
		return false;
	}
	m_phases = data.Data();
	if (m_magnitudes.size() != m_phases.size()) {
	       cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : # of magnitude sample and phase sample sizes must agree ("
				<< m_magnitudes.size() << " != " << m_times.size() <<  ")" << endl;
		return false;
	}
	m_dname = "times";
	if (bc.Read(data, m_dname, m_dpath) != IO::OK) {
		printf ("Couldn't read timing data %s \n", URI(m_dpath, m_dname).c_str());
		return false;
	}
	m_times = data.Data();
	if (m_magnitudes.size() != m_times.size()) {
	       cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : # of magnitude samples and time points must agree ("
				<< m_magnitudes.size() << " != " << m_times.size() <<  ")" << endl;
		return false;
	}

	m_duration = m_times.back();

	if (mode != PREP_UPDATE) insertGetPhaseFunction( &ExternalRFPulse::GetExtPhase );

	btag = ( RFPulse::Prepare(mode) && btag);

	if (mode != PREP_UPDATE) {
		HideAttribute ("Duration");
		HideAttribute ("Bandwidth", false);
	}

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

void ExternalRFPulse::SetTPOIs() {

	m_tpoi.Reset();
    for (size_t i = 0; i < m_times.size(); ++i)
    	m_tpoi + TPOI::set(m_times.at(i), -1.);

}


inline double ExternalRFPulse::GetMagnitude (const double time) {

	if (m_duration <= 0.)
		return 0.;
	size_t i = time * m_times.size() / m_duration;
	return (i < m_magnitudes.size()) ? m_scale * m_magnitudes[i] : 0.;

}

inline double ExternalRFPulse::GetPhase (const double time ) {

	if (m_duration <= 0.)
		return 0.;
	size_t i = time * m_times.size() / m_duration;
	return (i < m_magnitudes.size()) ? m_phases[i] * 180. / PI : 0.;

}



