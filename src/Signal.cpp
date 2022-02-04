/** @file Signal.cpp
 *  @brief Implementation of JEMRIS Signal
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

#include "Signal.h"
#include "World.h"
#include "BinaryContext.h"


/**********************************************************/
Signal::Signal   (long samples) {

	m_repository.Initialize (samples, 1);

}

/**********************************************************/
Signal::Signal   (long samples, int compartments) {
	
	//InitRandGenerator();
	m_repository.Initialize (samples, compartments);

}


/**********************************************************/
void Signal::ReadFrom (char* fname) {

	/*	ifstream fin (fname, ios::binary);

	for (long i = 0; i < m_repository.size; i++) {

		fin.read((char*)(&(m_repository[i                             ])), sizeof(double));
		fin.read((char*)(&(m_repository[i +     m_repository.Samples()])), sizeof(double));
		fin.read((char*)(&(m_repository[i + 2 * m_repository.Samples()])), sizeof(double));
		fin.read((char*)(&(m_repository[i + 3 * m_repository.Samples()])), sizeof(double));

	}

	fin.close();*/

}


