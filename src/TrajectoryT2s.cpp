/*
 * TrajectoryT2s.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
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

