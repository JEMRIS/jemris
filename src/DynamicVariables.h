/** @file DynamicVariables.h
 * @brief Implementation of JEMRIS DynamicVariables
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */

#ifndef DYNAMICVARIABLES_H_
#define DYNAMICVARIABLES_H_

using namespace std;
#include <vector>

class TrajectoryMotion;
class TrajectoryT2s;
class Trajectory1D;
class TrajectoryEmpty;
class Trajectory;

struct Circle{
	double x;
	double y;
	double z;
	double r;
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

    // upon activation: change T2 and/or T2prime?
    bool m_changeT2prime;
    bool m_changeT2;

    vector<Circle> m_Circles;


    /**
     * pool of available Trajectories to use with copy constructor of context class "Trajectory"
     */
    TrajectoryMotion* stub_motion;
    TrajectoryT2s* stub_T2s;
    Trajectory1D* stub_r1;
    Trajectory1D* stub_r2;
    Trajectory1D* stub_m0;
    TrajectoryEmpty* stub_empty;


};

#endif /* DYNAMICVARIABLES_H_ */
