/** @file LissajousGradPulse.cpp
 *  @brief Implementation of JEMRIS LissajousGradPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#include "LissajousGradPulse.h"

/***********************************************************/
LissajousGradPulse::LissajousGradPulse               (const LissajousGradPulse& hrfp) {
	m_rho_turns = 20;
	m_phi_turns = 21;
	m_krad     = 0.05;
};

/***********************************************************/
LissajousGradPulse::LissajousGradPulse               ()                   {};

/***********************************************************/
LissajousGradPulse::~LissajousGradPulse              ()                   {};

/***********************************************************/
LissajousGradPulse*  LissajousGradPulse::Clone       () const             {
	return (new LissajousGradPulse(*this));
}

/***********************************************************/
double            LissajousGradPulse::GetGradient (double const time)  {

        double t = (2.0 * PI * time)/GetDuration();
        if (GetAxis() == AXIS_GX)
              return ( (m_krad*cos(m_rho_turns*t)*m_rho_turns*cos(m_phi_turns*t)-m_krad*sin(m_rho_turns*t)*sin(m_phi_turns*t)*m_phi_turns)/(2.0*GetDuration()) );
        if (GetAxis() == AXIS_GY)
              return ( (m_krad*cos(m_rho_turns*t)*m_rho_turns*sin(m_phi_turns*t)+m_krad*sin(m_rho_turns*t)*cos(m_phi_turns*t)*m_phi_turns)/(2.0*GetDuration()) );
        if (GetAxis() == AXIS_GZ)
              return ( (-m_krad*sin(m_rho_turns*t)*m_rho_turns)/(2.0*GetDuration()) );

}

/***********************************************************/
bool              LissajousGradPulse::Prepare     (PrepareMode mode)   {

    bool btag = true;

    ATTRIBUTE("TurnsA" , &m_rho_turns);
    ATTRIBUTE("TurnsD" , &m_phi_turns);
    ATTRIBUTE("Radius" , &m_krad);

    if ( mode == PREP_VERBOSE) {
        double dArea = 0.0;
        if (GetAxis() == AXIS_GX)                                                                    
                dArea = m_krad*sin(m_rho_turns*2*PI)*cos(m_phi_turns*2*PI);                       
        if (GetAxis() == AXIS_GY)                                                                    
                dArea = m_krad*sin(m_rho_turns*2*PI)*sin(m_phi_turns*2*PI);                       
        if (GetAxis() == AXIS_GZ)                                                                    
                dArea = m_krad*cos(m_rho_turns*2*PI);
        SetArea(dArea);
    }

    btag = (GradPulse::Prepare(mode) && btag);

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;

    if (mode != PREP_UPDATE) {
		HideAttribute("Area"           );
		HideAttribute("MaxAmpl",  false);
		HideAttribute("SlewRate", false);
	}

    return btag;
}

/***********************************************************/
inline void  LissajousGradPulse::SetTPOIs () {

    //Reset and take care for ADCs
    Pulse::SetTPOIs();

    //add TPOIs at nonlinear points of the trapezoid
    int samplePts = 500;
    for (int i = 1; i<samplePts; i++) {
        m_tpoi + TPOI::set(GetDuration()/samplePts*i, -1.0);
    }

};

/***********************************************************/
string          LissajousGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (rt,pt,kr)= (" << m_rho_turns << "," << m_phi_turns << "," << m_krad << ")";

	return s.str();
};
