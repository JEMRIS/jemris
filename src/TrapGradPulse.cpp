/** @file TrapGradPulse.cpp
 *  @brief Implementation of JEMRIS TrapGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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
	if (m_has_flat_top_time ) m_ft = m_flat_top_time  	;
	if (m_has_flat_top_area ) m_fa = m_flat_top_area  	;
	if (m_has_duration		) m_du = m_duration 		;
	if (m_has_area			) m_ar = m_area 			;

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

	//predefined area definition for the flat top
	if ( m_has_flat_top_area ) {
	       double dC = 2.0/fabs(2.0*m_slew_rate);
           m_ar = m_fa *( 1.0 + m_max_ampl*m_max_ampl*dC / fabs(m_fa) );
	}

	//predefined duration or flat-top time
	if ( m_has_duration || m_has_flat_top_time )
	{
		//Prepare in shortest time first and check if requested time is possible
		double requested    = (m_has_duration?m_duration:m_flat_top_time);
		SetTrapezoid();
		double min_possible = (m_has_duration?m_du:m_ft);
		if (requested < min_possible && verbose)
		{
			cout	<< GetName() << "::SetShape() warning: requested "
				<< (m_has_duration?"duration":"FlatTopTime") << " too short for this TrapezGradPulse.\n" ;
			return false;
		}
		//change system limits (m_max_ampl) so that .Prepare in shortest time"
		//yields exactly the requested time
        double dGmax = m_max_ampl, dC = 0.0;
		if (m_has_duration)
		{
        		dC = 1.0/fabs(2.0*m_slope_up) + 1.0/fabs(2.0*m_slope_dn);
        		m_max_ampl = ( requested - sqrt(requested*requested - 4*fabs(m_area)*dC) )/(2.0*dC);
        		m_ar = m_area;
		}
		else
		{
			m_max_ampl = fabs(m_flat_top_area/requested);
	        dC = 2.0/fabs(2.0*m_slew_rate);
	        if (m_flat_top_area != 0.0)
	        	m_ar = m_flat_top_area *( 1.0 + m_max_ampl*m_max_ampl*dC / fabs(m_flat_top_area) );
	        else
	        	m_ar = 0.0;
		}
		SetTrapezoid();     //Calculate the gradient shape

        m_max_ampl = dGmax; //Reset maximum amplitude

		return true;
	}

	//standard case: Calculate trapezoid in shortest possible time
	SetTrapezoid();
	return true;
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

    double dAbsArea = fabs(m_ar);
    double dSign = m_ar/dAbsArea;
    double Gmax = m_max_ampl;

    m_slope_up = dSign*m_slew_rate;
    m_slope_dn = -1.0*dSign*m_slew_rate;
    if (m_asym_sr > 0.0) m_slope_up *= m_asym_sr;
    if (m_asym_sr < 0.0) m_slope_dn *= fabs(m_asym_sr);
    double dC = 1.0/fabs(2.0*m_slope_up) + 1.0/fabs(2.0*m_slope_dn);

    if (dAbsArea <= Gmax*Gmax*dC )  //triangle shape (no flat top)
    {
    	m_fa 		= 0.0;
        m_amplitude	= dSign*sqrt( dAbsArea / dC ) ;
    }
    else                            //trapezoidal shape (with flat top)
    {
    	m_fa		= dSign* ( dAbsArea - Gmax*Gmax*dC );
        m_amplitude	= dSign*Gmax;
    }

    m_ramp_up_time    = fabs(m_amplitude/m_slope_up);
    m_ramp_dn_time    = fabs(m_amplitude/m_slope_dn);
	m_ft			  = fabs(m_fa/m_amplitude);
	m_time_to_ramp_dn = m_ramp_up_time + m_ft;

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
	p0 += GetInitialPhase();

	if ( GetNADC() < 0 ) p0 = -1.0;

	if ( m_has_flat_top_time && GetNADC()>0 )	//add ADCs  only on the flat top
	{
		// add flat top time, if erased due to zero area
		if ( m_ar==0.0 ) m_ft = m_flat_top_time;
		
		//add ADCs only on the flat top
		for (int i = 0; i < N; i++) {
			// Calculate phase due to frequency offset
			double time = (i+1)*m_ft/(GetNADC()+1);
			double p = p0 + GetFrequency()*(time - m_flat_top_time/2);
			p = fmod( p, TWOPI );
			p = p<0.0 ? p+TWOPI : p;

			m_tpoi + TPOI::set(m_ramp_up_time + time, p, BIT(ADC_T) );
		}
	}
	else { 		//set ADCs over total duration (standard)
		for (int i = 0; i < N; i++) {
			// Calculate phase due to frequency offset
			double time = (i+1)*GetDuration()/(N+1);
			double p = p0 + GetFrequency()*(time - GetDuration()/2);
			p = fmod( p, TWOPI );
			p = p<0.0 ? p+TWOPI : p;

			m_tpoi + TPOI::set(time, p, BIT(ADC_T));
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

	return s.str();
}
