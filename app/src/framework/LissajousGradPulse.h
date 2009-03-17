/** @file LissajousGradPulse.h
 *  @brief Implementation of JEMRIS LissajousGradPulse
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

#ifndef _LISSAJOUSGRADPULSE_H
#define _LISSAJOUSGRADPULSE_H

#include <cmath>

#include "GradPulse.h"

//! Prototype of a Lissajous gradient

class LissajousGradPulse : public GradPulse {

 public:

    /**
     * Constructor
     */
    LissajousGradPulse               ();

    /**
     * Copy constructor.
     */
    LissajousGradPulse               (const LissajousGradPulse&);

    /**
     * Destructor.
     */
    ~LissajousGradPulse              ();

    /**
     *  See Module::clone
     */
    LissajousGradPulse* Clone        () const;

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

    void   SetRhoTurns  (double val) {m_rho_turns = val; } ;
    void   SetPhiTurns  (double val) {m_phi_turns = val; } ;
    void   SetKRadius   (double val) {m_krad = val;      } ;

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

    double m_rho_turns;        /**< Lissajous trajectory declination turns */
    double m_phi_turns;        /**< Lissajous trajectory azimutal turns */
    double m_krad;            /**< Lissajous trajectory radius */


};

#endif /*TRAPGRADPULSE_*/
