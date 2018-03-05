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

#include "header.hpp"
#define PI 3.14159265358979323846

int main (int argc, char *argv[]){
	string ConfigFileName = "";
	if (argc != 2){
		cout << "*** Usage ***" << endl;
		cout << "./CreateParticleTracer ConfigFile.in" << endl;
		cout << "Exit with error" << endl;
		return -1;
	}
	else{
		ConfigFileName = argv[1];
	}

	//PARAMETERS
	string VTKFileName = "";		//VTK file
	string VTUFileName = "";		//VTU file of the same simulation

	double FinalT = 0.;			//Final time
	double DeltaT = 0.;			//Time step (precise value will be set by the script)
	double CycleT = 0.;         //Cycle duration if unsteday flow (seeding during one cycle)

	unsigned int NLabels = 0;		//Number of needed label
	int *Labels = NULL;

	unsigned int NSourcePoints = 0;		//Number of source points along the paraboloidal seeding
	unsigned int ForceReinjection = 0;  //Seeding every N steps (set to 0 in ConfigFile for steady flow and 1 for unsteady)

	double Vmax=0;                   // mm/s peak velocity at the seeding entries (if highly variable, treat each entry separately)

	double SphereCenter[3]={0,0,0};     //Coordinates of the center of the sample
	double SphereRadius=0;             //Radius of the full filling shere
	double NodesInterval=1;           //Mean spatial interval between neighboring nodes (defines the inter-particles distance)

	double	  sphereVolume=0;
	long int  NFillingPoints=0;         //Total points number in the filling sphere

	double DeltaT_max = 0.;

	{
		ifstream File(ConfigFileName.c_str(), ios::in);
		if (File){
			string TMP;
			File >> TMP >> VTKFileName;
			File >> TMP >> VTUFileName;
			File >> TMP >> FinalT;
			File >> TMP >> DeltaT_max;
			File >> TMP >> CycleT;
			File >> TMP >> SphereCenter[0] >> SphereCenter[1] >> SphereCenter[2];
			File >> TMP >> SphereRadius;
            //Get entries labels
			File >> TMP;
			getline(File,TMP);
            stringstream    LabelsLine(TMP);
            vector<int>  LabelsSto;
            int Label;
            while(LabelsLine >> Label){
                LabelsSto.push_back(Label);
            }
            NLabels=LabelsSto.size();
            cout<<NLabels<<" entries labels found"<<endl;
			if (NLabels == 0){
				cout << "ERROR: 0 entries labels" << endl;
				cout << "Exit with error" << endl;
				return -1;
			}
			else{
				Labels = new int[NLabels];
				for (unsigned int i = 0; i < NLabels; i++){
					Labels[i] = LabelsSto[i];
				}
			}
			File >> TMP >> Vmax;
			File >> TMP >> NodesInterval;
			File >> TMP >> ForceReinjection;
			File.close();
		}
		else{
			cout << "ERROR: Unable to read " << ConfigFileName << endl;
			cout << "Exit with error" << endl;
			return -1;
		}
	}

	//Calculate DeltaT and NSourcePoints
	for(unsigned int k=0; k*NodesInterval/Vmax < DeltaT_max; k++)   {
        NSourcePoints=k+1;
        DeltaT=k*NodesInterval/Vmax;
	}
	if(NSourcePoints<2)    { NSourcePoints=0; DeltaT=NodesInterval/Vmax; }
	cout<<"Time step: "<<DeltaT*1000<<" ms"<<endl;
    cout<<"Seeding paraboloid height: "<<NodesInterval*fabs(NSourcePoints-1.0)<<" mm"<<"     <-- WARNING ! Decrease DeltaT if > to vessels diameter"<<endl;
    //Store flow loop duration in a file (DeltaT if steady, CycleT if unsteady)
    fstream FlowLoop("./particles_loop.dat",ios::out|ios::trunc);
    if(ForceReinjection==0)     FlowLoop<<DeltaT*1000<<" ms"<<endl;
    else                        FlowLoop<<CycleT*1000<<" ms"<<endl;
    FlowLoop.close();
    //Calculate filling sphere properties
    sphereVolume=(4./3.)*PI*pow(SphereRadius,3);
    NFillingPoints=ceil(sphereVolume/pow(NodesInterval,3));


	//Read VTK File
	unsigned int *NPoints = new unsigned int[NLabels];
	bool Res;
	double ***PointsVelocities = ReadFreeFemVTK(VTKFileName,
						NLabels, Labels,
						NPoints, &Res);

	if (!Res){
		cout << "ERROR: ReadFreeFemVTK failed!" << endl;
		cout << "Exit with error" << endl;
		return -1;
	}

	//Create ProgrammableFileName, Points and Velocities ——MODIFIER POUR AJOUTER POINTS INTERMEDIAIRES——
	double ***Points = new double**[NLabels];
	double ***Velocities = new double**[NLabels];
	for (unsigned int i = 0; i < NLabels; i++){
		Points[i] = new double*[3];
		Velocities[i] = new double*[3];
		for (unsigned int j = 0; j < 3; j++){
			Points[i][j] = new double[NPoints[i]];
			Velocities[i][j] = new double[NPoints[i]];
			for (unsigned int k = 0; k < NPoints[i]; k++){
				Points[i][j][k] = PointsVelocities[i][j][k];
				Velocities[i][j][k] = PointsVelocities[i][j+3][k];
			}
		}
	}

	string *ProgrammableSourceFile = new string[NLabels+1];
	string *ProgrammableFilterFile = new string[NLabels+1];
	for (unsigned int i = 0; i <= NLabels; i++){
		ostringstream PSoss;
		PSoss << "ProgrammableSource_" << i << ".py";
		ProgrammableSourceFile[i] = PSoss.str();

		ostringstream PFoss;
		PFoss << "ProgrammableFilter_" << i << ".py";
		ProgrammableFilterFile[i] = PFoss.str();
	}

	//Create ProgrammableSource for entries seeding
	for (unsigned int i = 0; i < NLabels; i++){
		Res = CreateProgrammableSource(ProgrammableSourceFile[i],
						Points[i], Velocities[i], NPoints[i],
						NSourcePoints, DeltaT);
		if (!Res){
			cout << "ERROR: CreateProgrammableSource failed!" << endl;
			cout << "Label = " << Labels[i] << endl;
			cout << "Exit with error" << endl;
			return -1;
		}
	}

	//Create ProgrammableFilter for entries seeding
	for (unsigned int i = 0; i < NLabels; i++){
        if(ForceReinjection==0)
            Res = CreateProgrammableFilter(ProgrammableFilterFile[i], i, SphereCenter, DeltaT, CycleT, 1);
        else
            Res = CreateProgrammableFilter(ProgrammableFilterFile[i], i, SphereCenter, DeltaT, CycleT, 0);
		if (!Res){
			cout << "ERROR: CreateProgrammableFilter failed!" << endl;
			cout << "Label = " << Labels[i] << endl;
			cout << "Exit with error" << endl;
			return -1;
		}
	}

	//Create ProgrammableFilter for initial filling
    Res = CreateProgrammableFilter_fill(ProgrammableFilterFile[NLabels], SphereCenter);
    if (!Res){
        cout << "ERROR: CreateProgrammableFilter failed!" << endl;
        cout << "for intial filling" << endl;
        cout << "Exit with error" << endl;
        return -1;
        }


	//Create PVD file
	Res = CreatePVD(VTUFileName,
			FinalT, DeltaT);
	if (!Res){
		cout << "ERROR: CreatePVD failed!" << endl;
		cout << "Exit with error" << endl;
			return -1;
	}


	//Create ParticleTracerFile
	Res = CreateParticleTracer(ProgrammableSourceFile, ProgrammableFilterFile, NLabels, ForceReinjection, SphereCenter, SphereRadius, NFillingPoints);
	if (!Res){
		cout << "ERROR: CreateParticleTracer failed!" << endl;
		cout << "Exit with error" << endl;
			return -1;
	}

	//Delete
	for (unsigned int i = 0; i < NLabels; i++){
		for (unsigned int j = 0; j < 3; j++){
			delete[] Points[i][j];
			delete[] Velocities[i][j];
			delete[] PointsVelocities[i][j];
			delete[] PointsVelocities[i][j+3];
		}
		delete[] Points[i];
		delete[] Velocities[i];
		delete[] PointsVelocities[i];
	}
	delete[] Points;
	delete[] Velocities;
	delete[] PointsVelocities;

	delete[] NPoints;
	delete[] Labels;
	delete[] ProgrammableSourceFile;

	//Exit
	cout << "Exit success" << endl;
	return 0;
}




