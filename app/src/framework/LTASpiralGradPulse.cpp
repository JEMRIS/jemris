/** @file SpiralGradPulse.cpp
 *  @brief Implementation of JEMRIS SpiralGradPulse
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

#include "LTASpiralGradPulse.h"

/***********************************************************/
double            LTASpiralGradPulse::GetGradient (double const time)  {

    double magn = 0.0;

	double angle = 2*PI*m_N*time/GetDuration();
	double ancos = cos(angle);
	double ansin = sin(angle);

	if      (GetAxis() == AXIS_GX)
		magn = (m_A/GetDuration()) * (-angle*ansin + ancos);
	else if (GetAxis() == AXIS_GY)
		magn = (m_A/GetDuration()) * ( angle*ancos - ansin);

    return magn;

}

/***********************************************************/
bool              LTASpiralGradPulse::Prepare     (PrepareMode mode)   {

    bool btag = true;

    ATTRIBUTE("A" , &m_A);
    ATTRIBUTE("N" , &m_N);

    if ( mode == PREP_VERBOSE) {

        if (!HasDOMattribute("A"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Pitch required!!\n";
        }

        if (!HasDOMattribute("N"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Alpha required!!\n";
        }

    }

    if (GetAxis()==AXIS_GX) m_area = m_A;

    btag = (GradPulse::Prepare(mode) && btag);

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;

    return btag;
}

/***********************************************************/
string          LTASpiralGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (A,N)= (" << m_area << "," << m_N << ")";

	return s.str();

};
