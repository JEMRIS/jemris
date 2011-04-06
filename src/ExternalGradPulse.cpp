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
/***********************************************************/
ExternalGradPulse::ExternalGradPulse               (const ExternalGradPulse&)  {
  
    m_scale=1.0;
    m_fname="";
    m_pulse_data.SetPulse(this);
  
}

/***********************************************************/
bool              ExternalGradPulse::Prepare     (PrepareMode mode)   {

    ATTRIBUTE("Filename" , m_fname);
    ATTRIBUTE("Scale"    , m_scale);

	//read data
	bool btag = m_pulse_data.ReadPulseShape (m_fname, mode == PREP_UPDATE);

	if ( btag && m_tpoi.GetSize()>0 ) m_area = GetAreaNumeric(m_tpoi.GetSize());

	btag = ( GradPulse::Prepare(mode) && btag);
    
	if (mode != PREP_UPDATE) HideAttribute ("Duration");

	if (!btag && mode == PREP_VERBOSE)
        cout	<< "\n warning in Prepare(1) of ExternalGradPulse " << GetName()
				<< " : can not read binary file " << m_fname << endl;

    return btag;

}

/***********************************************************/
string          ExternalGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , binary file = " << m_fname ;

	return s.str();

}
