/** @file Trajectory.h
 * @brief Implementation of JEMRIS Trajectory
 *
 *  Created on: Feb 27, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include <string>
#include <iostream>
using namespace std;
class TrajectoryInterface;


/**
 * @brief Context class for trajectories (design pattern: Strategy)
 */
class Trajectory {
private:
	TrajectoryInterface *m_strategy;

public:
	Trajectory(TrajectoryInterface *strategy):m_strategy(strategy) {}

	void GetValue(double time, double *value);

	void LoadFile(string filename);

	void SetStrategy(TrajectoryInterface *new_strategy) {m_strategy = new_strategy;};

	void UpdateTrajectory(bool init=false);

	TrajectoryInterface* GetStrategy(){cout << "THIS IS A DEBUG FUNCTION!" <<endl; return m_strategy;};
};

#endif /* TRAJECTORY_H_ */
