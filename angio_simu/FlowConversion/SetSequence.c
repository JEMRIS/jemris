/****************************************************************************
**  Copyright (C) 2017 Alexandre Fortin                                    **
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
** Author: Alexandre Fortin                                                **
** Contact: fortin.alexandre@yahoo.fr                                      **
** Date: 06/2017                                                           **
** Version: 1.0                                                            **
****************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdlib.h>
using namespace std;

int main (int argc, char *argv[])
{

    //Read config file
    string ConfigFileName = "";
	if (argc != 2){
		cout << "*** Usage ***" << endl;
		cout << "./CreateSimuFiles ConfigMRI.in" << endl;
		cout << "Exit with error" << endl;
		return -1;
	}
	else{
		ConfigFileName = argv[1];
	}

	//READ SEQUENCE PARAMETERS
	//string XMLSeqFile = "";		//XML sequence file

	double FOV = 0.;			// mm (Field-of-View)
	double FOVthickness = 0.;   // mm
	double VoxelSize = 0.;		// mm
	double SliceThickness = 0.; // mm
	double InterSlice = 0.;     // mm
	double SeqDuration = 0.;    // mm (Sequence duration)
	string SliceOrientation = "";    // tra, sag, cor
	{
		ifstream File(ConfigFileName.c_str(), ios::in);
		if (File){
			string TMP;
			File >> TMP >> SeqDuration;
			File >> TMP >> FOV;
			File >> TMP >> FOVthickness;
			File >> TMP >> VoxelSize;
			File >> TMP >> SliceThickness;
			File >> TMP >> InterSlice;
			File >> TMP >> SliceOrientation;
			File.close();
		}
		else{
			cout << "ERROR: Unable to read " << ConfigFileName << endl;
			cout << "Exit with error" << endl;
			return -1;
		}
	}
	double FOVx,FOVy,FOVz;
	unsigned int Nx,Ny,Nz;
	unsigned int Nread,Nplane,Nsli;
	//Calculate matrix lines
	Nread=FOV/VoxelSize;
	if(Nread%2==1)     Nread++;
	Nplane=Nread;
	Nsli=(FOVthickness+InterSlice)/(SliceThickness+InterSlice);
	//if(Nsli%2==1)     Nsli++;
	//Calculate new FOV
    FOV=Nplane*VoxelSize;
	FOVthickness=Nsli*(SliceThickness+InterSlice);
	//Actualise values according to SliceOrientation
	if(SliceOrientation!="tra" && SliceOrientation!="sag" && SliceOrientation!="cor")   {
        cout << "ERROR: SliceOrientation error "<< endl;
        return -1;
    }
    if(SliceOrientation=="tra") {
        Nx=Nplane;FOVx=FOV;
        Ny=Nplane;FOVy=FOV;
        Nz=Nsli;FOVz=FOVthickness;
    }
    if(SliceOrientation=="cor") {
        Nx=Nplane;FOVx=FOV;
        Ny=Nsli;FOVy=FOVthickness;
        Nz=Nplane;FOVz=FOV;
    }
    if(SliceOrientation=="sag") {
        Nx=Nsli;FOVx=FOVthickness;
        Ny=Nplane;FOVy=FOV;
        Nz=Nplane;FOVz=FOV;
    }


    cout << "Set the sequence with parameters:"<<endl;
    cout << "FOVx=\"" << FOVx <<"\" FOVy=\""<< FOVy << "\" FOVz=\""<< FOVz <<"\""<<endl;
    cout << "Nx=\"" << Nx <<"\" Ny=\""<< Ny << "\" Nz=\""<< Nz <<"\" and interslice "<<InterSlice<<endl;


    return 0;
}
