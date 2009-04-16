/** @file SpiralGradPulse.cpp
 *  @brief Implementation of JEMRIS SpiralGradPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#include "SpiralGradPulse.h"

/***********************************************************/
double            SpiralGradPulse::GetGradient (double const time)  {

    double value = 0.0;

    double a = m_alpha;
    double p = m_pitch;
    double t = time;

    double b    = a * pow(t,2) / (2 + 2 * pow(2/3*a,1/6) * pow(t,1/3) + pow(2/3*a,2/3) * pow(t,4/3));
    double bdot = 2.0*a*t/(2.0+2.0/3.0*pow(2.0,1/6)*pow(3.0,5/6)*pow(a,1/6)*pow(t,1/3)+pow(2.0,2/3)*pow(3.0,1/3)*pow(a,2/3)*pow(t,4/3)/3.0)-a*t*t/pow(2.0+2.0/3.0*pow(2.0,1/6)*pow(3.0,5/6)*pow(a,1/6)*pow(t,1/3)+pow(2.0,2/3)*pow(3.0,1/3)*pow(a,2/3)*pow(t,4/3)/3.0,2.0)*(2.0/9.0*pow(2.0,1/6)*pow(3.0,5/6)*pow(a,1/6)/pow(t,2/3)+4.0/9.0*pow(2.0,2/3)*pow(3.0,1/3)*pow(a,2/3)*pow(t,1/3));

    if      (m_axis == AXIS_GX) value = bdot * p * (sin(b) + b * cos(b));
    else if (m_axis == AXIS_GY) value = bdot * p * (cos(b) - b * sin(b));

    return value;
}

/***********************************************************/
bool              SpiralGradPulse::Prepare     (PrepareMode mode)   {

    bool btag = true;

    ATTRIBUTE("Pitch" , m_pitch);
    ATTRIBUTE("Alpha" , m_alpha);
    if ( mode == PREP_VERBOSE) {

        //XML error checking
        if (!HasDOMattribute("Alpha"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Alpha required!!\n";
        }
        if (!HasDOMattribute("Pitch"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Pitch required!!\n";
        }

        double dArea = 0.0;
        for (int i = 0; i < GetDuration()*1000; i++)
            dArea += GetGradient((double)i/1000)/1000;
        SetArea(dArea);

    }

    btag = (GradPulse::Prepare(mode) && btag);

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;


    return btag;
}

/***********************************************************/
inline void  SpiralGradPulse::SetTPOIs () {

    //Reset and take care for ADCs
    Pulse::SetTPOIs();

    //add TPOIs at nonlinear points of the trapezoid
    int samplePts = 500;
    for (int i = 1; i<samplePts; i++) {
        m_tpoi + TPOI::set(GetDuration()/samplePts*i, -1.0);
    }

};

/***********************************************************/
string          SpiralGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (a,p)= (" << m_alpha << "," << m_pitch << ")";

	return s.str();
};
