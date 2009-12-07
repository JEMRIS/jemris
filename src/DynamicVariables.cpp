/*
 * DynamicVariables.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */

#include "DynamicVariables.h"
#include "TrajectoryMotion.h"
#include "Trajectory.h"
#include "TrajectoryT2s.h"
#include "Trajectory1D.h"

DynamicVariables* DynamicVariables::m_instance=0;

/***********************************************************/
DynamicVariables* DynamicVariables::instance() {

    if (m_instance == 0) {

    	m_instance = new DynamicVariables();
    	//init stubs:
    	m_instance->stub_motion = new TrajectoryMotion();
    	m_instance->stub_T2s = new TrajectoryT2s();
    	m_instance->stub_r1 = new Trajectory1D();
    	m_instance->stub_r2 = new Trajectory1D();
    	m_instance->stub_m0 = new Trajectory1D();

    	// init Trajectories
   	    m_instance->m_Motion = new Trajectory(m_instance->stub_motion);
  	    m_instance->m_T2prime = new Trajectory(m_instance->stub_T2s);

  	    m_instance->m_R1 = new Trajectory(m_instance->stub_r1);
  	    m_instance->m_R2 = new Trajectory(m_instance->stub_r2);
  	    m_instance->m_M0 = new Trajectory(m_instance->stub_m0);

   }

    return m_instance;

}

/***********************************************************/
DynamicVariables::~DynamicVariables() {
	m_instance=0;
	delete m_Motion;
	delete m_T2prime;
	delete m_R1;
	delete m_R2;
	delete m_M0;

	//delete stubs:
	delete stub_motion;
	delete stub_T2s;
	delete stub_r1;
	delete stub_r2;
	delete stub_m0;
}
