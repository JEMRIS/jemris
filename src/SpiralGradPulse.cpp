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

	return m_amps [(long)floor(time / (GetDuration() * m_grad_samp_int))];

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

	if (btag && mode != PREP_VERBOSE) {

		double gamma          = 42576000.0;

		double samp_int       = 1.0 / m_bw;
		double max_grad_samp  = 1.0 / (gamma * m_fov * samp_int); 

		m_pitch               = m_arms / (2.0 * PI * m_fov);
		m_beta                = gamma * m_slewrate / m_pitch;

		if (m_max_grad > max_grad_samp) { // Nyquist limit exceeded
			cout << "\n warning in Prepare(1) of KVSPIRAL " << GetName() << endl;
			cout << "Nyquist limit (" << max_grad_samp << ") exceeded." << endl;
		}
		
		double time_of_switch = 0.0;
		double gradient       = 0.0;

		m_samples             = GetDuration() / m_grad_samp_int;

		double time           = 0.0;

		double* angle         = new double[m_samples];
		double* radius        = new double[m_samples];
		double* k             = new double[m_samples];
		m_amps                = new double[m_samples];

		long    I             = 0l;
		
		for (long i = 0; i <= m_samples; i++) {

			time = (double) i *  m_grad_samp_int * GetDuration() / 10;

			if ( time_of_switch == 0.0 ) { // Limited slewrate
				angle [i] = m_beta  * time * time / (2.0 + 2.0 * pow(2.0*m_beta/3.0, 1.0/6.0) * pow(time, 1.0/3.0) + pow(2*m_beta/3, 2.0/3.0) * pow(time, 4.0/3.0));
				radius[i] = m_pitch * angle[i]; 
			} else  {                     // Limited gradient
				angle [i] = sqrt (angle[I] * angle[I] + 2*(time-time_of_switch) * gamma * m_max_grad/m_pitch);
				radius[i] = m_pitch * angle[i]; 
			}

			if ( m_axis == AXIS_GX )
				k[i]  = radius[i] * sin(angle[i]);
			else if ( m_axis == AXIS_GY )
				k[i]  = radius[i] * cos(angle[i]);

		}
		
		if (m_inward) 
			for (long i = 0; i < m_samples; i++) 
				//k[m_samples]

				m_amps[i] = (double) ((k[m_samples-i+1] - k[m_samples - i-1]) / (gamma * m_grad_samp_int));
		else
			for (long i = 0; i < m_samples; i++)
				m_amps[i] = (double) ((k[i+1] - k[i]) / (gamma * m_grad_samp_int));

		delete [] angle;
		delete [] radius;
		delete [] k;

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
