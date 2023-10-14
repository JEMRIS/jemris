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
** Contact: simon.garnotel@gmail.com                                       **
** Date: 06/2016                                                           **
** Version: 1.0                                                            **
****************************************************************************/

#include "header.hpp"

double ***ReadFreeFemVTK(const string pFileName, const unsigned int pNLabels, const int *pLabels, unsigned int *pNPoints, bool *pRes){
	unsigned int NPoints = 0;
	double *PointsX = NULL, *PointsY = NULL, *PointsZ = NULL;
	unsigned int NCells = 0;
	unsigned int *CellsType = NULL, *Cells1 = NULL, *Cells2 = NULL, *Cells3 = NULL, *Cells4 = NULL;
	unsigned int *Labels = NULL;
	double *VelocityX = NULL, *VelocityY = NULL, *VelocityZ = NULL;

	ifstream File(pFileName.c_str(), ios::in);
	if (File){
		string TMP;

		//POINTS
		File >> TMP;
		while (TMP.compare("POINTS") != 0){
			File >> TMP;
		}
		File >> NPoints;
		//cout << "NPoints = " << NPoints << endl;

		if (NPoints == 0){
			(*pRes) = false;
			cout << "ERROR: NPoints == 0" << endl;
			return NULL;
		}

		PointsX = new double[NPoints];
		PointsY = new double[NPoints];
		PointsZ = new double[NPoints];

		File >> TMP;	//float

		for (unsigned int i = 0; i < NPoints; i++){
			File >> PointsX[i] >> PointsY[i] >> PointsZ[i];
		}

		//CELLS
		File >> TMP;	//CELLS
		File >> NCells;
		//cout << "NCells = " << NCells << endl;

		if (NCells == 0){
			(*pRes) = false;
			cout << "ERROR: NCells == 0" << endl;
			return NULL;
		}

		CellsType = new unsigned int[NCells];
		Cells1 = new unsigned int[NCells];
		Cells2 = new unsigned int[NCells];
		Cells3 = new unsigned int[NCells];
		Cells4 = new unsigned int[NCells];

		File >> TMP;

		for (unsigned int i = 0; i < NCells; i++){
			File >> CellsType[i];
			if (CellsType[i] == 4){
				File >> Cells1[i] >> Cells2[i] >> Cells3[i] >> Cells4[i];
			}
			else if (CellsType[i] == 3){
				File >> Cells1[i] >> Cells2[i] >> Cells3[i];
				Cells4[i] = 0.;
			}
		}

		//LABELS
		File >> TMP;
		while (TMP.compare("FreeFempp_table") != 0){
			File >> TMP;
		}

		Labels = new unsigned int[NCells];
		for (unsigned int i = 0; i < NCells; i++){
			File >> Labels[i];
		}

		//VELOCITY
		File >> TMP;
		while (TMP.compare("Velocity") != 0){
			File >> TMP;
		}
		File >> TMP;
		File >> TMP;
		File >> TMP;

		VelocityX = new double[NPoints];
		VelocityY = new double[NPoints];
		VelocityZ = new double[NPoints];
		for (unsigned int i = 0; i < NPoints; i++){
			File >> VelocityX[i] >> VelocityY[i] >> VelocityZ[i];
		}

		//Assign labels to points
		int *NewLabels = new int[NPoints];
		for (unsigned int i = 0; i < NPoints; i++){
			NewLabels[i] = -1;
		}
		for (unsigned int i = 0; i < NCells; i++){
			if (NewLabels[Cells1[i]] == -1){
				NewLabels[Cells1[i]] = Labels[i];
			}
			else{
				bool test = false;
				for (unsigned int j = 0; j < pNLabels; j++){
					if ((NewLabels[Cells1[i]] == pLabels[j]) || (Labels[i] != 0)){	//!= OR ==
						test = true;
					}
				}
				if (test) NewLabels[Cells1[i]] = Labels[i];
			}

			if (NewLabels[Cells2[i]] == -1){
				NewLabels[Cells2[i]] = Labels[i];
			}
			else{
				bool test = false;
				for (unsigned int j = 0; j < pNLabels; j++){
					if ((NewLabels[Cells2[i]] == pLabels[j]) || (Labels[i] != 0)){	//!= OR ==
						test = true;
					}
				}
				if (test) NewLabels[Cells2[i]] = Labels[i];
			}

			if (NewLabels[Cells3[i]] == -1){
				NewLabels[Cells3[i]] = Labels[i];
			}
			else{
				bool test = false;
				for (unsigned int j = 0; j < pNLabels; j++){
					if ((NewLabels[Cells3[i]] == pLabels[j]) || (Labels[i] != 0)){	//!= OR ==
						test = true;
					}
				}
				if (test) NewLabels[Cells3[i]] = Labels[i];
			}
		}

		//CHECK NEEDED LABELS
		for (unsigned int i = 0; i < pNLabels; i++){
			pNPoints[i] = 0;
			for (unsigned int j = 0; j < NPoints; j++){
				if (NewLabels[j] == pLabels[i]){
					pNPoints[i]++;
				}
			}
			//cout << "NPoints " << i << " = " << pNPoints[i] << endl;
		}

		double ***PointsVelocities = new double**[pNLabels];
		for (unsigned int i = 0; i < pNLabels; i++){
			PointsVelocities[i] = new double*[6];
			for (unsigned int j = 0; j < 6; j++){
				PointsVelocities[i][j] = new double[pNPoints[i]];
			}
		}

		for (unsigned int i = 0; i < pNLabels; i++){
			unsigned int k = 0;
			for (unsigned int j = 0; j < NPoints; j++){
				if (NewLabels[j] == pLabels[i]){
					PointsVelocities[i][0][k] = PointsX[j];
					PointsVelocities[i][1][k] = PointsY[j];
					PointsVelocities[i][2][k] = PointsZ[j];
					PointsVelocities[i][3][k] = VelocityX[j];
					PointsVelocities[i][4][k] = VelocityY[j];
					PointsVelocities[i][5][k] = VelocityZ[j];
					k++;
				}
			}
		}

		File.close();
		(*pRes) = true;

		//Delete
		delete[] PointsX;
		delete[] PointsY;
		delete[] PointsZ;
		delete[] CellsType;
		delete[] Cells1;
		delete[] Cells2;
		delete[] Cells3;
		delete[] Cells4;
		delete[] Labels;
		delete[] VelocityX;
		delete[] VelocityY;
		delete[] VelocityZ;
		delete[] NewLabels;

		//Return
		return PointsVelocities;
	}
	else{
		(*pRes) = false;
		cout << "ERROR: Unable to read " << pFileName << endl;
		return NULL;
	}
}



