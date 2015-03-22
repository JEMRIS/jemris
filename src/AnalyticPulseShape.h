/** @file AnalyticPulseShape.h
 *  @brief Implementation of JEMRIS AnalyticPulseShape
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

#ifndef _ANALYTICPULSESHAPE_H
#define _ANALYTICPULSESHAPE_H

#include "Pulse.h"

/**
 * @brief prepare and GiNaC evaluation of analytic pulse shapes
 */

class AnalyticPulseShape {

 //friends using this class
 friend class AnalyticGradPulse;
 friend class AnalyticRFPulse;
 friend class EmptyPulse;
 friend class EddyPulse;

 private:
    /**
     * @brief Default destructor.
     */
    ~AnalyticPulseShape              () {};

    /**
     * @brief constructor
     */
    AnalyticPulseShape               ();
  
    /**
     * @brief set the pulse owning this 
     */
    void     SetPulse(Pulse* p){m_pulse=p;};

    /**
     * @brief get the shape value at current time
     *
     * @param time
     * @return Gradient value or RF magnitude
     */
    double   GetShape (double const time)  ;

    /**
     * @brief intitial prepare of the attributes (needs to be called before Prototype::Prepare)
     *
     * @param verbose if true, dump warning/error messages
     */
    void     PrepareInit(bool verbose);

    /**
     * @brief GiNaC prepare of the attributes (needs to ba called after Prototype::Prepare)
     *
     * @param verbose if true, dump warning/error messages
     * @return success of preparation
     */
    bool     PrepareAnalytic(bool verbose);

    /**
     * @brief analytic GiNaC calculation of the integral of this pulse shape
     *
     * @param verbose if true, dump warning/error messages
     * @return value of the analytic integral
     */
    double  GetAnalyticIntegral(bool verbose) { return m_analytic_integral; };

    /**
     * @see Pulse::SetTPOIs
     */
    //void     SetTPOIs();

    static double GetPhase (Module* mod, double time );

    Pulse*	m_pulse;	/**<@brief my pulse                               */
    double	m_phase;	/**<@brief current phase                          */
    bool	m_rfpulse;	/**<@brief true, if my pulse is an RF pulse       */
    bool	m_prepared;	/**<@brief true, if prepared                      */
    
    //unsigned int  m_more_tpois;        /**< For analytic evaluation of GetValue */
    double        m_analytic_value;    /**< For analytic evaluation of GetValue */
    double        m_analytic_time;     /**< For analytic evaluation of GetValue */
    double        m_constant[20];      /**< For analytic evaluation of GetValue */
    double        m_analytic_integral; /**< For analytic evaluation of GetValue */
    
};

#endif /*_ANALYTICPULSESHAPE_H*/
