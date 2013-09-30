/** @file Signal.h
 *  @brief Implementation of JEMRIS Signal
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

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include "rng.h"

#include <string>
#include <assert.h>
using namespace std;


/**
 * @brief Defined for robust memory handling
 */
static int sig_alloc = 0;


/**
 *  @brief  Signal repository structure
 */
struct Repository {

	long     m_noofsamples;      /**< Number of samples             */
	int      m_noofcompartments; /** < Number of compartments       */
	double*  m_times;
	double*  m_data;             /**< Data (column major)           */


	/**
	 * @brief Construct
	 */
	Repository () {

		m_data  = 0;
		m_times = 0;
		
		m_noofcompartments = 1; 
		m_noofsamples      = 0;
		
	};


	/**
	 * @brief Destruct
	 */
	~Repository () {

		if (sig_alloc) {
			free (m_data);
			free (m_times);
			sig_alloc --;
		}
	
	}


	/**
	 * @brief Size of bulk data (i.e. number of elemments of m_data)
	 *
	 * @return Size
	 */
	long Size() {

		return m_noofsamples * (long) NProps();

	};


	/**
	 * @brief Reference to data repository
	 *
	 * @return Refernce to data repository
	 */
	double* Data () {

		return m_data;

	};
	

	/**
	 * @brief Reference to time point repository
	 *
	 * @return Refernce to time point repository
	 */
	double* Times () {

		return m_times;

	};
	

	/**
	 * @brief Number of samples
	 *
	 * @return Number of samples
	 */
	inline const long Samples () const {
		
		return m_noofsamples;
		
	};


	/**
	 * @brief Number of compartments
	 *
	 * @return Number of compartments
	 */
	inline const int Compartments () const {
		
		return m_noofcompartments;
		
	};


	/**
	 * @brief Number of properties per compartment
	 *
	 * @return Number of properties per compartment
	 */
	inline const int NProps () const {
		
		return 3 * m_noofcompartments;
		
	};


	/**
	 * @brief Time of given sample index
	 *
	 * @return Time of given sample index
	 */
	inline const double TP (long l) const {
		
		return m_times[l];
		
	};


	/**
	 * @brief Reference to time of given sample index
	 *
	 * @return reference to the time point of given sample index
	 */
	inline double& TP (long l) {

		return m_times[l];

	};


	/**
	 * @brief Reference to data of given sample index 
	 */
	inline const long Position (long l) const {

		return l*NProps();

	};


	/**
	 * @brief Occupy RAM and set some dimensions
	 */
	inline const void Initialize (long samples, int compartments) {

		assert (samples      > 0);
		assert (compartments > 0);

		m_noofcompartments = compartments;
		m_noofsamples      = samples;

		m_data         = (double*) malloc (Size()    * sizeof (double));
		m_times        = (double*) malloc (Samples() * sizeof (double));
		sig_alloc ++;

		for (int i = 0; i < Size(); i++) 
			m_data[i]  = 0.0;

		for (int i = 0; i < Samples(); i++) 
			m_times[i] = 0.0;

	};


	/**
	 * @brief       Value at position in store
	 * @param  pos  Desired position 
	 * @return      Reference to pos-th value in the store
	 */
	inline double
	&operator[]     (long pos) {
		
		assert(pos >= 0);
		assert(pos <  Size());
		
		return m_data[pos];
		
	};
	
	/**
	 * @brief       Access to position in store
	 * @param  pos  Desired position 
	 * @return      Reference to pos-th value in the store
	 */
	inline const double
	operator[]     (long pos)  const {
		
		return m_data[pos];
		
	};
	
	/**
	 * @brief       Value at position in store
	 * @param  pos  Desired position 
	 * @return      Reference to pos-th value in the store
	 */
	inline double&     
	at (long pos) {
		
		assert(pos >= 0);
		assert(pos <  Size());
		
		return m_data[pos];
		
	};
	
	/**
	 * @brief       Access to position in store
	 * @param  pos  Desired position 
	 * @return      Reference to pos-th value in the store
	 */
	inline const double
	at (long pos)  const {
		
		return m_data[pos];
		
	};
	

};

/**
 *  @brief The signal store and IO
 */
class Signal {


 public:


	/**
	 * @brief Default constructor
	 */
	Signal               () {};


	/**
	 * @brief Default destructor
	 */
	~Signal              () {};


	/**
	 * Instantiate a signal object of given size
	 *
	 * @param size Number of signal definition points
	 */
    Signal              (long size);


	/**
	 * Instantiate a signal object of given size
	 *
	 * @param size Number of signal definition points
	 * @param compartments Number of signal compartments
	 */
    Signal              (long size, int compartments);

	/**
	 * Read in binary data from file
	 *
	 * @param fname File name
	 */
	void    ReadFrom    (char* fname);


	/**
	 * Get the number of samples of the data
	 *
	 * @return Number of the samples.
	 */
	const long    GetSize()  {
		return m_repository.Samples();
	};


	/**
	 * Access repository
	 *
	 * @return Number of the samples.
	 */
	Repository*    Repo ()  {
		return &m_repository;
	};


	/**
     * Initialize the random number generator
     *
     * @param val Any integer (default 1)
     */
    void    InitRandGenerator   (int val=1) { 
		m_rng = RNG( val * (long ) clock() ); 
	};

	/**
     * Initialize the random number generator
     *
     * @param val Any integer (default 1)
     */
    RNG*    Noise   () { 
		return &m_rng;
	};

private:
	
    RNG        m_rng;        /**< @brief random number generator */
	Repository m_repository; /**< @brief Signal repository       */

};

#endif /*SIGNAL_H_*/
