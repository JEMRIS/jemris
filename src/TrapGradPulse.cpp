/** @file TrapGradPulse.cpp
 *  @brief Implementation of JEMRIS TrapGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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

#include "TrapGradPulse.h"
#include "ConcatSequence.h"

TrapGradPulse::TrapGradPulse        (const TrapGradPulse& hrfp) {
	m_flat_top_area	    = 0.0;
	m_flat_top_time	    = 0.0;
	m_asym_sr	    	= 0.0;
	m_has_flat_top_time = false;
	m_has_flat_top_area = false;
	m_has_duration	    = false;
	m_has_area		    = false;
	m_has_rise_time     = false;
	m_amplitude	    	= 0.0;
	m_ramp_up_time	    = 0.0;
	m_time_to_ramp_dn   = 0.0;
	m_ramp_dn_time	    = 0.0;
	m_slope_up	    	= 0.0;
	m_slope_dn	    	= 0.0;
	m_ft  				= 0.0;
	m_fa  				= 0.0;
	m_du  				= 0.0;
	m_ar  				= 0.0;
}

/***********************************************************/
TrapGradPulse::TrapGradPulse        ()                          {
	m_flat_top_area	    = 0.0;
	m_flat_top_time	    = 0.0;
	m_asym_sr	    	= 0.0;
	m_has_flat_top_time = false;
	m_has_flat_top_area = false;
	m_has_duration	    = false;
	m_has_area		    = false;
	m_has_rise_time     = false;
	m_amplitude	    	= 0.0;
	m_ramp_up_time	    = 0.0;
	m_time_to_ramp_dn   = 0.0;
	m_ramp_dn_time	    = 0.0;
	m_slope_up	    	= 0.0;
	m_slope_dn	    	= 0.0;
	m_ft  				= 0.0;
	m_fa  				= 0.0;
	m_du  				= 0.0;
	m_ar  				= 0.0;
}

/***********************************************************/
TrapGradPulse::~TrapGradPulse       ()                          {}

/***********************************************************/
TrapGradPulse* TrapGradPulse::Clone () const  { return (new TrapGradPulse(*this)); }


/***********************************************************/
bool TrapGradPulse::Prepare  (PrepareMode mode) {

	ATTRIBUTE("FlatTopArea"         , m_flat_top_area   );
	ATTRIBUTE("FlatTopTime"         , m_flat_top_time   );
	ATTRIBUTE("Asymmetric"          , m_asym_sr         );
	ATTRIBUTE("Frequency"           , m_frequency       );
	ATTRIBUTE("InitialPhase"        , m_initial_phase   );

	HIDDEN_ATTRIBUTE("Amplitude"    , m_amplitude       );
	HIDDEN_ATTRIBUTE("RampUpTime"   , m_ramp_up_time    );
	HIDDEN_ATTRIBUTE("RampDnTime"   , m_ramp_dn_time    );
	HIDDEN_ATTRIBUTE("EndOfFlatTop" , m_time_to_ramp_dn ); //for convenience: m_time_to_ramp_dn = m_ramp_up_time + m_flat_top_time

    if (mode != PREP_UPDATE) HideAttribute("Vector",false);

	if ( mode != PREP_UPDATE )
	{
		//XML error checking
    	m_has_flat_top_time = HasDOMattribute("FlatTopTime"); // these conditions have to be known
    	m_has_flat_top_area = HasDOMattribute("FlatTopArea"); // also during PREP_UPDATE, therefore
    	m_has_duration      = HasDOMattribute("Duration");    // local boolean are defined
    	m_has_area          = HasDOMattribute("Area");        // to increase speed
    	m_has_rise_time     = (m_rise_time>0.0);

    	if (m_has_flat_top_area && !m_has_area )
    		CopyObservers(GetAttribute("Area"),GetAttribute("FlatTopArea"));
    	if (!m_has_flat_top_area && m_has_area )
    		CopyObservers(GetAttribute("FlatTopArea"),GetAttribute("Area"));

		if ( m_has_duration && m_has_flat_top_time )
		{
			if ( mode == PREP_VERBOSE)
				cout	<< GetName() << "::Prepare() error: set only one of "
					<< "'Duration' and 'FlatTopTime' for a TrapGradPulse\n";
			return false;
		}

		if ( m_has_area && m_has_flat_top_area )
		{
			if ( mode == PREP_VERBOSE)
				cout	<< GetName() << "::Prepare() error: set only one of "
					<< "'Area' and 'FlatTopArea' for a TrapGradPulse\n";
			return false;
		}
		if ( m_has_flat_top_time && !m_has_flat_top_area )
		{
			if ( mode == PREP_VERBOSE)
				cout	<< GetName() << "::Prepare() error: 'FlatTopTime' needs "
					<< "also 'FlatTopArea' for a TrapGradPulse\n";
			return false;
		}
	}

	//call the base-class Prepare() and set local variables
	bool btag = GradPulse::Prepare(mode) ;
	if (m_has_flat_top_time ) m_flat_top_time = ceil(100.0*m_flat_top_time)/100.0;
	if (m_has_duration      ) m_duration = ceil(100.0*m_duration)/100.0;
	if (m_has_flat_top_time ) m_ft = m_flat_top_time;
	if (m_has_flat_top_area ) m_fa = m_flat_top_area  	;
	if (m_has_duration      ) m_du = m_duration; 		;
	if (m_has_area          ) m_ar = m_area 			;

	//call SetShape() to compute the trapezoid and set attributes members for observation
	btag = (btag && SetShape(mode == PREP_VERBOSE) );
	if (!m_has_flat_top_time) m_flat_top_time	= m_ft	;
	if (!m_has_flat_top_area) m_flat_top_area 	= m_fa	;
	if (!m_has_duration		) m_duration 		= m_du	;
	if (!m_has_area			) m_area 			= m_ar	;


    if (!btag && mode == PREP_VERBOSE)
             cout << "\n warning in Prepare(1) of TRAPGRADPULSE " << GetName() << endl;
	return btag;

}


