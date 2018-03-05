/*
 * MicrostructureCrossingFiber.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: dpflug
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

#include "MicrostructureCrossingFiber.h"

MicrostructureCrossingFiber::MicrostructureCrossingFiber() {
}

/***********************************************************/
MicrostructureCrossingFiber::~MicrostructureCrossingFiber() {
}

/***********************************************************/
void MicrostructureCrossingFiber::SetSecondFiberOrientation(double dx,double dy,double dz) {
	double norm;
	norm = sqrt(dx*dx + dy*dy + dz*dz);
	if (norm == 0) {
		cout << "invalid fiber orientation! (dx=" << dx <<", dy="<<dy<<", dz="<<dz<<"). Exit!" << endl;
		exit(-1);
	}
	m_2_fiber_orientation[0]=dx/norm;
	m_2_fiber_orientation[1]=dy/norm;
	m_2_fiber_orientation[2]=dz/norm;
};
/***********************************************************/
void MicrostructureCrossingFiber::SetCylOrientation(neuron &tmp) {
	if (m_rng->uniform()<0.5) {
		tmp.dx = m_fiber_orientation[0];tmp.dy = m_fiber_orientation[1];tmp.dz = m_fiber_orientation[2];
	} else {
		tmp.dx = m_2_fiber_orientation[0];tmp.dy = m_2_fiber_orientation[1];tmp.dz = m_2_fiber_orientation[2];
	}
}
