/** @file SpiralGradPulse.h
 *  @brief Implementation of JEMRIS SpiralGradPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#ifndef _STASPIRALGRADPULSE_H
#define _STASPIRALGRADPULSE_H

#include <cmath>

#include "GradPulse.h"

//! Prototype of a spiral gradient as described by Pauly et al.

class STASpiralGradPulse : public GradPulse {

 public:

    /**
     * Constructor
     */
    STASpiralGradPulse               () {};

    /**
     * Copy constructor.
     */
    STASpiralGradPulse               (const STASpiralGradPulse&) {};

    /**
     * Destructor.
     */
    ~STASpiralGradPulse              () {};

    /**
     *  See Module::clone
     */
    inline STASpiralGradPulse* Clone () const {return (new STASpiralGradPulse(*this));};

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

 protected:
    /**
     * Get informations on this trapezoidal gradient
     *
     * @return GradPulse::GetInfo plus m_pitch and m_alpha
     */
    virtual string          GetInfo        ();

    int    m_N;            /**< Spiral trajectory turns */
    double m_A;            /**< Spiral trajectory pitch */


};

#endif /*STASPIRALGRADPULSE_*/