/***********************************************************/
inline bool    TrapGradPulse::SetShape  (bool verbose){

	if (m_has_rise_time) {
		m_ramp_up_time = m_rise_time;
		m_ramp_dn_time = m_rise_time;
	} else {
		m_slope_up = m_slew_rate;
		m_slope_dn = -1.0*m_slew_rate;
		if (m_asym_sr > 0.0) m_slope_up *= m_asym_sr;
		if (m_asym_sr < 0.0) m_slope_dn *= fabs(m_asym_sr);
	}

	// Check if requested time possible (when given)
	bool possible=true;
	if (m_has_flat_top_time) {		/* flattop time and flattop area given */
		possible = (m_ft==0) || (fabs(m_fa/m_ft) < m_max_ampl);
		m_amplitude = m_fa/m_ft;
		if (!m_has_rise_time) {
			m_ramp_up_time = ceil(100.0*fabs(m_amplitude/m_slope_up))/100.0;
			m_ramp_dn_time = ceil(100.0*fabs(m_amplitude/m_slope_dn))/100.0;
		}

		m_du = m_ramp_up_time + m_ft + m_ramp_dn_time;
		m_ar = m_amplitude*(m_ramp_up_time/2.0 + m_ramp_dn_time/2.0 + m_ft);

	} else if (m_has_duration) {	/* area and total duration given */
		if (m_has_rise_time) {
			possible = fabs(m_ar/(m_du-m_rise_time)) < m_max_ampl;
		} else {
			double dC = 1.0/fabs(2.0*m_slope_up) + 1.0/fabs(2.0*m_slope_dn);
			possible = m_du*m_du > 4*fabs(m_ar)*dC;
			m_amplitude = ( m_du - sqrt(m_du*m_du - 4*fabs(m_ar)*dC) )/(2.0*dC);
			m_ramp_up_time = ceil(100.0*fabs(m_amplitude/m_slope_up))/100.0;
			m_ramp_dn_time = ceil(100.0*fabs(m_amplitude/m_slope_dn))/100.0;
		}
		m_ft = m_du - m_ramp_up_time - m_ramp_dn_time;
		// Adjust amplitude to achieve given area
		m_amplitude = m_ar/(m_ramp_up_time/2.0 + m_ramp_dn_time/2.0 + m_ft);
		m_fa = m_ft*m_amplitude;
	} else {
		//standard case: Calculate trapezoid in shortest possible time
		SetTrapezoid();
	}

	if (!possible) {
		if (m_has_flat_top_time ) m_ft = m_flat_top_time  	;
		if (m_has_flat_top_area ) m_fa = m_flat_top_area  	;
		if (m_has_duration		) m_du = m_duration 		;
		if (m_has_area			) m_ar = m_area 			;
		SetTrapezoid();
		m_duration = m_du;
		if (verbose)
			cout << GetName() << "::SetShape() warning: requested "
				<< (m_has_duration?"Duration":"FlatTopTime") << " too short for this TrapGradPulse.\n" ;
	}

    m_slope_up = m_amplitude/m_ramp_up_time;
    m_slope_dn = -m_amplitude/m_ramp_dn_time;
	if (fabs(m_ramp_up_time)<1e-8) m_slope_up=1e4;
	if (fabs(m_ramp_dn_time)<1e-8) m_slope_dn=1e4;
	
	m_time_to_ramp_dn = m_ramp_up_time + m_ft;
	return possible;
	
}

