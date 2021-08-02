/** @file SpiralGradPulse.cpp
 *  @brief Implementation of JEMRIS SpiralGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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
m_slewrate(0), m_amps(0), m_fov(0), m_intl(0), m_max_grad(0),
m_grad_raster_time(0), m_samples(0), m_res(0), m_inward(0) {}


SpiralGradPulse::SpiralGradPulse (const SpiralGradPulse& sp) :
m_slewrate(0), m_amps(0), m_fov(0), m_intl(0), m_max_grad(0),
m_grad_raster_time(0), m_samples(0), m_res(0), m_inward(0){

	m_slew_rate = sp.m_slewrate;
	m_amps = sp.m_amps;
	m_fov = sp.m_fov;
	m_max_grad = sp.m_max_grad;
	m_grad_raster_time = sp.m_grad_raster_time;
	m_samples = sp.m_samples;
	m_res = sp.m_res;
	m_inward = sp.m_inward;

}


/***********************************************************/
double            SpiralGradPulse::GetGradient (double const time)  {

	// linear interpolation
	size_t ilo (floor (time / m_grad_raster_time));
	double tlo = ilo * m_grad_raster_time;
	double val = (time - tlo) * (m_amps[ilo + 1] - m_amps[ilo]) / m_grad_raster_time + m_amps[ilo];
	return (m_inward) ? -val : val;

	// nearest neighbour
	// 	size_t index (round (time / m_grad_raster_time));
	// 	return (m_inward) ? -m_amps[index] : m_amps[index]; 

}

/***********************************************************/
bool              SpiralGradPulse::Prepare     (const PrepareMode mode)   {

    bool btag = true;

    ATTRIBUTE("Interleaves"	, m_intl);
	ATTRIBUTE("SlewRate"	, m_slewrate);
	ATTRIBUTE("MaxAmpl"		, m_max_grad);
	ATTRIBUTE("GradRasterTime", m_grad_raster_time);
	ATTRIBUTE("FOV"			, m_fov);
	ATTRIBUTE("Resolution"	, m_res);
	ATTRIBUTE("SpiralIn"	, m_inward);

    if ( mode == PREP_VERBOSE) {

        if (!HasDOMattribute("Interleaves"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Number of spiral interleavevs required.\n";
			return false;
        }

        if (!HasDOMattribute("SlewRate"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: SlewRate [rad/mm/ms/ms] required.\n";
			return false;
        }

        if (!HasDOMattribute("MaxAmpl"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: MaxAmpl [rad/mm/ms] required.\n";
			return false;
        }

        if (!HasDOMattribute("FOV"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: FOV [mm] required.\n";
			return false;
        }

        if (!HasDOMattribute("Resolution"))    {
            btag = false;
            cout << GetName() << "::Prepare() error: Resolution [mm] required.\n";
			return false;
        }

		if (!HasDOMattribute("GradRasterTime"))    {
            btag = false;
            cout << GetName() << ":Warning: GradRasterTime [ms] not defined.\n";
			return false;
        }

    }

    btag = (GradPulse::Prepare(mode) && btag);

	if (mode == PREP_VERBOSE) {
		
		double gammabar = 42.5766;
		double kmax = 5./m_res;  							// kmax = 1/(2*res) BUT: kmax in 1/cm, res in mm
		double Gmax = m_max_grad/(2*PI * gammabar/100.);	// rad/mm/ms -> G/cm
		double Smax = m_slewrate/(2*PI * gammabar/100.);	// rad/mm/ms/ms -> G/cm/ms
		m_fov /= 10.;										// [mm] >> [cm]

		double dr = 1./100. * 1./(m_fov/m_intl); 
		long   nr = long(kmax/dr) + 1;

		vector<double> x, y, z;
		x.resize(nr, 0.);
		y.resize(nr, 0.);
		z.resize(nr, 0.);

		// calculate parametrized curve
		double theta = 0.;
		for (size_t k=0; k<nr; k++) {
			double r = k*dr;
			x[k] = r * cos(theta);
			y[k] = r * sin(theta);
			theta += 2.*PI * dr*m_fov / m_intl;
		}
				
		int n;
		double g0   = 0.; // gradient will start at 0.
		double gfin = 0.; // and end at 0.
		double *gx; double *gy; double *gz;

		minTimeGradientRIV(&x[0], &y[0], &z[0], nr, g0, gfin, Gmax, Smax, m_grad_raster_time, gx, gy, gz, n, -0.5, gammabar/10.);
		m_amps.resize(n);

		// gradient [G/cm] -> [rad/mm/ms]
		m_area = 0.;
		for (size_t k=0; k<n; ++k) {
			if(m_axis == AXIS_GX)
				m_amps[k] = gx[k] * 2*PI * gammabar/100.;
			else if(m_axis == AXIS_GY)
				m_amps[k] = gy[k] * 2*PI * gammabar/100.;
			else
				m_amps[k] = 0;
			m_area += m_amps[k] * m_grad_raster_time;
    	}
		
		// add points at start and end of gradient to avoid slewrate overflow
		m_amps.insert(m_amps.begin(), m_amps[0]/2.);
		m_amps.push_back(m_amps.back()/2.);
		m_area += (m_amps[0] + m_amps.back()) * m_grad_raster_time;

		// add one zero in the end to get also the last value in Pulseq output
		m_amps.push_back(0);

		m_samples = m_amps.size();
		SetDuration((m_samples - 1) * m_grad_raster_time); // first sample at t=0s

		if (m_inward){
			for (size_t k=0; k<(int)m_amps.size()/2; ++k)
				swap(m_amps[k],m_amps[m_amps.size()-1-k]);
		}

	}

    if (!btag && mode == PREP_VERBOSE)
        cout << "\n warning in Prepare(1) of SPIRAL " << GetName() << endl;
	
	if (mode != PREP_UPDATE){
		HideAttribute ("Duration");
		HideAttribute ("Area");
	}

    return btag;
}


/***********************************************************/
string          SpiralGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo() << " , (intl,grad_raster_time,inward)= (" << m_intl << "," << m_grad_raster_time << "," << m_inward << ")";

	return s.str();

}


/**********************************************************/
SpiralGradPulse::~SpiralGradPulse ()  { }
