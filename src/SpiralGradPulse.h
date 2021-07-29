/** @file SpiralGradPulse.h
 *  @brief Implementation of JEMRIS SpiralGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
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

#ifndef _SPIRALGRADPULSE_H
#define _SPIRALGRADPULSE_H

#include <cmath>

#include "GradPulse.h"
#include "mtg_functions.h"

// Spiral Gradient calculated with time optimal gradients (Lustig et. al.)

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
     * Get informations on this Spiral gradient
     *
     * @return GradPulse::GetInfo
     */
    virtual string          GetInfo        ();

    double  m_intl;              /**< Spiral interleaves */
    double  m_slewrate;          /**< Maximum slew rate [rad/mm/ms/ms]  */
    double  m_max_grad;          /**< Maximum gradient amplitude [rad/mm/ms]*/
    double  m_grad_raster_time;  /**< Gradient raster time / sampling interval [ms] */
    double  m_fov;               /**< Field of view [mm] */
    double  m_res;               /**< Resolution [mm] */
	 int     m_inward;            /**< Spiral in? */
	 size_t  m_samples;           /**< Number of samples */
	std::vector<double> m_amps;   /**< Gradient amplitudes [rad/mm/ms] */

};

#endif /*_SPIRALGRADPULSE_H*/
