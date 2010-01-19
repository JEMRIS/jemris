/** @file SpiralGradPulse.h
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

#ifndef _SPIRALGRADPULSE_H
#define _SPIRALGRADPULSE_H

#include <cmath>

#include "GradPulse.h"

//! Prototype of a spiral gradient

class SpiralGradPulse : public GradPulse {

 public:


    /**
     * Constructor
     */
    SpiralGradPulse               () {};

    /**
     * Copy constructor.
     */
    SpiralGradPulse               (const SpiralGradPulse&) {};

    /**
     * Destructor.
     */
    ~SpiralGradPulse              () {};

    /**
     *  See Module::clone
     */
    SpiralGradPulse* Clone        () const { return (new SpiralGradPulse(*this)); };

    /**
     * Prepare the spiral gradient pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool     Prepare      (PrepareMode mode);

    /**
     * See Module::GetValue
     */
    virtual double   GetGradient  (double const time);

    /**
     * See Pulse::SetTPOIs
     */
    virtual void     SetTPOIs      ();

 protected:
    /**
     * Get informations on this trapezoidal gradient
     *
     * @return GradPulse::GetInfo plus m_pitch and m_alpha
     */
    virtual string          GetInfo        ();

    double m_pitch;            /**< Spiral trajectory pitch */
    double m_alpha;            /**< Spiral trajectory angle */


};

#endif /*TRAPGRADPULSE_*/
