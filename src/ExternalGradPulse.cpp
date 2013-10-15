/** @file ExternalGradPulse.cpp
 *  @brief Implementation of JEMRIS ExternalGradPulse
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

#include "ExternalGradPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalGradPulse::ExternalGradPulse               (const ExternalGradPulse&)  {
  
    m_scale=1.0;
    m_fname="";

}

/***********************************************************/
bool              ExternalGradPulse::Prepare     (const PrepareMode mode)   {

    ATTRIBUTE("Filename", m_fname);
    ATTRIBUTE("Scale"   , m_scale);
	ATTRIBUTE("DataPath", m_dpath);

	bool btag = GradPulse::Prepare(mode);

	if (!m_dpath.length())
		m_dpath = "/";

	if (!m_fname.length()) {
	       cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : can not read binary file  with empty name" << endl;
		return false;
	}

	BinaryContext bc (m_fname, IO::IN);
	NDData<double> data;

	if (bc.Status() != IO::OK) {
	       cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : can not read binary file " << m_fname << endl;
		return false;
	}

	m_dname = "G";
	if (m_axis == AXIS_GX)
		m_dname += "x";
	else if (m_axis == AXIS_GY)
		m_dname += "y";
	else if (m_axis == AXIS_GZ)
		m_dname += "z";

	if (bc.Read(data, m_dname, m_dpath) != IO::OK) {
		printf ("Couldn't read %s from file %s\n", URI(m_dpath, m_dname).c_str(), m_fname.c_str());
		return false;
	}
	m_magnitudes = data.Data();

	if (bc.Read(data, "times", m_dpath) != IO::OK) {
		printf ("Couldn't read timing data %s \n", URI(m_dpath, "times").c_str());
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

	if ( btag && m_tpoi.GetSize()>0 )
		m_area = GetAreaNumeric(m_tpoi.GetSize());
    
	btag = ( GradPulse::Prepare(mode) && btag);

	if (mode != PREP_UPDATE) {
		HideAttribute ("Duration");
		HideAttribute ("Area");
		HideAttribute ("MaxAmpl",false);
		HideAttribute ("SlewRate",false);
	}

	if (!btag && mode == PREP_VERBOSE)
        cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : can not read binary file " << m_fname << endl;

    return btag;

}


inline double ExternalGradPulse::GetGradient (const double time) {

	if (m_duration <= 0.)
		return 0.;
	size_t i = time * m_times.size() / m_duration;
	return (i < m_magnitudes.size()) ? m_scale * m_magnitudes[i] : 0.;

}


string ExternalGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , binary file = " << m_fname ;

	return s.str();

}

void ExternalGradPulse::SetTPOIs() {

	m_tpoi.Reset();
    for (size_t i = 0; i < m_times.size(); ++i)
    	m_tpoi + TPOI::set(m_times.at(i), -1.);

}
