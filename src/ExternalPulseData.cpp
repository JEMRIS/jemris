/** @file ExternalPulseData.cpp
 *  @brief Implementation of JEMRIS ExternalPulseData
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

#include "ExternalPulseData.h"
#include "ExternalRFPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalPulseData::ExternalPulseData ()  {
  
    m_fname = "";
    m_pulse = NULL;
    m_phase = 0;
    m_interp = false;
  
};


/***********************************************************/
double  ExternalPulseData::Interp(double const t, vector<double> const &v)  {

	unsigned s = v.size();
	double x = t * s / m_pulse->GetDuration();
	unsigned i   = unsigned(x);
	x = x-i;
	unsigned n = (i+1<s?i+1:s-1);
	if (i>n) return 0.0;
	return v.at(i) + x*(v.at(n)-v.at(i));

}

/***********************************************************/
double  ExternalPulseData::GetData (double const time)  {

  double scale = *((double*) m_pulse->GetAttribute("Scale")->GetAddress() );

  //linear interpolation
  if (m_interp)
		  return ( scale*Interp(time,m_magnitudes) );

  //otherwise nearest neighbor
  unsigned i   = ((unsigned) (time * m_times.size() / m_pulse->GetDuration()));
  if (i<m_magnitudes.size())
	  return ( scale * m_magnitudes.at(i) );

  //time out of bounds
  return 0.0;
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

	if (p->m_interp)
		return ( p->Interp(time,p->m_phases) * 180.0 / PI );

    unsigned i = ((unsigned) (time * p->m_times.size() / mod->GetDuration()));

    if (i<p->m_phases.size())
    	return ( p->m_phases.at(i) * 180.0 / PI );

    return 0.0 ;
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


