/** @file BiotSavartLoop.h
 *  @brief Implementation of JEMRIS BiotSavartLoop
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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
	BiotSavartLoop  () : m_mask(0), m_biosavart_phase(0.), m_radius(0.), m_first_call(true) {};

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
	virtual double GetSensitivity (const double* position);

	/**
	 * @brief  Request sensitivity at given position
	 *
	 * @param  position Position.
	 * @return          Sensitivity at requested position.
	 */
	inline double GetPhase (const double* position) {return this->m_biosavart_phase;};

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
    virtual bool Prepare (const PrepareMode mode);


 private:

    double            m_mask;               /**< Torus radius to mask field on the wire */
    double            m_radius;             /**< Loop radius */
    double            m_biosavart_phase;    /**< @brief Phase */
    bool			  m_first_call;			/**< @brief print warning on first call if no boost available */
    
};

#endif /*BIOTSAVARTLOOP_H_*/
