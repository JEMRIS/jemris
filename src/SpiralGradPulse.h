/** @file SpiralGradPulse.h
 *  @brief Implementation of JEMRIS SpiralGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#ifndef _SPIRALGRADPULSE_H
#define _SPIRALGRADPULSE_H

#include <cmath>

#include "GradPulse.h"

//! Prototype of a spiral gradient as described by Pauly et al.

class SpiralGradPulse : public GradPulse {
	
 public:
	
    /**
     * Constructor
     */
    SpiralGradPulse               ();
	
    /**
     * Copy constructor.
     */
    SpiralGradPulse               (const SpiralGradPulse&);
	
    /**
     * Destructor.
     */
    ~SpiralGradPulse              ();

    /**
     *  See Module::clone
     */
    inline SpiralGradPulse* Clone () const {return (new SpiralGradPulse(*this));};

    /**
     * Prepare the spiral gradient pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool     Prepare      (const PrepareMode mode);

    /**
     * See Module::GetValue
     */
    virtual double   GetGradient  (double const time);

 protected:
    /**
     * Get informations on this trapezoidal gradient
     *
     * @return GradPulse::GetInfo plus m_pitch and m_alpha
     */
    virtual string          GetInfo        ();

    double  m_arms;          /**< Acceleration factor */
    double  m_slewrate;      /**< Maximum slew rate override  */
    double  m_max_grad;      /**< Maximum gradient amplitude override*/
    double  m_grad_samp_int; /**< Gradient samping interval */
    double  m_fov;           /**< Field of view override */
    double  m_bw;            /**< Bandwidth */
    double  m_pitch;         /**< Spiral pitch */
    double  m_beta;          /**< Beta */
	int     m_inward;        /**< Spiral in? */
	long    m_samples;       /**< Number of samples */
	std::vector<double> m_amps;          /**< Gradient amplitudes */

};

#endif /*_SPIRALGRADPULSE_H*/