/***********************************************************/
inline void    TrapGradPulse::SetTrapezoid  (){

	if (m_ar == 0.0 )
	{
		m_ramp_up_time  = 0.0; m_ramp_dn_time = 0.0;
		m_slope_up = 0.0; m_slope_dn = 0.0; m_amplitude = 0.0;
		m_du = 0.0; m_ft = 0.0;  m_fa=0.0;
		return;
	}
	double area = m_has_flat_top_area ? m_fa : m_ar;
	double dAbsArea = fabs(area);
	double dSign = area/dAbsArea;
	
	if (m_has_rise_time) {
		m_ramp_up_time = m_rise_time;
		m_ramp_dn_time = m_rise_time;
	} else {
		m_slope_up = dSign*m_slew_rate;
		m_slope_dn = -1.0*dSign*m_slew_rate;
		if (m_asym_sr > 0.0) m_slope_up *= m_asym_sr;
		if (m_asym_sr < 0.0) m_slope_dn *= fabs(m_asym_sr);
	}
	
	if (m_has_flat_top_area) {
		// Achieve flat top area with max gradient - not time optimal!
		m_ft = fabs(m_fa/m_max_ampl);
		m_ft = ceil(100.0*m_ft)/100.0;
		m_amplitude=area/m_ft;
		if (!m_has_rise_time) {
			m_ramp_up_time = ceil(100.0*fabs(m_amplitude/m_slope_up))/100.0;
			m_ramp_dn_time = ceil(100.0*fabs(m_amplitude/m_slope_dn))/100.0;
		}
		
		m_ar = m_amplitude*(m_ramp_up_time/2.0 + m_ft + m_ramp_dn_time/2.0);

	} else {
		if (!m_has_rise_time) {
			m_ramp_up_time = ceil(100.0*m_max_ampl/fabs(m_slope_up))/100.0;
			m_ramp_dn_time = ceil(100.0*m_max_ampl/fabs(m_slope_dn))/100.0;
		}
		double area_ramps = m_max_ampl*(m_ramp_up_time + m_ramp_dn_time)/2.0;

		if (dAbsArea <= area_ramps )  //triangle shape (no flat top)
		{
			if (!m_has_rise_time) {
				double reduce_factor = sqrt(dAbsArea/area_ramps);
				m_ramp_up_time = ceil(100.0*reduce_factor*m_ramp_up_time)/100.0;
				m_ramp_dn_time = ceil(100.0*reduce_factor*m_ramp_dn_time)/100.0;
			}
			m_fa 		= 0.0;
			m_ft		= 0.0;

			m_amplitude	= (2.0*area)/(m_ramp_up_time + m_ramp_dn_time) ;
		}
		else                            //trapezoidal shape (with flat top)
		{
			m_ft		= ceil(100.0*(dAbsArea-area_ramps)/m_max_ampl)/100.0;
			m_amplitude	= area/(m_ramp_up_time/2.0 + m_ramp_dn_time/2.0 + m_ft);
			m_fa		= m_amplitude*m_ft;
		}

	}

	m_du = m_ramp_up_time + m_ft + m_ramp_dn_time;

	return;
}

