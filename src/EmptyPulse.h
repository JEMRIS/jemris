/** @file EmptyPulse.h
 *  @brief Implementation of JEMRIS EmptyPulse
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

#ifndef EMPTYPULSE_H_
#define EMPTYPULSE_H_

#include "Pulse.h"
#include "AnalyticPulseShape.h"


//! Prototype of an empty pulse

class EmptyPulse : public Pulse {

 public:

    /**
     * @brief Default constructor
     */
    EmptyPulse           () {};

    /**
     * @brief Default destructor.
     */
    ~EmptyPulse          () {};

    /**
     * @brief Default copy constructor.
     */
    EmptyPulse           (const EmptyPulse&) { m_pulse_shape.SetPulse(this); };

    /**
     *  See Prototype::Clone.
     */
    inline EmptyPulse*         Clone() const { return (new EmptyPulse(*this)); };

   /**
     * @brief Prepare the hard RF pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return     Success.
     */
    virtual bool Prepare (PrepareMode mode);

    /**
     * @brief Allows non-equdistant setting of ADCs via Shape attribute (GiNaC formula)
     * @see Pulse::SetTPOIs()
     */
    virtual void    SetTPOIs  ();

    /**
     * @see Pulse::GenerateEvents()
     */
    virtual void GenerateEvents(std::vector<Event*> &events);

 protected:
   
    /**
     * Get informations
     *
     * @return Infos for display.
     */
    virtual string         GetInfo        ();


    AnalyticPulseShape	   m_pulse_shape; /**<conputes pulse shape*/

};

#endif /*HARDRFPULSE_H_*/
