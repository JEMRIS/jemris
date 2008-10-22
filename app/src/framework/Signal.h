/** @file Signal.h
 *  @brief Implementation of JEMRIS Signal
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <string>
using namespace std;

/**
 *  @brief  Signal repository structure
 */
struct Repository {

    long    size;   /**< Data size                     */
    double* tp;     /**< Time points                   */
    double* mx;     /**< Magnetisations in X direction */
    double* my;     /**< Magnetisations in Y direction */
    double* mz;     /**< Magnetisations in Z direction */

};

/**
 *  @brief The signal store and IO
 */
class Signal {


 public:

	/**
	 * Default constructor
	 */
	Signal               () {};

	/**
	 * Destructor
	 */
	virtual ~Signal      ();

	/**
	 * Instatiate a signal object of given size
	 *
	 * @param size Number of signal definition points
	 */
    Signal              (long size);

	/**
	 * Dump binary data to file
	 *
	 * @param fname File name
	 */
	void    DumpTo      (string fname);

	/**
	 * Read in binary data from file
	 *
	 * @param fname File name
	 */
	void    ReadFrom    (char* fname);


	/**
	 * Get the size of the signal.
	 *
	 * @return Size of this signa.
	 */
	long    GetSize(){return repository.size;};

	Repository repository; /**< @brief Signal repository */

};

#endif /*SIGNAL_H_*/
