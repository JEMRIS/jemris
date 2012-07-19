/*
 * Trajectory.cpp
 *
 *  Created on: Feb 27, 2009
 *      Author: dpflug
 */

#include "Trajectory.h"
#include "TrajectoryInterface.h"

void Trajectory::GetValue(double time, double *value){
	m_strategy->GetValue(time, value);
};


void Trajectory::LoadFile(string filename){
	m_strategy->LoadFile(filename);
};


void Trajectory::UpdateTrajectory(bool init){
	m_strategy->UpdateTrajectory(init);
};



