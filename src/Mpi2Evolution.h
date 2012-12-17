/** @file Mpi2Evolution.h
 * @brief Implementation of JEMRIS Mpi2Evolution
 *
 *  Created on: Jan 13, 2010
 *      Author: dpflug
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

#ifndef MPI2EVOLUTION_H_
#define MPI2EVOLUTION_H_
#include "mpi.h"
#include <vector>
#include "config.h"

using namespace std;

/**
 * @brief pure virtual class; provides methods to write evolution file using parallel I/O
 */

class Mpi2Evolution {
public:
	Mpi2Evolution();

	/**
	 * use virtual destructor to make the class pure virtual; its not meant to be instantiated
	 */
	virtual ~Mpi2Evolution()=0;

	/**
	 * open evolution file(s)
	 */
	static void OpenFiles(int is_restart);

	/**
	 * close evolution file(s)
	 */
	static void CloseFiles();

	/**
	 * write evolution file(s) using parallel I/O
	 */
	static void saveEvolution(long index, bool close_files);

	/**
	 * sets saveEvolution function
	 */
	static void SetSaveFunction();

#ifdef HAVE_MPI_THREADS
	static vector<MPI::File> 	m_files;
#endif
	static vector<bool>			m_first_write;

};


#endif /* MPI2EVOLUTION_H_ */
