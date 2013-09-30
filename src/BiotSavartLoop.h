/** @file BiotSavartLoop.h
 *  @brief Implementation of JEMRIS BiotSavartLoop
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

#ifndef BIOTSAVARTLOOP_H_
#define BIOTSAVARTLOOP_H_

#include "Coil.h"

/**
 * @brief Prototype of a Biot-Savart loop
 */
class BiotSavartLoop : public Coil {

 public:


	/**
	 * @brief Default constructor
	 */
	BiotSavartLoop  () {};

	/**
	 * @brief Default destructor
	 */
	~BiotSavartLoop () {};

	/**
	 * @brief  Request sensitivity at given position
	 *
	 * @param  position Position.
	 * @return          Sensitivity at requested position.
	 */
	virtual double GetSensitivity (double* position);

	/**
	 * @brief  Request sensitivity at given position
	 *
	 * @param  position Position.
	 * @return          Sensitivity at requested position.
	 */
	inline double GetPhase (double* position) {return this->m_biosavart_phase;};

	/**
	 * @brief Clone.
	 *
	 * @return A clone.
	 */
	inline virtual BiotSavartLoop* Clone () const {return (new BiotSavartLoop(*this));};

    /**
     * @brief Prepare.
     *
     * @param  mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return      Success
     */
    virtual bool Prepare (PrepareMode mode);


 private:

    double            m_mask;               /**< Torus radius to mask field on the wire */
    double            m_radius;             /**< Loop radius */
    double            m_biosavart_phase;    /**< @brief Phase */
    
};

#endif /*BIOTSAVARTLOOP_H_*/
