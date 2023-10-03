/** @file TrajectoryMotion.h
 * @brief Implementation of TrajectoryMotion
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#ifndef TRAJECTORYMOTION_H_
#define TRAJECTORYMOTION_H_

#include "TrajectoryInterface.h"

/**
 * @brief Motion Trajectory
 */
class TrajectoryMotion : public TrajectoryInterface {
public:
	TrajectoryMotion();
	virtual ~TrajectoryMotion();


	virtual void GetValueDerived(double time, double *value) ;

	virtual void LoadFile(string filename) ;

private:
	void GetPosition(double time, double &trans_x, double &trans_y, double &trans_z, double &rot_x, double &rot_y, double &rot_z);
	vector<double> m_trans_x,m_trans_y,m_trans_z;
	vector<double> m_rot_x,m_rot_y,m_rot_z;

	bool btx, bty, btz, brx, bry, brz;					/* flags==true: there is data!=0 on this axis  */

	double m_rot_origin_x, m_rot_origin_y, m_rot_origin_z;	/* point araound which to rotate; default: origin */


};

#endif /* TRAJECTORYMOTION_H_ */
