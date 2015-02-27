/** @file IdealCoil.h
 *  @brief Implementation of JEMRIS IdealCoil
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#ifndef IDEALCOIL_H_
#define IDEALCOIL_H_

#include "Coil.h"

/**
 * @brief Prototype of an ideal coil
 */
class IdealCoil : public Coil {

 public:

    /**
     * @brief Default constructor
     */
	IdealCoil() {};

	/**
	 * @brief Default destructor
	 *
	 * @return
	 */
	~IdealCoil() {};

	/**
	 * @brief  Clone an ideal coil
	 *
	 * @return Cloned ideal coil
	 */
	inline virtual IdealCoil* Clone() const { return (new IdealCoil(*this)); };

	/**
	 * @brief Get sensitivity
	 *
	 * @param  position Spin position.
	 * @return Ideal coil delivers 1.0 at every point.
	 */
	virtual double GetSensitivity (const double* position) {return 1.0;};

};

#endif /*IDEALCOIL_H_*/
