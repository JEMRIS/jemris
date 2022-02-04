/*
 * DynamicVariables.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C)
 *                        2006-2022  Tony Stoecker
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

#include "DynamicVariables.h"
#include "TrajectoryMotion.h"
//MODIF
#include "TrajectoryFlow.h"
//MODIF***
//Mod
#include "TrajectoryRespiration.h"
//
#include "Trajectory.h"
#include "TrajectoryT2s.h"
#include "Trajectory1D.h"
#include "TrajectoryEmpty.h"
#include "TrajectoryDiffusion.h"

DynamicVariables* DynamicVariables::m_instance=0;

/***********************************************************/
DynamicVariables* DynamicVariables::instance() {

    if (m_instance == 0) {

    	m_instance = new DynamicVariables();
    	//init stubs:
//MODIF
        m_instance->stub_flow = new TrajectoryFlow();
//MODIF***
//Mod
        m_instance->stub_respiration = new TrajectoryRespiration();
//
    	m_instance->stub_motion = new TrajectoryMotion();
    	m_instance->stub_T2s = new TrajectoryT2s();
    	m_instance->stub_r1 = new Trajectory1D();
    	m_instance->stub_r2 = new Trajectory1D();
    	m_instance->stub_m0 = new Trajectory1D();
    	m_instance->stub_empty = new TrajectoryEmpty();
    	m_instance->stub_diff = new TrajectoryDiffusion();


    	// init Trajectories
//MODIF
   	    m_instance->m_Flow = new Trajectory(m_instance->stub_flow);
//MODIF***
//Mod
        m_instance->m_Respiration = new Trajectory(m_instance->stub_respiration);
//
   	    m_instance->m_Motion = new Trajectory(m_instance->stub_motion);
  	    m_instance->m_T2prime = new Trajectory(m_instance->stub_T2s);

  	    m_instance->m_R1 = new Trajectory(m_instance->stub_r1);
  	    m_instance->m_R2 = new Trajectory(m_instance->stub_r2);
  	    m_instance->m_M0 = new Trajectory(m_instance->stub_m0);
  	    m_instance->m_Diffusion = new Trajectory(m_instance->stub_diff);


  	    m_instance->m_changeT2=true;
  	    m_instance->m_changeT2prime=true;
   }

    return m_instance;

}

/***********************************************************/
DynamicVariables::~DynamicVariables() {
	m_instance=0;
	delete m_Motion;
	//MODIF
	delete m_Flow;
	//MODIF***
	//Mod
	delete m_Respiration;
	//
	delete m_T2prime;
	delete m_R1;
	delete m_R2;
	delete m_M0;
	delete m_Diffusion;

	//delete stubs:
	delete stub_motion;
	//MODIF
	delete stub_flow;
	//MODIF***
	//Mod
	delete stub_respiration;
	//
	delete stub_T2s;
	delete stub_r1;
	delete stub_r2;
	delete stub_m0;
	delete stub_empty;
	delete stub_diff;
}
/***********************************************************/
void DynamicVariables::AddActiveCircle(double pos[3],double radius) {

	Circle circ;
	circ.x=pos[0];
	circ.y=pos[1];
	circ.z=pos[2];
	circ.r=radius;
	m_Circles.push_back(circ);

}
/***********************************************************/
void DynamicVariables::SetActivation() {
	World* pw=World::instance();

    double x=pw->Values[XC];
    double y=pw->Values[YC];
    double z=pw->Values[ZC];

	bool activated=false;

	for (unsigned int i=0;i < m_Circles.size(); i++) {
		if (m_Circles[i].r*m_Circles[i].r > ((m_Circles[i].x-x)*(m_Circles[i].x-x) + (m_Circles[i].y-y)*(m_Circles[i].y-y) + (m_Circles[i].z-z)*(m_Circles[i].z-z) )  ){
			activated=true;
			break;
		}
	}

	if (m_changeT2) {
		if (activated)
			m_R2->SetStrategy(stub_r2);
		else
			m_R2->SetStrategy(stub_empty);
	}

	if (m_changeT2prime) {
		if (activated)
			m_T2prime->SetStrategy(stub_T2s);
		else
			m_T2prime->SetStrategy(stub_empty);
	}

}


