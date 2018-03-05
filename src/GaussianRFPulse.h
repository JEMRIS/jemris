/** @file GaussianRFPulse.h
 *  @brief Implementation of JEMRIS GaussianRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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

#ifndef GAUSSIANRFPULSE_H_
#define GAUSSIANRFPULSE_H_

#include "RFPulse.h"

/**
 * @brief Gaussian shaped radio frequency pulses
 */

class GaussianRFPulse : public RFPulse {

 public:

    /**
     * @brief Default constructor
     */
    GaussianRFPulse() {};

    /**
     * @brief Default copy constructor.
     */
    GaussianRFPulse  (const GaussianRFPulse&) {};

    /**
     * @brief Default destructor.
     */
    ~GaussianRFPulse () {};

    /**
     * @see Prototype::Clone()
     */
    inline GaussianRFPulse* Clone() const { return (new GaussianRFPulse(*this)); };

    /**
     * @brief Prepare the hard RF pulse.
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool        Prepare            (PrepareMode mode)  ;

    /**
     * @brief Returns the Gaussian Magnitidue for all times.
     *
     * @param time The flip angle as double.
     * @return the Magntidue.
     */
    virtual double    GetMagnitude  (double time );

    /**
     * @see Pulse::SetTPOIs()
     */
    virtual void    SetTPOIs  () ;


 protected:

    double m_max_amplitude; /**< @brief Maximum aplitude */

};

#endif
