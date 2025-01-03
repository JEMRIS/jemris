/** @file TrapGradPulse.h
 *  @brief Implementation of JEMRIS TrapGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2025  Tony Stoecker
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

#ifndef _TRAPGRADPULSE_H
#define _TRAPGRADPULSE_H

#include <cmath>

#include "GradPulse.h"

//! Prototype of a trapezoidal gradient pulse (e.g. for cartesian k-space imaging)

class TrapGradPulse : public GradPulse {

 public:

    /**
     * Constructor
     */
    TrapGradPulse();

    /**
     * Copy constructor.
     */
    TrapGradPulse  (const TrapGradPulse&);

    /**
     * Destructor.
     */
    ~TrapGradPulse                ();

    /**
     *  See Module::clone
     */
    TrapGradPulse*  Clone         () const;

    /**
     * Prepare the trapezoidal gradient pulse.
     *
     * See Module::Prepare
     */
    virtual bool    Prepare       (PrepareMode mode);

    /**
     * See GradPulse::GetGradient
     */
    virtual double  GetGradient   (double const time);

    /**
     * @brief Set the shape of the trapezoidal gradient.
     *
     * The method checks for various inputs from XML
     * and prepares the trapezoidal shape accordingly by
     * calling TrapGradPulse::SetTrapezoid
     *
     * @param verbose Shout error in preparation, if true.
     */
    bool    SetShape      (bool verbose = false);

    /**
     * @brief Set the shape of the trapezoid in minimal
     * possible time.
     */
    void    SetTrapezoid  ();

    /**
     * See Pulse::SetTPOIs
     */
    virtual void    SetTPOIs      ();

    /**
     * @see Pulse::GenerateEvents()
     */
    virtual void	GenerateEvents(std::vector<Event*> &events);

 protected:
    /**
     * Get informations on this trapezoidal gradient
     *
     * @return GradPulse::GetInfo plus info on the flat top (if set)
     */
    virtual string          GetInfo        ();

    double  m_flat_top_area; /**< Area beyond the Flat Top set from XML*/
    double  m_flat_top_time; /**< Duration of the Flat Top set from XML*/
    double  m_asym_sr;      /**< Factor for asymmetric slew rate set from XML. Should be in [-1,1]*/

    bool    m_has_flat_top_time;    /**< true, if corresponding double attribute was set in XML*/
    bool    m_has_flat_top_area;    /**< true, if corresponding double attribute was set in XML*/
    bool    m_has_duration;         /**< true, if corresponding double attribute was set in XML*/
    bool    m_has_area;             /**< true, if corresponding double attribute was set in XML*/
    bool    m_has_rise_time;        /**< true, if corresponding double attribute was set in XML*/
 
    double  m_ft;       /**< internal flat_top time */
    double  m_fa;       /**< internal flat_top area */
    double  m_du;       /**< internal duration      */
    double  m_ar;       /**< internal area          */

    double  m_amplitude;       /**< My amplitude      */
    double  m_ramp_up_time;    /**< My ramp up time   */
    double  m_ramp_dn_time;    /**< My ramp down time */
    double  m_slope_up;        /**< My slope up       */
    double  m_slope_dn;        /**< My slope down     */
    double  m_time_to_ramp_dn; /**< My time to the end of the flat top */
};

#endif /*TRAPGRADPULSE_*/
