/*
 * Mpi2Evolution.h
 *
 *  Created on: Jan 13, 2010
 *      Author: dpflug
 */

#ifndef MPI2EVOLUTION_H_
#define MPI2EVOLUTION_H_
#include "mpi.h"
#include <vector>

using namespace std;



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

	static vector<MPI::File> 	m_files;
	static vector<bool>			m_first_write;

};


#endif /* MPI2EVOLUTION_H_ */
