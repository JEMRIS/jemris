/*
 * Mircostructure.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: dpflug
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
