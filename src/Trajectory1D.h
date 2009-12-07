/*
 * Trajectory1D.h
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORY1D_H_
#define TRAJECTORY1D_H_

#include "TrajectoryInterface.h"

class Trajectory1D: public TrajectoryInterface {
public:
	Trajectory1D();
	virtual ~Trajectory1D();

	virtual void LoadFile(string filename);

	virtual void GetValueDerived(double time, double * values);

private:
	vector<double> m_data;

protected:
	virtual void LinearInterpolation(double time, double *value);

};

#endif /* TRAJECTORY1D_H_ */
