/*
 * MicrostructureCrossingFiber.h
 *
 *  Created on: Jan 25, 2010
 *      Author: dpflug
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

#ifndef MICROSTRUCTURECROSSINGFIBER_H_
#define MICROSTRUCTURECROSSINGFIBER_H_

#include "MicrostructureSingleFiber.h"

/**
 * @brief Microstructure consisting of crossing fibers
 */
class MicrostructureCrossingFiber: public MicrostructureSingleFiber {
public:
	MicrostructureCrossingFiber();
	virtual ~MicrostructureCrossingFiber();

	void SetSecondFiberOrientation(double dx,double dy,double dz);

protected:
	virtual void SetCylOrientation(neuron &tmp);
	double m_2_fiber_orientation[3];
// uncomment next line to allow neurons to overlap spatially
//	virtual bool CylinderCollide(const neuron &test) {return false;}



};

#endif /* MICROSTRUCTURECROSSINGFIBER_H_ */
