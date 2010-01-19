/** @file STARFPulse.cpp
 *  @brief Implementation of JEMRIS STARFPulse
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

#include "STARFPulse.h"

/***********************************************************/
bool STARFPulse::Prepare  (PrepareMode mode) {

    bool btag = true;

    ATTRIBUTE("Gamma", m_Gamma);
    ATTRIBUTE("Alpha", m_Alpha);
    ATTRIBUTE("A"    , m_A);
	ATTRIBUTE("Beta" , m_Beta);
	ATTRIBUTE("N"    , m_N);
	ATTRIBUTE("TPOIs", m_more_tpois ); //number of TPOIs along the analytical expression


    if ( mode == PREP_VERBOSE) {

        //XML error checking
        if (!HasDOMattribute("Alpha"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: gamma required!!\n";
        }

        if (!HasDOMattribute("Gamma"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: gamma required!!\n";
        }

        if (!HasDOMattribute("A"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: A required!!\n";
        }

        if (!HasDOMattribute("Beta"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Beta required!!\n";
        }

        if (!HasDOMattribute("N"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: N required!!\n";
        }


    }

    btag = (RFPulse::Prepare(mode) && btag);

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of RFPULSE " << GetName() << endl;

    return btag;

};

/***********************************************************/
double            STARFPulse::GetMagnitude (double const time)  {

	double magn = 0.0;

	magn = m_Gamma * m_Alpha * m_A / GetDuration() * exp(-pow(m_Beta,2)*pow(1-time/GetDuration(),2)) *
		sqrt( pow(2*PI*m_N*(1-time/GetDuration()),2) + 1);

    return magn;

}

/***********************************************************/
string          STARFPulse::GetInfo() {

	stringstream s;
	s << RFPulse::GetInfo() << " , (Gamma,Alpha,A,Beta,N,Energy) = (" << m_A << "," << m_Gamma << "," << m_Alpha << "," << m_A << "," << m_Beta << "," << m_N << "," << GetIntegralNumeric((int)GetDuration()*1000) << ")";
	return s.str();

}

