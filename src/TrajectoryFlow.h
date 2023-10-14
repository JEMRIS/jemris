//MODIF
/** @file TrajectoryFlow.h
 * @brief Implementation of TrajectoryFlow
 *
 *  Created on: feb 26, 2014
 *      Author: afortin, in collaboration with edurand (adapted from dpflug's TrajectoryMotion class)
 */


/*
 *  JEMRIS Copyright (C)
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#ifndef TRAJECTORYFLOW_H_
#define TRAJECTORYFLOW_H_

#include "TrajectoryInterface.h"

/**
 * @brief Flow Trajectory
 */
class TrajectoryFlow : public TrajectoryInterface {
public:
	TrajectoryFlow();
	virtual ~TrajectoryFlow();

    virtual void GetValueDerived(double time, double *value) ;

	virtual void LoadFile(string filename) ;

/*	void setCurrentTraj(long currentTraj);// { m_currentSpinIndex=currentTraj; };

	long getCurrentTraj();// { return m_currentSpinIndex; };

	void setLoop(double loopDuration, long loopTrajNumber);// { m_trajLoopNumber=loopTrajNumber; m_trajLoopDuration=loopDuration; };*/

private:
//MODIF
    void LoadTrajectoriesHDF5(string filename);

	void LoadTrajectoriesASCII(string filename);

	void GetPosition(double time, double &trans_x, double &trans_y, double &trans_z, long traj_number);

    int GetLowerIndex(double t, vector<double> timeArray, int LastHuntIndex);

    int m_LastHuntIndexActivation;

    long m_TotalTrajNumber;

    vector< pair< vector<double> , vector<bool> > > m_activation_full;

    vector<double> m_activation_time;

    vector<bool> m_activation_state;

    vector< vector<float> > m_trans_x_full,m_trans_y_full,m_trans_z_full;

	vector<float> m_trans_x,m_trans_y,m_trans_z;
	//vector<double> m_rot_x,m_rot_y,m_rot_z;


	//bool btx, bty, btz, brx, bry, brz;					/* flags==true: there is data!=0 on this axis  */

	//float m_rot_origin_x, m_rot_origin_y, m_rot_origin_z;	/* point araound which to rotate; default: origin */
//MODIF***
};

#endif /* TRAJECTORYFLOW_H_ */
//MODIF***
