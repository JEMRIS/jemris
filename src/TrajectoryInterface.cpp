/*
 * TrajectoryInterface.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
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

#include "TrajectoryInterface.h"

TrajectoryInterface::TrajectoryInterface() {
	m_LastHuntIndex=0;
//MODIF
	m_currentSpinIndex=0;
	m_trajLoopDuration=0;
	m_trajLoopNumber=0;
	m_spinActive=true;
//MODIF***
}

TrajectoryInterface::~TrajectoryInterface() {
}

void TrajectoryInterface::GetValue(double time, double*value){
	if (m_time.size()>0 || m_time_full.size()>0)    //MODIF
		GetValueDerived(time,value);

}


/***********************************************************/

int TrajectoryInterface::GetLowerIndex(double t) {
	int ihi;
	int ilo;

	// test bounds:
	if ((t<= m_time[0]) || (t>=m_time.back())) {
		if (t == m_time[0]) {ilo = 0;  return ilo;};
		if (t == m_time.back()) {ilo = m_time.size()-2;  return ilo;};
		cout << "Interpolation out of bounds! exit.(t= "<<t<<"; m_time[0]="<<m_time[0]<<"; m_time.back()="<<m_time.back()<< endl;
		exit(-1);
	}

	// hunt phase:
	int iHuntStep = 1;
	int iend = m_time.size()-1;

	if (m_time[m_LastHuntIndex]<t) {
		// hunt up:
		ilo = m_LastHuntIndex;
		ihi = ilo + 1;
		while (m_time[ihi] < t ) {
			ilo = ihi;
			//iHuntStep << 1;
			ihi = ilo + iHuntStep;
			if (ihi > iend) ihi = iend;
		}
	} else {
		// hunt down:
		ihi = m_LastHuntIndex;
		ilo = ihi - 1;
		while (m_time[ilo] > t ) {
			ihi = ilo;
			//iHuntStep << 1;
			ilo = ihi - iHuntStep;
			if (ilo < 0 ) ilo = 0;
		}
	}
	// bisection phase:
	int middle;
	while ( (ihi-ilo) > 1) {
		middle=(ihi+ilo) >> 1;
		if (m_time[middle] > t) ihi = middle; else ilo = middle;
	}

	m_LastHuntIndex = ilo;
	return ilo;
}
/****************************************************************/
