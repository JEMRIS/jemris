/*
 * Mircostructure.cpp
 *
 *  Created on: Dec 10, 2009
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

#include "Microstructure.h"

Microstructure::Microstructure() {
	m_boxsize=0.05;
	m_D_external=0.7e-6;  //mm^2/ms

	m_rng = NULL;

}

Microstructure::~Microstructure() {
}
/**************************************************/
triple Microstructure::PeriodicBoundary(triple trialpos) {
	double edge=2*m_boxsize;
	triple pos;
	pos.x = fmod(trialpos.x+m_boxsize , edge)-m_boxsize;
	pos.y = fmod(trialpos.y+m_boxsize , edge)-m_boxsize;
	pos.z = fmod(trialpos.z+m_boxsize , edge)-m_boxsize;
	return pos;
}
