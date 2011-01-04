/** @file TrajectoryInterface.h
 * @brief Implementation of JEMRIS TrajectoryInterface
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

/**
 * @brief pure virtual base class for all Trajectories
 */

class TrajectoryInterface {
public:
	TrajectoryInterface();
	virtual ~TrajectoryInterface();

	/*
	 * checks if data available; then call GetValueDerived:
	 */
	void GetValue(double time, double *value) ;

	/**
	 * @brief retrieve value at given time-point
	 */
	virtual void GetValueDerived(double time, double *value) =0;

	/**
	 * @brief load trajectory from file
	 */
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
