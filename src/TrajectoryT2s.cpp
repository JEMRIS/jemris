/*
 * TrajectoryT2s.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
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

#include "TrajectoryT2s.h"

TrajectoryT2s::TrajectoryT2s() {
}

TrajectoryT2s::~TrajectoryT2s() {
}

/***********************************************************/
void TrajectoryT2s::GetValueDerived(double time, double *deltaB) {

	World* pw=World::instance();

	double offB=0.001*pw->Values[DB];
	double data;
	LinearInterpolation(time,&data);

	deltaB[0] = (deltaB[0]-offB)*data + offB;

	return;

}