/***********************************************************/
inline double  TrapGradPulse::GetGradient  (double const time){

	//on the ramp up ?
	if (time < m_ramp_up_time )
	{
		//no ramp-up for SlewRate >= 1000 (for constant gradients)
		if (fabs(m_slope_up) >999.9)
			return m_amplitude;
		else
			return (time * m_slope_up);
	}

	//on the flat top ?
	if (time < m_ramp_up_time+m_ft ) { return m_amplitude ; }

	//on the ramp down!
	//no ramp-dn for SlewRate >= 1000 (for constant gradients)
	if (fabs(m_slope_dn) >999.9)
		return m_amplitude;
	else
		return (m_amplitude + (time - m_ramp_up_time - m_ft) * m_slope_dn );

}

/***********************************************************/
inline void  TrapGradPulse::SetTPOIs () {

	m_tpoi.Reset();
	m_tpoi + TPOI::set(TIME_ERR_TOL              , -1.0);
	m_tpoi + TPOI::set(GetDuration()-TIME_ERR_TOL, -1.0);

	int N = abs(GetNADC());
	double p0 = (Pulse::m_phase_lock ? World::instance()->PhaseLock : 0.0);
	p0 += GetInitialPhase()*PI/180.0;

	size_t bitmask = GetNADC() < 0 ? 0 : BIT(ADC_T);

	if ( m_has_flat_top_time || m_has_flat_top_area)
	{
		
		// add flat top time, if erased due to zero area
		if ( m_ar==0.0 ) m_ft = m_flat_top_time;


		//add ADCs only on the flat top
		for (int i = 0; i < N; i++) {
			// Calculate phase due to frequency offset
			double time = (i+0.5)*m_ft/N;
			double p = p0 + GetFrequency()*(time - m_ft/2);
			p = fmod( p, 2*PI );
			p = p<0.0 ? p+2*PI : p;
			p = GetNADC() < 0 ? -1.0 : p;

			// Sample to maintain logical dwell time (e.g. for a 3.2ms FlatTopTime
			// with 32 ADCs, the dwell time is 100us) This is required for hardware implementation
			m_tpoi + TPOI::set(m_ramp_up_time + time, p, bitmask );
		}
	}
	else { 		//set ADCs over total duration (standard)
		for (int i = 0; i < N; i++) {
			// Calculate phase due to frequency offset
			double time = (i+0.5)*GetDuration()/N;
			double p = p0 + GetFrequency()*(time - GetDuration()/2);
			p = fmod( p, 2*PI );
			p = p<0.0 ? p+2*PI : p;
			p = GetNADC() < 0 ? -1.0 : p;

			m_tpoi + TPOI::set(time, p, bitmask);
		}
	}

	//add TPOIs at nonlinear points of the trapezoid
	m_tpoi + TPOI::set(m_ramp_up_time	   , -1.0);
	m_tpoi + TPOI::set(m_ramp_up_time+m_ft , -1.0);

}

/***********************************************************/
string          TrapGradPulse::GetInfo() {

	stringstream s;
	s << GradPulse::GetInfo();
	if ( m_has_flat_top_time )
		s << " , FlatTop: (Area,time)= (" << m_flat_top_area << "," << m_flat_top_time << ")";

	if ( GetInitialPhase() != 0.0 )
		s << " , InitialPhase=" << GetInitialPhase();

	if ( GetFrequency() != 0.0 )
		s << " , Frequency=" << GetFrequency();

	return s.str();
}

/*****************************************************************/
inline void TrapGradPulse::GenerateEvents(std::vector<Event*> &events) {

	// Create standard trapezoid events
	GradEvent *grad = new GradEvent();
	grad->m_flat_time = round(m_flat_top_time*1e3);
	grad->m_ramp_up_time = round(m_ramp_up_time*1e3);
	grad->m_ramp_down_time = round(m_ramp_dn_time*1e3);
	grad->m_amplitude = m_amplitude;

	grad->m_channel = (int)(m_axis-AXIS_GX);
	grad->m_shape = -1;	// indicates trapezoid;

	events.push_back(grad);

	int N = GetNADC();
	if (N>0) {
		ADCEvent *adc = new ADCEvent();
		adc->m_num_samples = N;
		adc->m_dwell_time = 1e6*m_flat_top_time/N;
		adc->m_delay = m_ramp_up_time*1e3;

		double p = GetInitialPhase()*PI/180.0;
		p = fmod( p, 2*PI );
		p = p<0.0 ? p+2*PI : p;
		p = round(p*1.0e5)/1.0e5;
		adc->m_phase_offset = p;
		adc->m_freq_offset = GetFrequency();

		events.push_back(adc);
	}
}
