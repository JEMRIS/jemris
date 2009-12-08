/*
 * TrajectoryEmpty.h
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORYEMPTY_H_
#define TRAJECTORYEMPTY_H_

#include "TrajectoryInterface.h"

class TrajectoryEmpty: public TrajectoryInterface {
public:
	TrajectoryEmpty(){};
	virtual ~TrajectoryEmpty(){};

	virtual void GetValueDerived(double time, double *value) {return;};

	virtual void LoadFile(string filename) {return;};

};

#endif /* TRAJECTORYEMPTY_H_ */
