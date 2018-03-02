/****************************************************************************
**  This is a part of ParticleTracer                                       **
**  Copyright (C) 2016  Simon Garnotel                                     **
**                                                                         **
**  This program is free software: you can redistribute it and/or modify   **
**  it under the terms of the GNU General Public License as published by   **
**  the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                    **
**                                                                         **
**  This program is distributed in the hope that it will be useful,        **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of         **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          **
**  GNU General Public License for more details.                           **
**                                                                         **
**  You should have received a copy of the GNU General Public License      **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.  **
**                                                                         **
** **************************************************************************
**                                                                         **
** Author: Simon Garnotel                                                  **
** Last update: Alexandre Fortin                                           **
** Contact: simon.garnotel@gmail.com, fortin.alexandre@yahoo.fr            **
** Date: 06/2017                                                           **
** Version: 2.0                                                            **
****************************************************************************/

#ifndef HEADER_HPP
#define HEADER_HPP

//C++ headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

//C headers
#include <cstdio>
#include <cstdlib>
#include <cmath>

//Define
//#define OS "LINUX"
#define OS "MACOS"

//Prototypes
double ***ReadFreeFemVTK(const string FileName,
			const unsigned int NLabels, const int *Labels,
			unsigned int *NPoints,
			bool *Res);

bool CreateProgrammableSource(const string FileName,
			double **Points, double **Velocities, const unsigned  int NPoints,
			const int NSourcePoint, const double DeltaT);

bool CreateProgrammableFilter(const string FileName, const int N, const double SphereCenter[3], const double DeltaT, const double tseedMax, const bool steady);

bool CreateProgrammableFilter_fill(const string FileName, const double SphereCenter[3]);

bool CreatePVD(const string DataFile,
			const double FinalT, const double DeltaT);

bool CreateParticleTracer(const string *ProgrammableSourceFile, const string *ProgrammableFilterFile, const unsigned int NLabels, const unsigned int ForceReinjection, const double SphereCenter[3], const double SphereRadius, const long int NFillingPoints);


#endif



