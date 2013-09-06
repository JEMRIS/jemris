	/** @file ExternalPulseData.cpp
 *  @brief Implementation of JEMRIS ExternalPulseData
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

#include "ExternalPulseData.h"
#include "ExternalRFPulse.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalPulseData::ExternalPulseData () : m_phase(0.), m_pulse(0) {
  
    m_fname = "";
  
};

/***********************************************************/
double            ExternalPulseData::GetData (double const time)  {

   unsigned i = ((unsigned) (time * m_times.size() / m_pulse->GetDuration()));
   if (i<m_magnitudes.size())
    return ( *((double*) m_pulse->GetAttribute("Scale")->GetAddress() ) * m_magnitudes.at(i) );
/*
   if (time < 0.0 || time > m_pulse->GetDuration() ) return 0.0;
   int i=0;
   double diff = abs(time - m_times.at(0));
   for (i=0; i<m_magnitudes.size(); i++) {
     double diffn = abs(time - m_times.at(i));
     if (diffn>diff) break;
     diff=diffn;
   }
     
     
    double t = (diff==0.0 ? 0.0 : time*2.0/diff);//m_times.size() / m_pulse->GetDuration();
    unsigned j = (diff==0.0 ? i : i+1);
    //unsigned i = ((unsigned) t);
    if (i<m_magnitudes.size()) {
   //unsigned j = (i+1<m_times.size()?i+1:m_times.size()-1);  
	cout << "Siz=" << m_times.size() << ", Dur=" <<  m_pulse->GetDuration()
	     << ", time=" << time << " , t=" << t << ", i=" << i << ",  g= " 
	     << m_magnitudes.at(i)+(m_magnitudes.at(j)-m_magnitudes.at(i))*(t-i) << endl;
	return  m_magnitudes.at(i)+(m_magnitudes.at(j)-m_magnitudes.at(i))*(t-i);
    }
*/
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

    unsigned i = ((unsigned) (time * p->m_times.size() / mod->GetDuration()));

    if (i<p->m_phases.size())
    	return ( p->m_phases.at(i) * 180.0 / PI );

    return 0.0 ;
};


/***********************************************************/
bool  ExternalPulseData::ReadPulseShape (const string& fname, const string& dpath,
		const string& dname, bool verbose) {

	//read data if filename changed
	if (m_fname == fname) return true;

	BinaryContext bc;
	DataInfo      di;

	bc.Initialize (fname, IO::IN);
	if (bc.Status() != IO::OK) {
		cout	<< "Error in Module " << m_pulse->GetName()
				<< " ::Prepare can not read HDF5 file " << fname << endl;
		return false;
	}
	
	int columns = (m_pulse->GetAxis() == AXIS_RF) ? 3 : 2;
	
	if (bc.ReadData (m_magnitudes, "mag", dpath) != IO::OK)
		return false;

	if (bc.ReadData (m_times, "t", dpath) != IO::OK)
		return false;

	m_pulse->m_tpoi.Reset();
	for (size_t i = 0; i < m_times.size(); ++i)
		m_pulse->m_tpoi + (m_times[i], -1.0);

	if (bc.ReadData (m_phases, "pha", dpath) != IO::OK)

	m_pulse->SetDuration(m_times.back());
	m_pulse->m_tpoi + TPOI::set(TIME_ERR_TOL, -1.0);
	m_fname = fname;
		
	return true;

};


