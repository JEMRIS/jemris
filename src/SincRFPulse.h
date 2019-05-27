/** @file SincRFPulse.h
 *  @brief Implementation of JEMRIS SincRFPulse
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

#ifndef SINCRFPULSE_H_
#define SINCRFPULSE_H_

//! The prototype for Sinc RF Pulse

#include "RFPulse.h"

class SincRFPulse : public RFPulse {

 public:

    /**
     * @brief Default constructor
     */
    SincRFPulse() {};

    /**
     * @brief Copy constructor.
     */
    SincRFPulse  (const SincRFPulse&);

    /**
     * @brief Default destructor.
     */
    ~SincRFPulse () {};

    /**
     * See Module::clone
     */
    inline SincRFPulse* Clone() const { return (new SincRFPulse(*this)); }

    /**
     * @brief Prepare the sinc RF pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return     Success.
     */
    virtual bool        Prepare  (PrepareMode mode);

    /**
     * @brief Returns the sinc shape of this SincRFpulse
     *
     * See also RFPulse::GetMagnitude
     *
     * @param time Time-point at which the magnitude is requested.
     * @return     Magntidue.
     */
    virtual double    GetMagnitude  (double time );

    /**
     * see Pulse::SetTPOIs()
     */
    virtual void    SetTPOIs  ();


 protected:
    /**
     * Get informations on this Pulse
     *
     * @return Infos for display.
     */
    virtual string          GetInfo        ();

    double       m_zeros;         /**< @brief Half the number of zero crossings of the sinc                                   */
    double       m_max_amplitude; /**< @brief Maximum amplitude of the sinc; derived from flipangle and duration in Prepare() */
    double       m_alpha;         /**< @brief Apodization factor (default 0.5 = Hamming window)                               */

};

#endif /*SINCRFPULSE_H_*/
