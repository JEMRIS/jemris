/** @file TrajectoryT2s.h
 * @brief Implementation of JEMRIS TrajectoryT2s
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
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

#ifndef TRAJECTORYT2S_H_
#define TRAJECTORYT2S_H_

#include "Trajectory1D.h"

/**
 * @brief Strategy for T2s; needs some extra handling for GetValueDerived.
 */
class TrajectoryT2s : public Trajectory1D {
public:
	TrajectoryT2s();
	virtual ~TrajectoryT2s();

	virtual void GetValueDerived(double time, double *deltaB );

};

#endif /* TRAJECTORYT2S_H_ */
