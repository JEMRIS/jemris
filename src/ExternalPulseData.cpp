/** @file ExternalPulseData.cpp
 *  @brief Implementation of JEMRIS ExternalPulseData
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

#include "ExternalPulseData.h"
#include "ExternalRFPulse.h"

/***********************************************************/
ExternalPulseData::ExternalPulseData ()  {
  
    m_fname = "";
  
};

/***********************************************************/
double            ExternalPulseData::GetData (double const time)  {

    unsigned i = ((unsigned) (time * m_times.size() / m_pulse->GetDuration()));

    if (i<m_magnitudes.size())
    	return ( *((double*) m_pulse->GetAttribute("Scale")->GetAddress() ) * m_magnitudes.at(i) );

    return 0.0;
}

/***********************************************************/
void  ExternalPulseData::SetTPOIs () {

    m_pulse->m_tpoi.Reset();

    for (unsigned int i=0; i<m_times.size(); ++i)
	m_pulse->m_tpoi + TPOI::set(m_times.at(i), -1.0);

    for (unsigned int i = 0; i < m_pulse->GetNADC(); i++)
        m_pulse->m_tpoi + TPOI::set(i*m_pulse->GetDuration()/m_pulse->GetNADC(), (m_pulse->m_phase_lock?World::instance()->PhaseLock:0.0));

};

/*****************************************************************/
double    ExternalPulseData::GetPhase  (Module* mod, double time ) {

	ExternalPulseData*  p = ( (ExternalRFPulse*) mod)->GetPulseData();

    unsigned i = ((unsigned) (time * p->m_times.size() / mod->GetDuration()));

    if (i<p->m_phases.size())
    	return ( p->m_phases.at(i) * 180.0 / PI );

    return 0.0 ;
};


/***********************************************************/
bool  ExternalPulseData::ReadPulseShape (string fname, bool verbose) {

	//read data if filename changed
	if (m_fname == fname) return true;

	//read TPOIs and B1-vaules from file
	ifstream fin(fname.c_str(), ios::binary);
	if (!fin.is_open()) {
	      if (verbose)
	    	  cout	<< "Error in Module " << m_pulse->GetName()
					<< ": ExternalRFPulse::Prepare can not read RF binary file " << fname << endl;
	      return false;
	}
	m_fname = fname;
		
	double dTP, dVal;
	fin.read((char *)(&(dTP)), sizeof(double));
	int iNumberOfTimePoints = ((int) dTP);

	m_times.clear();
	m_magnitudes.clear();
	m_pulse->m_tpoi.Reset();

	for (int i=0; i<iNumberOfTimePoints; ++i) {

		fin.read((char *)(&dTP), sizeof(double));
		m_times.push_back(dTP);
		m_pulse->m_tpoi + TPOI::set(dTP,-1.0);

		fin.read((char *)(&dVal), sizeof(double));
		m_magnitudes.push_back(dVal);

		if (m_pulse->GetAxis() == AXIS_RF) {
		    fin.read((char *)(&dVal), sizeof(double));
		    m_phases.push_back( dVal );
		}

	}

	m_pulse->SetDuration(dTP);

	fin.close();
	return true;
};


