/*
 * TrajectoryInterface.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */

#include "TrajectoryInterface.h"

TrajectoryInterface::TrajectoryInterface() {
	m_LastHuntIndex=0;
}

TrajectoryInterface::~TrajectoryInterface() {
}

void TrajectoryInterface::GetValue(double time, double*value){
	if (m_time.size()>0)
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
		cout << "Interpolation out of bounds! exit."<< endl;
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
			iHuntStep << 1;
			ihi = ilo + iHuntStep;
			if (ihi > iend) ihi = iend;
		}
	} else {
		// hunt down:
		ihi = m_LastHuntIndex;
		ilo = ihi - 1;
		while (m_time[ilo] > t ) {
			ihi = ilo;
			iHuntStep << 1;
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
