/** @file SpiralGradPulse.cpp
 *  @brief Implementation of JEMRIS SpiralGradPulse
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

#include <algorithm>
#include "SpiralGradPulse.h"


SpiralGradPulse::SpiralGradPulse() :
m_slewrate(0), m_amps(0), m_beta(0), m_fov(0), m_arms(0), m_max_grad(0),
m_grad_samp_int(0), m_pitch(0), m_samples(0), m_bw(0), m_inward(0) {}


SpiralGradPulse::SpiralGradPulse (const SpiralGradPulse& sp) :
m_slewrate(0), m_amps(0), m_beta(0), m_fov(0), m_arms(0), m_max_grad(0),
m_grad_samp_int(0), m_pitch(0), m_samples(0), m_bw(0), m_inward(0){

	m_slew_rate = sp.m_slewrate;
	m_amps = sp.m_amps;
	m_beta = sp.m_beta;
	m_fov = sp.m_fov;
	m_max_grad = sp.m_max_grad;
	m_grad_samp_int = sp.m_grad_samp_int;
	m_pitch = sp.m_pitch;
	m_samples = sp.m_samples;
	m_bw = sp.m_bw;
	m_inward = sp.m_inward;

}


/***********************************************************/
double            SpiralGradPulse::GetGradient (double const time)  {

	long index = (long) floor (time / m_grad_samp_int);

	return (m_inward) ? -m_amps[index] : m_amps[index]; 

}

/***********************************************************/
bool              SpiralGradPulse::Prepare     (const PrepareMode mode)   {

    bool btag = true;
	m_inward  = 0;

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
		
		m_samples             = GetDuration() / m_grad_samp_int;
		
		double time           = 0.0;
		double phi            = 0.0;
		double max_phi        = 0.0;
		
		double klast[3];
		double k    [3];
		double g    [3];
		double gabs           = 0.0;

		m_amps.resize(m_samples+1);
		
		k[XC]                 = 0.0;
		k[YC]                 = 0.0;
		k[ZC]                 = 0.0;
		klast[XC]             = 0.0;
		klast[YC]             = 0.0;
		klast[ZC]             = 0.0;
		g[XC]                 = 0.0;
		g[YC]                 = 0.0;
		g[ZC]                 = 0.0;
		
		m_amps[0]             = 0.0;
		
		for (long i = 0; i <= m_samples; i++) {
			
			time = (double) i * m_grad_samp_int / 1000;
			
			if ( time_of_switch == 0.0 ) // Limited slewrate
				phi = m_beta * pow (time, 2.0) / (2.0 + 2.0 * pow(2.0*m_beta/3.0, 1.0/6.0) * pow(time, 1.0/3.0) + pow(2*m_beta/3, 2.0/3.0) * pow(time, 4.0/3.0));
			else                         // Limited gradient
				phi = sqrt ( pow(max_phi, 2.0) + 2.0 * (time-time_of_switch) * gamma * m_max_grad / m_pitch * PI / 1000);
			
			k[XC]  = m_pitch * phi * sin(phi);
			k[YC]  = m_pitch * phi * cos(phi);
			k[ZC]  = m_pitch * phi * sin (phi/3/PI*4) * cos (phi/3/PI*4) * 2;
			
			if (i > 0) {
				
				g[XC]  = 10000000.0 * (k[XC] - klast[XC]) / m_fov * 2 * PI / (gamma * m_grad_samp_int);
				g[YC]  = 10000000.0 * (k[YC] - klast[YC]) / m_fov * 2 * PI / (gamma * m_grad_samp_int);
				g[ZC]  = 10000000.0 * (k[ZC] - klast[ZC]) / m_fov * 2 * PI / (gamma * m_grad_samp_int);
				
				if (m_axis == AXIS_GX)
					m_amps[i] = g[XC];
				else if (m_axis == AXIS_GY)
					m_amps[i] = g[YC];
				else
					m_amps[i] = g[ZC];

				gabs   = sqrt (pow(g[XC],2) + pow(g[YC],2) + pow(g[ZC],2));
				
			}
			
			klast[XC] = k[XC];
			klast[YC] = k[YC];
			klast[ZC] = k[ZC];
			
			// Maximum gradient reached?
			if (gabs >= m_max_grad && time_of_switch == 0.0) { 
				m_max_grad     = gabs;
				time_of_switch = time;
				max_phi        = phi;
			}
			
		}
		
		if (m_inward)  {
			
			double* tmp = (double*) malloc ((m_samples+1)*sizeof(double)); 
			std::reverse_copy (&m_amps[0], &m_amps[m_samples], &tmp[0]);
			memcpy (&m_amps[0], &tmp[0], sizeof(double)*m_samples);
			m_amps[m_samples] = 0.0;
			free (tmp);

		}
		
	}
	
    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of KVSPIRAL " << GetName() << endl;
	
    return btag;
}


/***********************************************************/
string          SpiralGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (beta,pitch,sampint,inward)= (" << m_beta << "," << m_pitch << "," << m_inward << ")";

	return s.str();

}


/**********************************************************/
SpiralGradPulse::~SpiralGradPulse ()  { }
