/*
 * DynamicVariables.h
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */

#ifndef DYNAMICVARIABLES_H_
#define DYNAMICVARIABLES_H_

class TrajectoryMotion;
class TrajectoryT2s;
class Trajectory1D;
class Trajectory;

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

    Trajectory* m_Motion;
    Trajectory* m_T2prime;

    Trajectory* m_R1;
    Trajectory* m_R2;
    Trajectory* m_M0;


 private:

    /**
     * @brief Default constructor
     */
	 DynamicVariables  () {};

    static DynamicVariables*    m_instance;           /**< @brief Pointer to the sole instance of this implementation */

    /**
     * pool of available Trajectories to use with copy constructor of context class "Trajectory"
     */
    TrajectoryMotion* stub_motion;
    TrajectoryT2s* stub_T2s;
    Trajectory1D* stub_r1;
    Trajectory1D* stub_r2;
    Trajectory1D* stub_m0;

};

#endif /* DYNAMICVARIABLES_H_ */
