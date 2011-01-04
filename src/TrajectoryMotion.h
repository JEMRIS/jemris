/** @file TrajectoryMotion.h
 * @brief Implementation of TrajectoryMotion
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
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
