/** @file SechRFPulse.h
 *  @brief Implementation of JEMRIS SechRFPulse
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

#ifndef SECHRFPULSE_H_
#define SECHRFPULSE_H_

#include "RFPulse.h"

/**
 * @brief Sech shaped radio frequency pulses
 */

class SechRFPulse : public RFPulse {

 public:

    /**
     * @brief Default constructor
     */
    SechRFPulse() {};

    /**
     * @brief Default copy constructor.
     */
    SechRFPulse  (const SechRFPulse&);

    /**
     * @brief Default destructor.
     */
    ~SechRFPulse () {};

    /**
     * @see Prototype::Clone()
     */
    inline SechRFPulse* Clone() const { return (new SechRFPulse(*this)); };

    /**
     * @brief Prepare the hard RF pulse.
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool        Prepare            (PrepareMode mode)  ;

    /**
     * @brief Returns the Sech Magnitidue for all times.
     *
     * @param time The flip angle as double.
     * @return the Magntidue.
     */
    virtual double    GetMagnitude  (double time );

    /**
     * @see Pulse::SetTPOIs()
     */
    virtual void    SetTPOIs  () ;

    virtual double GetPhase() { return m_sech_phase;};
    static double  GetSechPhase (Module* mod, double time ) { return (((SechRFPulse*) mod)->GetPhase()  ); };

 protected:

    double m_max_amplitude; /**< @brief Maximum amplitude */
    double m_sech_phase;

};

#endif
