/** @file Mpi2Evolution.h
 * @brief Implementation of JEMRIS Mpi2Evolution
 *
 *  Created on: Jan 13, 2010
 *      Author: dpflug
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
