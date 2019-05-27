/** @file ExternalPulseData.cpp
 *  @brief Implementation of JEMRIS ExternalPulseData
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
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

#include "ExternalPulseData.h"
#include "ExternalRFPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalPulseData::ExternalPulseData ()  {
  
    m_fname = "";
    m_pulse = NULL;
    m_phase = 0;
    m_interp = false;
    m_LastHuntIndex = 0;
  
};


/***********************************************************/
double  ExternalPulseData::GetData (double const time)  {

  if (time < 0.0 || time > m_pulse->GetDuration() ) return 0.0;

  double scale = *((double*) m_pulse->GetAttribute("Scale")->GetAddress() );

  int ilo = GetLowerIndex(time);

  //linear interpolation
  if (m_interp) {
	  double step = m_times[ilo+1] - m_times[ilo];
	  double b = (time - m_times[ilo])/step;
		return (scale * (m_magnitudes[ilo] + ((m_magnitudes[ilo + 1] - m_magnitudes[ilo]) * b) ) );
  }
  //otherwise nearest neighbor
  int i = ( (time-m_times[ilo]<m_times[ilo+1]-time) ?ilo:ilo+1);
  return ( scale * m_magnitudes[i] );

}

/***********************************************************/
void  ExternalPulseData::SetTPOIs () {

    m_pulse->Pulse::SetTPOIs();

    for (unsigned int i=0; i<m_times.size(); ++i)
	m_pulse->m_tpoi + TPOI::set(m_times.at(i), -1.0);

};

/*****************************************************************/
double    ExternalPulseData::GetPhase  (Module* mod, double time ) {

	ExternalPulseData*  p = ( (ExternalRFPulse*) mod)->GetPulseData();

    if (time < 0.0 || time > p->m_pulse->GetDuration() ) return 0.0;

	int ilo = p->m_LastHuntIndex;

	if (p->m_interp ) {
		double step = p->m_times[ilo+1] - p->m_times[ilo];
		double b = (time - p->m_times[ilo])/step;
		return ( (180.0 / PI) * (p->m_phases[ilo] + ((p->m_phases[ilo + 1] - p->m_phases[ilo]) * b) ) );
	}

	  //otherwise nearest neighbor
	int i = ( (time-p->m_times[ilo]<p->m_times[ilo+1]-time) ?ilo:ilo+1);
    return ( p->m_phases.at(i) * 180.0 / PI );

};


/***********************************************************/
bool  ExternalPulseData::ReadPulseShape (string fname, bool verbose) {

	//read data if filename changed
	if (m_fname == fname) return true;

	BinaryContext bc(fname, IO::IN);

	if (bc.Status() != IO::OK) {
	      if (verbose)
		cout	<< "Error: ExternalPulseData::ReadPulseShape() can not read HDF5 file " << fname << endl;
	      return false;
	}
	
	NDData<double> data;
	bc.Read(data,"extpulse","/");


	int columns = ( (m_pulse->GetAxis() == AXIS_RF) ? 3 : 2 );
	int samples = data.Size();
	int iNumberOfTimePoints = samples / columns;

	m_times.clear();
	m_magnitudes.clear();
	if (m_pulse->GetAxis() == AXIS_RF) m_phases.clear();
	m_pulse->m_tpoi.Reset();

	for (int i=0; i<iNumberOfTimePoints; ++i) {

		m_times.push_back(data[i]-data[0]);
		m_pulse->m_tpoi + TPOI::set(data[i]-data[0],-1.0);

		m_magnitudes.push_back(data[iNumberOfTimePoints+i]);

		if ( columns==3 ) {
		    m_phases.push_back( data[2*iNumberOfTimePoints+i] );
		}

	}

	m_pulse->SetDuration(data[iNumberOfTimePoints-1]-data[0]);
	m_pulse->m_tpoi + TPOI::set(TIME_ERR_TOL, -1.0);
	m_fname = fname;
		
	return true;
};

/***********************************************************/

int ExternalPulseData::GetLowerIndex(double t) {
	int ihi;
	int ilo;

	// test bounds:
	if ((t<= m_times[0]) || (t>=m_times.back())) {
		if (t == m_times[0]) {ilo = 0;  return ilo;};
		if (t == m_times.back()) {ilo = m_times.size()-2;  return ilo;};
		cout << "Interpolation out of bounds! exit.(t= "<<t<<"; m_times[0]="<<m_times[0]<<"; m_times.back()="<<m_times.back()<< endl;
		exit(-1);
	}

	// hunt phase:
	int iHuntStep = 1;
	int iend = m_times.size()-1;

	if (m_times[m_LastHuntIndex]<t) {
		// hunt up:
		ilo = m_LastHuntIndex;
		ihi = ilo + 1;
		while (m_times[ihi] < t ) {
			ilo = ihi;
			//iHuntStep << 1;
			ihi = ilo + iHuntStep;
			if (ihi > iend) ihi = iend;
		}
	} else {
		// hunt down:
		ihi = m_LastHuntIndex;
		ilo = ihi - 1;
		while (m_times[ilo] > t ) {
			ihi = ilo;
			//iHuntStep << 1;
			ilo = ihi - iHuntStep;
			if (ilo < 0 ) ilo = 0;
		}
	}
	// bisection phase:
	int middle;
	while ( (ihi-ilo) > 1) {
		middle=(ihi+ilo) >> 1;
		if (m_times[middle] > t) ihi = middle; else ilo = middle;
	}

	m_LastHuntIndex = ilo;
	return ilo;
}
/****************************************************************/


