/** @file SpiralGradPulse.cpp
 *  @brief Implementation of JEMRIS SpiralGradPulse
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

#include "SpiralGradPulse.h"

/***********************************************************/
double            SpiralGradPulse::GetGradient (double const time)  {

	long index = (long) floor (time / m_grad_samp_int);

	if (index > m_samples-1)
		index = m_samples-1;

	return m_amps [index];

}

/***********************************************************/
bool              SpiralGradPulse::Prepare     (PrepareMode mode)   {

    bool btag = true;
	m_inward  = false;

    ATTRIBUTE("Arms"       , m_arms);
	ATTRIBUTE("MaxSlew"    , m_slewrate);
	ATTRIBUTE("MaxGrad"    , m_max_grad);
	ATTRIBUTE("GradSampInt", m_grad_samp_int);
	ATTRIBUTE("FOV"        , m_fov);
	ATTRIBUTE("BandWidth"  , m_bw);
	ATTRIBUTE("Inward"     , m_inward);
	
    if ( mode == PREP_VERBOSE) {

        if (!HasDOMattribute("Arms"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Arms required!!\n";
        }

        if (!HasDOMattribute("MaxSlew"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: MaxSlew required!!\n";
        }

        if (!HasDOMattribute("MaxGrad"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: MaxGrad required!!\n";
        }

        if (!HasDOMattribute("FOV"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: FOV required!!\n";
        }

        if (!HasDOMattribute("BandWidth"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: BandWidth required!!\n";
        }

    }

    btag = (GradPulse::Prepare(mode) && btag);

	if (mode == PREP_VERBOSE) {

		double gamma          = 42576000.0;

		double samp_int       = 1.0 / m_bw;

		double max_grad_samp  = 1000 / (gamma * m_fov * samp_int); 

		m_pitch               = 1000 * m_arms / (2.0 * PI * m_fov);
		m_beta                = gamma * m_slewrate / m_pitch;

		if (m_max_grad/1000 > max_grad_samp) { // Nyquist limit exceeded
			cout << "\n warning in Prepare(1) of KVSPIRAL " << GetName() << endl;
			cout << "Nyquist limit (" << max_grad_samp << ") exceeded." << endl;
		}
		
		double time_of_switch = 0.0;
		double gradient       = 0.0;

		m_samples             = GetDuration() / m_grad_samp_int;

		double time           = 0.0;
		double phi            = 0.0;
		double max_phi        = 0.0;

		double klast[2];
		double k    [2];
		double g    [2];
		double gabs           = 0.0;

		m_amps                = new double[m_samples];

		m_initialised         = true;

		k[0]                  = 0.0;
		m_amps[0]             = 0.0;

		for (long i = 0; i <= m_samples; i++) {

			time = (double) i * m_grad_samp_int / 1000;
			
			if ( time_of_switch == 0.0 ) // Limited slewrate
				phi = m_beta * pow (time, 2.0) / (2.0 + 2.0 * pow(2.0*m_beta/3.0, 1.0/6.0) * pow(time, 1.0/3.0) + pow(2*m_beta/3, 2.0/3.0) * pow(time, 4.0/3.0));
			else                         // Limited gradient
				phi = sqrt ( pow(max_phi, 2.0) + 2.0 * (time-time_of_switch) * gamma * m_max_grad / m_pitch * PI / 1000);
			
			k[XC]  = m_pitch * phi * sin(phi);
			k[YC]  = m_pitch * phi * cos(phi);
			
			if (i > 0) {

				g[XC]  = 10000000.0 * (k[XC] - klast[XC]) / m_fov * 2 * PI / (gamma * m_grad_samp_int);
				g[YC]  = 10000000.0 * (k[YC] - klast[YC]) / m_fov * 2 * PI / (gamma * m_grad_samp_int);

				m_amps[i] = 1.0e16 * (m_axis == AXIS_GX) ? g[XC] : g[YC];
				
				gabs   = sqrt (pow(g[XC],2) + pow(g[YC],2));

			}

			klast[XC] = k[XC];
			klast[YC] = k[YC];

			// Maximum gradient reached?
			if (gabs >= m_max_grad && time_of_switch == 0.0) { 
				m_max_grad     = gabs;
				time_of_switch = time;
				max_phi        = phi;
			}
			
		}

		if (m_inward)  {

			double* tmp = new double [m_samples]; 

			for (long i = 0; i < m_samples; i++)
				tmp [m_samples-1 - i] = m_amps[i];

			delete [] m_amps;
			m_amps = tmp;

		}

	}
	
    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of KVSPIRAL " << GetName() << endl;

    return btag;
}


/***********************************************************/
string          SpiralGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (beta,pitch,sampint)= (" << m_beta << "," << m_pitch << ")";

	return s.str();

};


/**********************************************************/
SpiralGradPulse::~SpiralGradPulse ()  { 

	if (m_initialised)
		delete [] m_amps; 

};
