/** @file World.h
 *  @brief Implementation of JEMRIS World
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#ifndef WORLD_H_
#define WORLD_H_

#include "Declarations.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>


using namespace std;

class AtomicSequence;
class Trajectory;

//! The simulated reality

class World {

 public:

     /**
      * @brief Default destructor
      */
     virtual ~World   () { m_instance=0; };

    /**
     * @brief Get sole instance of the sequence tree
     */
    static World*  instance      ();

    /**
     * @brief    Get concomitant field term for the current gradients.
     *
     * @param  G Current gradients
     * @return   Concomitant field term for the current gradients.
     */
    double ConcomitantField(double* G);

    /**
     * @brief Pointer to the evolution saving function
     */
    void      (*saveEvolFunPtr)(long, bool)  ;

    void*             solverSettings ;      /**< @brief Arbitrary solver settings  */

    long              SpinNumber;		    /**< @brief Number of the current spin*/
    long              TotalSpinNumber;      /**< @brief Total number of spins*/
    long              TotalADCNumber;       /**< @brief Total number of spins*/

    double*           Values;               /**< @brief Values of the current spin (position and physical parameters) */
    double            InitValues[8];        /**< @brief For initialization of Values */
    double            time;                 /**< @brief Current time point (in atom) */
    double            total_time;           /**< @brief Current time point (absolut time) */
    double            phase;                /**< @brief Receiver phase taken from the TPOIs*/
    double            PhaseLock;            /**< @brief Locked Phase (the phase set by the last RF pulse)*/
    double            deltaB;               /**< @brief Any off-resonance terms*/
    double            solution[3];          /**< @brief Solution [M_r, phi, M_z] at the current time point*/

    double			  RandNoise;            /**< @brief percentage of random noise added to the signal */
    double            GMAXoverB0;           /**< @brief Constant for the concomittant field term */
    double            NonLinGradField;      /**< @brief Non-linear contriution to B_z from gradients */

    //members for saving the magnetisation evolution of the spins
    int	              saveEvolStepSize;     /**< @brief Step-size (in numbers of ADC) at which the evolution is stored */
    string            saveEvolFileName;     /**< @brief Filename in which the evolution is stored */
    ofstream*         saveEvolOfstream;     /**< @brief Output stream for saving the evolutions */

    AtomicSequence*   pAtom;                /**< @brief Atomic sequence repsonsible for the current time point*/
    AtomicSequence*   pStaticAtom;          /**< @brief Atomic sequence repsonsible for all time points*/

    int 			  m_myRank;				/**< @brief MPI rank of this process. if m_myRank<0 process is serial jemris */
    bool			  m_useLoadBalancing;	/**< @brief use load balancing (send sample in small packages top slaves) */

    Trajectory*		  m_VarT2Prime;			/**< @brief factor by which T2Prime is scaled over time */


 private:

    /**
     * @brief Default constructor
     */
    World  () {};

    static World*    m_instance;           /**< @brief Pointer to the sole instance of this implementation */

};

#endif /*WORLD_H_*/
