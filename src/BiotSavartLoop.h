/** @file BiotSavartLoop.h
 *  @brief Implementation of JEMRIS BiotSavartLoop
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

    double            m_radius;        /**< @brief My loop radius                  */
	double            m_maxsens;       /**< @brief maximum sensitivity             */
	bool              m_havemax;       /**< @brief maximum sensitivity calculated? */


};

#endif /*BIOTSAVARTLOOP_H_*/
