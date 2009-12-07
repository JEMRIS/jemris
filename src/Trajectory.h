/*
 * Trajectory.h
 *
 *  Created on: Feb 27, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include <string>
using namespace std;
class TrajectoryInterface;


/*
 * Context class for trajectories (design pattern: Strategy)
 */
class Trajectory {
private:
	TrajectoryInterface *m_strategy;

public:
	Trajectory(TrajectoryInterface *strategy):m_strategy(strategy) {}

	void GetValue(double time, double *value);

	void LoadFile(string filename);
};

#endif /* TRAJECTORY_H_ */
