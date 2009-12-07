/*
 * TrajectoryInterface.h
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */
#ifndef TRAJECTORYINTERFACE_H_
#define TRAJECTORYINTERFACE_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "World.h"

using namespace std;

class TrajectoryInterface {
public:
	TrajectoryInterface();
	virtual ~TrajectoryInterface();

	/*
	 * checks if data available; then call GetValueDerived:
	 */
	void GetValue(double time, double *value) ;

	virtual void GetValueDerived(double time, double *value) =0;

	virtual void LoadFile(string filename) =0;

protected:
	/**
	 * @brief find data index for interpolation
	 * get lower index for given timepoint using hunt search algorithm.
	 */
	int GetLowerIndex(double time);

	int m_LastHuntIndex;

	vector<double> m_time;

};

#endif /* TRAJECTORYINTERFACE_H_ */
