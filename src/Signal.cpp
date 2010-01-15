/** @file Signal.cpp
 *  @brief Implementation of JEMRIS Signal
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

#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

#include "Signal.h"
#include "World.h"



/**********************************************************/
Signal::~Signal  () {

 delete[] repository.tp;
 delete[] repository.mx;
 delete[] repository.my;
 delete[] repository.mz;

}

/**********************************************************/
Signal::Signal   (long size) {

	//InitRandGenerator();

	repository.size = size;
	repository.tp   = new double[size];
	repository.mx   = new double[size];
	repository.my   = new double[size];
	repository.mz   = new double[size];

	for (long i = 0; i < size; i++) {
		repository.tp[i] = 0.0;
		repository.mx[i] = 0.0;
		repository.my[i] = 0.0;
		repository.mz[i] = 0.0;
	}

}

/**********************************************************/
void Signal::ReadFrom (char* fname) {

	ifstream fin (fname, ios::binary);

	for (long i = 0; i < repository.size; i++) {

		fin.read((char*)(&(repository.tp[i])), sizeof(repository.tp[i]));
		fin.read((char*)(&(repository.mx[i])), sizeof(repository.mx[i]));
		fin.read((char*)(&(repository.my[i])), sizeof(repository.my[i]));
		fin.read((char*)(&(repository.mz[i])), sizeof(repository.mz[i]));

	}

	fin.close();

}

/**********************************************************/
void Signal::DumpTo   (string fname, bool normalize) {

	ofstream fout(fname.c_str() , ios::binary);
	World* pw = World::instance();

	for (long i = 0; i < repository.size; i++) {

		if (normalize) {
			repository.mx[i] /= pw->TotalSpinNumber;
			repository.my[i] /= pw->TotalSpinNumber;
			repository.mz[i] /= pw->TotalSpinNumber;

			//dwelltime-weighted random noise
			if (pw->RandNoise > 0.0)
			{
				double dt =  1;
				if (i>0)					dt = repository.tp[i]-repository.tp[i-1];
				else if (repository.size>1)	dt = repository.tp[i+1]-repository.tp[i];
				//definition: Gaussian has std-dev pw->RandNoise at a dwell-time of 0.01 ms
				repository.mx[i] += pw->RandNoise*m_rng.normal()*0.1/sqrt(dt);
				repository.my[i] += pw->RandNoise*m_rng.normal()*0.1/sqrt(dt);
			}
		}

		fout.write((char*)(&(repository.tp[i])), sizeof(repository.tp[i]));
		fout.write((char*)(&(repository.mx[i])), sizeof(repository.mx[i]));
		fout.write((char*)(&(repository.my[i])), sizeof(repository.my[i]));
		fout.write((char*)(&(repository.mz[i])), sizeof(repository.mz[i]));

	}

	fout.close();

}
