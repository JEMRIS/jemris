/** @file ExternalGradPulse.h
 *  @brief Implementation of JEMRIS ExternalGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#ifndef _EXTERNALGRADPULSE_H
#define _EXTERNALGRADPULSE_H

#include <cmath>
#include "GradPulse.h"
#include "ExternalPulseData.h"

/**
 * @brief External gradient prototype.
 */

class ExternalGradPulse : public GradPulse {
  
 public:

    /**
     * @brief Default constructor
     */
    ExternalGradPulse               () {};

    /**
     * @brief Copy constructor.
     */
    ExternalGradPulse               (const ExternalGradPulse&);

    /**
     * @brief Default destructor.
     */
    ~ExternalGradPulse              () {};

    /**
     * @see Prototype::Clone
     */
    inline ExternalGradPulse* Clone () const { return (new ExternalGradPulse(*this)); };

    /**
     * @brief Prepare the external gradient pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool     Prepare      (PrepareMode mode);

    /**
     * @see GradPulse::GetGradient
     */
    virtual double   GetGradient  (double const time) {return m_scale*m_pulse_data.GetData(time); };

    /**
     * @see Pulse::SetTPOIs
     */
    virtual void     SetTPOIs() { m_pulse_data.SetTPOIs(); } ;


 protected:

    /**
     * @brief Get information on this external gradient.
     *
     * @return Information for display.
     */
    string           GetInfo      ();

    ExternalPulseData		   m_pulse_data; /**<contains the data*/
    string m_fname;                /**< @brief Filename containing the RF amps and phases  */
    double m_scale;                /**< @brief Scaling factor for the amps                 */

};

#endif /*EXTERNALGRADPULSE_*/
