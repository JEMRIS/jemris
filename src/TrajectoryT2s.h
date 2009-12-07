/*
 * TrajectoryT2s.h
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORYT2S_H_
#define TRAJECTORYT2S_H_

#include "Trajectory1D.h"

class TrajectoryT2s : public Trajectory1D {
public:
	TrajectoryT2s();
	virtual ~TrajectoryT2s();

	virtual void GetValueDerived(double time, double *deltaB );

};

#endif /* TRAJECTORYT2S_H_ */
