/*
 * MicrostructureCrossingFiber.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: dpflug
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
