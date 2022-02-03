/** @file DynamicVariables.h
 * @brief Implementation of JEMRIS DynamicVariables
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2019  Tony Stoecker
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

#ifndef DYNAMICVARIABLES_H_
#define DYNAMICVARIABLES_H_

using namespace std;
#include <vector>
#include "Trajectory.h"

//MODIF
class TrajectoryFlow;
//MODIF***
//Mod
class TrajectoryRespiration;
//
class TrajectoryMotion;
class TrajectoryT2s;
class Trajectory1D;
class TrajectoryEmpty;

class TrajectoryDiffusion;
class Trajectory;


/**
 *@brief circle for dynamic variables
 */
struct Circle{
	double x;	/**<@brief x position of circle center */
	double y;	/**<@brief y position of circle center */
	double z;	/**<@brief z position of circle center */
	double r;	/**<@brief radius of the circle */
};
/**
 * @brief Instance class holding dynamic sample properties
 *
 */
class DynamicVariables {

 public:

     /**
      * @brief Default destructor
      */
     virtual ~DynamicVariables   ();

    /**
     * @brief Get sole instance of DynamicVariables
     */
    static DynamicVariables*  instance      ();

    /**
     * @brief set appropriate strategy for current spin (active/non_active)
     */
    void SetActivation();

    /**
     * @brief spins within this circle are simulated with time-varying T2/T2s.
     * as many active circles as needed may be added.
     */
    void AddActiveCircle(double pos[3],double radius);

//MODIF
    Trajectory* m_Flow;
//MODIF***

//Mod
    Trajectory* m_Respiration;
//
    Trajectory* m_Motion;
    Trajectory* m_T2prime;

    Trajectory* m_R1;
    Trajectory* m_R2;
    Trajectory* m_M0;

    Trajectory* m_Diffusion;

 private:

    /**
     * @brief Default constructor
     */
	 DynamicVariables  () {};

    static DynamicVariables*    m_instance;           /**< @brief Pointer to the sole instance of this implementation */

    // upon activation: change T2 and/or T2prime?
    bool m_changeT2prime;
    bool m_changeT2;

    vector<Circle> m_Circles;


    /**
     * pool of available Trajectories to use with copy constructor of context class "Trajectory"
     */
//MODIF
    TrajectoryFlow* stub_flow;
//MODIF***
//Mod
    TrajectoryRespiration* stub_respiration;
//
    TrajectoryMotion* stub_motion;
    TrajectoryT2s* stub_T2s;
    Trajectory1D* stub_r1;
    Trajectory1D* stub_r2;
    Trajectory1D* stub_m0;
    TrajectoryEmpty* stub_empty;
    TrajectoryDiffusion* stub_diff;


};

#endif /* DYNAMICVARIABLES_H_ */
