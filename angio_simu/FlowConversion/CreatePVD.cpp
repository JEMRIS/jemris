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

bool CreatePVD(const string DataFile, const double FinalT, const double DeltaT){
	ofstream File("PT.pvd", ios::out | ios::trunc);
	if (File){
		int NbIter = FinalT/DeltaT + 1;
		
		File << "<VTKFile type=\"Collection\" version=\"0.1\">" << endl;
		File << "\t<Collection>" << endl;
		for (int i = 0; i < NbIter; i++){
			double CurrentTime = ((double)i) * DeltaT;
			if ((string)OS == "LINUX"){
				ostringstream oss;
				oss << CurrentTime;
				string CurrentTimeStr = oss.str();
				int Pos = CurrentTimeStr.find(".");
				if (Pos != -1){
					CurrentTimeStr.replace(Pos, 1, ",");
				}
				File << "\t\t<DataSet file=\"";
				File << DataFile;
				File << "\" group=\"\" part=\"0\" timestep=\"";
				File << CurrentTimeStr;
				File << "\" />" << endl;
			}
			else if ((string)OS == "MACOS"){
				File << "\t\t<DataSet file=\"";
				File << DataFile;
				File << "\" group=\"\" part=\"0\" timestep=\"";
				File << CurrentTime;
				File << "\" />" << endl;
			}
			else{
				cout << "Wnidows is not supported" << endl;
			}
		}
		File << "\t</Collection>" << endl;
		File << "</VTKFile>" << endl;
		
		File.close();
		return true;
	}
	else{
		cout << "ERROR: Unable to write PT.pvd" << endl;
		return false;
	}
}



