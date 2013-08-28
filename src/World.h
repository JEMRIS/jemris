/** @file World.h
 *  @brief Implementation of JEMRIS World
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
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

#ifndef WORLD_H_
#define WORLD_H_

#include "Declarations.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <vector>

using namespace std;

class AtomicSequence;

//! The simulated reality

class World {

 public:

     /**
      * @brief Default destructor
      */
	virtual ~World   ();

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
    double ConcomitantField (double* G);

	/**
	 * @brief    Set number of spinproperties
	 *
	 * @param n  Number of spin properties
	 */
	void   SetNoOfSpinProps (int n); 

	/**
	 * @brief    Get number of spin properties
	 *
	 * @return   Number of spin properties
	 */
	int    GetNoOfSpinProps () {return m_noofspinprops;};


	/**
	 * @brief  Reference to helper array
	 */
	double*    Helper () {
		return &helper[0];
	};


	/**
	 * @brief    Initilize helper array
	 *
	 * @param size Size of helper array
	 */
	void InitHelper (long size);


	/**
	 * @brief    Number of compartments
	 *
	 * @return   Number of compartments
	 */
	int GetNoOfCompartments ();


	/**
	 * @brief Set number of compartments to
	 *
	 * @param n Number of compartments
	 */
	void SetNoOfCompartments (int n);


	/**
     * @brief Pointer to the evolution saving function
     *
     * @param l NEEDS documenting
     * @param b NEEDS documenting
     */
    void      (*saveEvolFunPtr)(long l, bool b)  ;

    void*             solverSettings ;      /**< @brief Arbitrary solver settings  */
    bool              solverSuccess;	    /**< @brief true, if last calculation successful */

	void*             auxiliary;           /**< @brief Auxiliary data any kind of container needed for static methods can go here */

    long              SpinNumber;		    /**< @brief Number of the current spin*/
    long              TotalSpinNumber;      /**< @brief Total number of spins*/
    long              TotalADCNumber;       /**< @brief Total number of spins*/

    double*           Values;               /**< @brief Values of the current spin (position and physical parameters) */
    double            time;                 /**< @brief Current time point (in atom) */
    double            total_time;           /**< @brief Current time point (absolut time) */
    double            phase;                /**< @brief Receiver phase taken from the TPOIs*/
    double            PhaseLock;            /**< @brief Locked Phase (the phase set by the last RF pulse)*/
    double            deltaB;               /**< @brief Any off-resonance terms*/
    double*           solution;          /**< @brief Solution [M_r, phi, M_z] at the current time point*/

    double            RandNoise;            /**< @brief percentage of random noise added to the signal */
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
    int				  m_no_processes;		/**< @brief number of parallel processes; used by load balancing */

    long			  m_startSpin;			/**< @brief start calculation with this spin ( in case of restart)  */

	int               m_noofspinprops;      /**< @brief # of spin properties */
	int               m_noofspincompartments; /** Number of spin compartments (i.e. MT pools etc.)*/

	std::vector<double> helper;
	//double*           helper;             /** @brief Any additional data necessary for solving the model. (i.e. Global MT exchange matrix) */
	
 private:

    /**
     * @brief Default constructor
     */
    World  () {};

    static World*    m_instance;           /**< @brief Pointer to the sole instance of this implementation */

};

#endif /*WORLD_H_*/