bool CreateParticleTracer(const string *ProgrammableSourceFile, const string *ProgrammableFilterFile, const unsigned int NLabels, const unsigned int ForceReinjection, const double SphereCenter[3], const double SphereRadius, const long int NFillingPoints){
	ofstream File("ParticleTracer.py", ios::out | ios::trunc);
	unsigned int i;
	if (File){
		//Get pwd
		string pwd;
		int Result = system("pwd > pwd.dat");
		if (Result != 0){
			cout << "ERROR: Unable to determine pwd" << endl;
			return false;
		}
		ifstream TMPFile("pwd.dat", ios::in);
		if (TMPFile){
			TMPFile >> pwd;
			TMPFile.close();
			Result = system("rm pwd.dat");
			if (Result != 0){
				cout << "ERROR: Unable to determine pwd" << endl;
			return false;
		}
		}
		else{
			cout << "ERROR: Unable to determine pwd" << endl;
			return false;
		}

		File << "# This file is automatically generated" << endl;
		File << "# DO NOT MODIFY" << endl;

		File << "import os" << endl;
		File << "from paraview.simple import *" << endl;
		File << "paraview.simple._DisableFirstRenderCameraReset()" << endl;

		File << "pvdfile = PVDReader(FileName='";
		File << pwd;
		File << "/PT.pvd')" << endl;
		File << "animationScene1 = GetAnimationScene()" << endl;
		File << "animationScene1.UpdateAnimationUsingDataTimeSteps()" << endl;
		File << "renderView1 = GetActiveViewOrCreate('RenderView')" << endl;
		File << "pvdfileDisplay = Show(pvdfile, renderView1)" << endl;
		File << "pvdfileDisplay.ColorArrayName = [None, '']" << endl;
		//Seeding of the entries with programmable points source
		for (i = 0; i < NLabels; i++){
			File << "programmableSource" << i << " = ProgrammableSource();" << endl;
			File << "filePS = open('";
			File << pwd;
			File << "/";
			File << ProgrammableSourceFile[i] << "', 'r')" << endl;
			File << "contentPS" << i << " = filePS.read()" << endl;
			File << "programmableSource" << i << ".Script = contentPS" << i << endl;
			File << "programmableSourceDisplay" << i << " = Show(programmableSource" << i << ", renderView1)" << endl;
			File << "programmableSourceDisplay" << i << ".ColorArrayName = [None, '']" << endl;
			File << "SetActiveSource(pvdfile)" << endl;
			File << "particleTracer" << i << " = ParticleTracer(Input=pvdfile," << endl;
			File << "	    SeedSource=programmableSource" << i << ")" << endl;
			File << "particleTracer" << i << ".SelectInputVectors = ['POINTS', 'Velocity']" << endl;
			File << "renderView1.ResetCamera()" << endl;
			File << "particleTracer" << i << ".StaticMesh = 1" << endl;
			File << "particleTracer" << i << ".ForceReinjectionEveryNSteps = " << ForceReinjection << endl;
			File << "particleTracer" << i << ".ComputeVorticity = 0" << endl;
			File << "particleTracer" << i << "Display = Show(particleTracer" << i << ", renderView1)" << endl;
			File << "particleTracer" << i << "Display.ColorArrayName = [None, '']" << endl;
			File << "SetActiveSource(pvdfile)" << endl;
			File << "SetActiveSource(particleTracer" << i << ")" << endl;
			File << "programmableFilter" << i << " = ProgrammableFilter(Input=particleTracer" << i << ")" << endl;
			File << "programmableFilter" << i << ".Script = ''" << endl;
			File << "programmableFilter" << i << ".RequestInformationScript = ''" << endl;
			File << "programmableFilter" << i << ".RequestUpdateExtentScript = ''" << endl;
			File << "programmableFilter" << i << ".PythonPath = ''" << endl;
			File << "filePF = open('";
			File << pwd;
			File << "/";
			File << ProgrammableFilterFile[i];
			File << "', 'r')" << endl;
			File << "contentPF = filePF.read()" << endl;
			File << "programmableFilter" << i << ".Script = contentPF" << endl;
			File << "programmableFilter" << i << ".RequestInformationScript = ''" << endl;
			File << "programmableFilter" << i << ".RequestUpdateExtentScript = ''" << endl;
			File << "programmableFilter" << i << ".PythonPath = ''" << endl;
			File << "programmableFilter" << i << "Display = Show(programmableFilter" << i << ", renderView1)" << endl;
			File << "programmableFilter" << i << "Display.ColorArrayName = [None, '']" << endl;
		}
		//Initial filling of the geometry with a sphere of points source
        File << "pointSource1 = PointSource()" << endl;
        File << "pointSource1.Center = ["<<SphereCenter[0]<<", "<<SphereCenter[1]<<", "<<SphereCenter[2]<<"]"<< endl;
        File << "pointSource1.NumberOfPoints = " <<NFillingPoints<< endl;
        File << "pointSource1.Radius = " <<SphereRadius<< endl;
        File << "pointSource1Display = Show(pointSource1, renderView1)"<< endl;
        File << "pointSource1Display.ColorArrayName = [None, '']" << endl;
        File << "SetActiveSource(pvdfile)" << endl;
        File << "particleTracer" << i << " = ParticleTracer(Input=pvdfile," << endl;
        File << "	    SeedSource=pointSource1)" << endl;
        File << "particleTracer" << i << ".SelectInputVectors = ['POINTS', 'Velocity']" << endl;
        File << "renderView1.ResetCamera()" << endl;
        File << "particleTracer" << i << ".StaticMesh = 1" << endl;
        File << "particleTracer" << i << ".ForceReinjectionEveryNSteps = 0" << endl;
        File << "particleTracer" << i << ".ComputeVorticity = 0" << endl;
        File << "particleTracer" << i << "Display = Show(particleTracer" << i << ", renderView1)" << endl;
        File << "particleTracer" << i << "Display.ColorArrayName = [None, '']" << endl;
        File << "Hide(pointSource1, renderView1)" << endl;
        File << "SetActiveSource(pvdfile)" << endl;
        File << "SetActiveSource(particleTracer" << i << ")" << endl;
        File << "programmableFilter" << i << " = ProgrammableFilter(Input=particleTracer" << i << ")" << endl;
        File << "programmableFilter" << i << ".Script = ''" << endl;
        File << "programmableFilter" << i << ".RequestInformationScript = ''" << endl;
        File << "programmableFilter" << i << ".RequestUpdateExtentScript = ''" << endl;
        File << "programmableFilter" << i << ".PythonPath = ''" << endl;
        File << "filePF = open('";
        File << pwd;
        File << "/";
        File << ProgrammableFilterFile[i];
        File << "', 'r')" << endl;
        File << "contentPF = filePF.read()" << endl;
        File << "programmableFilter" << i << ".Script = contentPF" << endl;
        File << "programmableFilter" << i << ".RequestInformationScript = ''" << endl;
        File << "programmableFilter" << i << ".RequestUpdateExtentScript = ''" << endl;
        File << "programmableFilter" << i << ".PythonPath = ''" << endl;
        File << "programmableFilter" << i << "Display = Show(programmableFilter" << i << ", renderView1)" << endl;
        File << "programmableFilter" << i << "Display.ColorArrayName = [None, '']" << endl;
        //Display options
		File << "pvdfileDisplay = Show(pvdfile, renderView1)" << endl;
		File << "pvdfileDisplay.SetRepresentationType('Wireframe')" << endl;
		File << "ColorBy(pvdfileDisplay, ('POINTS', 'Pressure'))" << endl;
		File << "pvdfileDisplay.RescaleTransferFunctionToDataRange(True)" << endl;
		File << "pvdfileDisplay.SetScalarBarVisibility(renderView1, True)" << endl;
		File << "pressureLUT = GetColorTransferFunction('Pressure')" << endl;
		File << "pressurePWF = GetOpacityTransferFunction('Pressure')" << endl;
		File << "animationScene1.Play()" << endl;
		File.close();
		return true;
	}
	else{
		cout << "ERROR: Unable to write ParticleTracer.py" << endl;
		return false;
	}
}



