/** @file ExternalCoil.h
 *  @brief Implementation of JEMRIS ExternalCoil
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
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

#ifndef EXTERNALCOIl_H_
#define EXTERNALCOIl_H_

#include "Coil.h"

/**
 * @brief Prototype of an ideal coil
 */
class ExternalCoil : public Coil {

 public:

    /**
     * @brief Default constructor
     */
	ExternalCoil() {};

	/**
	 * @brief Default destructor
	 */
	~ExternalCoil();

    /**
     * @brief Prototype cloning mechanism
     *
     * @return An unprepared clone
     */
	virtual ExternalCoil* Clone() const;

    /**
     * @brief Get the B1+ magnitude at point (x,y,z)
     *
     * This method must be implemented by every derived coil.
     *
     * @param position  At position.
     * @return          Sensitivity with respect to spin in World.
     */
	virtual double GetSensitivity (double* position);

    /**
     * @brief Get the B1+ phase at point (x,y,z)
     *
     * This method may be implemented by every derived coil. Otherwise phase is zero.
     *
     * Important: the phase of Coils needs to be implemented with unit radians!
     * (In contrast to the phase of RF pulses which has units degrees.)
     *
     * @param position  At position.
     * @return          B1+ phase with respect to spin in World.
     */
	virtual double GetPhase (double* position);

    /**
     * @brief Prepare a coil with provided sensitivity map.
     *        Called twice one for initialisation once for configuration.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE}.
     * @return     Success
     */
    virtual bool Prepare  (PrepareMode mode);

 private:

     string    m_fname;        /**< @brief URI of the sensitivity map  */

};
#endif /*EXTERNALCOIl_H_*/
