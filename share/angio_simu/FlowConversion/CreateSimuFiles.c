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
#include <algorithm>
using namespace std;

#define DisplMinFactor 0.1   //Displacement not written if < VoxelSize*DisplMinFactor (mm)

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
	double DISPLmin = min(VoxelSize,SliceThickness)*DisplMinFactor;  //Displacement not written if < DISPLmin (mm)



    int N=0,m=0,l=0,statSeed=0,sep=0;
    double t,tShift=0,t_test,tmem=-999999,grandissT=1,tmaxWritten=0,t_write;
    float Dx=0,Dy=0,Dz=0,grandissX=1,grandissY=1,grandissZ=1;
    float x,y,z,xmem,ymem,zmem,xextrSup=-Dx,yextrSup=-Dy,zextrSup=-Dz,xextrInf=-Dx,yextrInf=-Dy,zextrInf=-Dz;
    streampos pos;

    fstream spins("./particles.dat",ios::in);
    fstream flow("./Flow.dat",ios::out|ios::trunc);
    while(spins>>t)
    {
        if(t==-999999)     { if(tmem==t)  //Si deux -999999 de suite, on n'ecrit rien, sinon on ecrit le -999999
                                        {   m++;
                                            continue; }
                            N++;
                            if(N%10000==0)   cout<<"N: "<<N<<endl;
                            if(t_write!=-111 && t_write!=-222)		flow<<-222<<endl; //TEMP desactivation finale
                            flow<<-999999<<endl<<endl;
                            }
        else                {
                            spins>>x>>y>>z;
                            //Decalage temporel
                            t=t-tShift;
                            if(t<tmem && t!=-999999)
                                {   cout<<"Warning: decreasing time "<<tmem<<" "<<t<<", adding separator "<<endl;t_write=-999999;flow<<t_write<<endl<<endl;tmem=-999999;N++;sep++;  }
                            if(t<0 && t!=-999999)
                                {   cout<<"Error: negative time "<<t*grandissT<<"   "<<(x+Dx)*grandissX<<" "<<(y+Dy)*grandissY<<" "<<grandissZ*(z+Dz)<<endl; continue; }
                            //Si le spin ne commence pas a t0
                            //on ajoute une ligne a t0 identique a la premiere position connue (spin desactive)
                            if(tmem==-999999 && t!=0) {
								//Pas d'ecriture si une seule ligne de trajectoire
								pos = spins.tellg();
								spins>>t_test;
								if(t_test==-999999)	{
									statSeed++;
									continue;  }
								spins.seekg(pos);
								//Ecriture sinon
								t_write=0;flow<<t_write<<"   "<<(x+Dx)*grandissX<<" "<<(y+Dy)*grandissY<<" "<<grandissZ*(z+Dz)<<endl;
								t_write=-222;flow<<t_write<<endl;
								t_write=t*grandissT;flow<<t_write<<"   "<<(x+Dx)*grandissX<<" "<<(y+Dy)*grandissY<<" "<<grandissZ*(z+Dz)<<endl;
								t_write=-111;flow<<t_write<<endl;
								xmem=x;ymem=y;zmem=z;
							}
                            if(t==0)    { xmem=x;ymem=y;zmem=z; }
                            //Copie de la position du spin si deplacement non negligeable ou si t=t0
                            if(t==0 || sqrt(pow(x-xmem,2)+pow(y-ymem,2)+pow(z-zmem,2))>DISPLmin)    {
                                t_write=t*grandissT;flow<<t_write<<"   "<<(x+Dx)*grandissX<<" "<<(y+Dy)*grandissY<<" "<<grandissZ*(z+Dz)<<endl;
                                if(t_write>tmaxWritten)     tmaxWritten=t_write;

                            	//Activation initiale
                            	if(t==0)    {
                                        //t_write=-222;flow<<t_write<<endl;
                                        //t_write=0.000001;flow<<t_write<<"   "<<(x+Dx)*grandissX<<" "<<(y+Dy)*grandissY<<" "<<grandissZ*(z+Dz)<<endl;
                                        t_write=-111;flow<<t_write<<endl;
                                }
                                //Enregistre positions extremes partout
                                if(x>xextrSup)     xextrSup=x;
                                if(y>yextrSup)     yextrSup=y;
                                if(z>zextrSup)     zextrSup=z;
                                if(x<xextrInf)     xextrInf=x;
                                if(y<yextrInf)     yextrInf=y;
                                if(z<zextrInf)     zextrInf=z;
                                //Enregistre positions extremes pour slice proche du centre
                              /*if(fabs(x+Dx)>fabs(xextr+Dx) && fabs(z+Dz)<=fabs(zextr+Dz)/10)     xextr=x;
                                if(fabs(y+Dy)>fabs(yextr+Dy) && fabs(z+Dz)<=fabs(zextr+Dz)/10)     yextr=y;
                                if(fabs(z+Dz)>fabs(zextr+Dz))     zextr=z;*/
                                xmem=x;ymem=y;zmem=z;
                            }
                            else    l++;
                            //cout<<t<<"   "<<z+Dz<<" "<<y<<" "<<x<<endl;
                            }
        //if(t==0)            flow<<-111<<endl;
        tmem=t;
    }
    flow.close();
    spins.close();
    cout<<"Adding "<<sep<<" missing separators"<<endl;
    cout<<l<<" lines aborted inferior to min displacement (<"<<DISPLmin<<" mm)"<<endl;
    cout<<statSeed<<" non moving spins aborted"<<endl;
    cout<<"t max written (ms): "<<tmaxWritten<<endl;
    cout<<"FOV extrema positions (mm): X:   "<<(xextrSup+Dx)*grandissX<<" Y:   "<<(yextrSup+Dy)*grandissY<<" Z:   "<<grandissZ*(zextrSup+Dz)<<endl;
    cout<<"                            X:  "<<(xextrInf+Dx)*grandissX<<" Y:  "<<(yextrInf+Dy)*grandissY<<" Z:  "<<grandissZ*(zextrInf+Dz)<<endl;
    cout<<"N trajectories="<<N<<"   N empty trajectories="<<m<<endl;



    //Get particles trajectories information for the loop
    double FlowLoopDuration,FlowLoopNumber;
    string TMP;
    fstream FlowLoop("./particles_loop.dat",ios::in);
    if(FlowLoop.is_open())  {
        FlowLoop>>FlowLoopDuration;
        cout<<"FlowLoopDuration (ms): "<<FlowLoopDuration<<endl;
    }
    else {
        cout<<"FlowLoopDuration (ms): ";
        cin>>FlowLoopDuration;
    }
    FlowLoop.close();
    fstream info("./particles_info.dat",ios::in);
    if(info.is_open())  {
        info>>TMP>>FlowLoopNumber;
        FlowLoopNumber=FlowLoopNumber-statSeed;
        cout<<"FlowLoopNumber (loop last N trajectories): "<<FlowLoopNumber<<endl;
    }
    else {
        cout<<"FlowLoopNumber (loop last N trajectories): ";
        cin>>FlowLoopNumber;
    }
    info.close();
    if(! (isfinite(FlowLoopDuration) && isfinite(FlowLoopNumber))) {
        cout << "ERROR: FlowLoopDuration or FlowLoopNumber has non finite value" << endl;
        cout << "Exit with error" << endl;
        return -1;
    }



	//Write .xml sequence file
    //fstream seq("./MySequence.xml",ios::out|ios::trunc);
    //seq.close();



    //Write .xml simulation file
    fstream simu("./simu_flow.xml",ios::out|ios::trunc);
    simu<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>"<<endl;
    simu<<"<simulate name=\"JEMRIS\">"<<endl;
    simu<<"<sample name=\"Vessels\" FlowTrajectories=\"Flow.dat\" ";
    if(FlowLoopDuration!=0 && FlowLoopNumber!=0)    simu<<" FlowLoopDuration=\""<<FlowLoopDuration<<"\" FlowLoopNumber=\""<<FlowLoopNumber<<"\" ";
    simu<<" uri=\"./sample.h5\"/>"<<endl;
    simu<<"<RXcoilarray uri=\"./uniform.xml\"/>"<<endl;
    simu<<"<TXcoilarray uri=\"./uniform.xml\"/>"<<endl;
    simu<<"<parameter ConcomitantFields=\"0\" EvolutionPrefix=\"evol\" EvolutionSteps=\"0\" RandomNoise=\"0\"/>"<<endl;
    simu<<"<sequence name=\"MySequence\" uri=\"./sequences/MySequence.xml\"/>"<<endl;
    simu<<"<model name=\"Bloch\" type=\"CVODE\"/>"<<endl;
    simu<<"</simulate>"<<endl;
    simu.close();

    cout.precision(0);
    cout << endl << fixed;
    cout << "-------- Usage --------" << endl;
    cout << "Generate a sample of " << floor(N + FlowLoopNumber*floor(SeqDuration/FlowLoopDuration))<<" spins with BuildMySample.m"<<endl;
    cout << "Set the acquisition sequence path in simu_flow.xml"<<endl;
    cout << "Launch with 'jemris simu_flow.xml' (use 'pjemris' for parallel)" << endl;


    return 0;
}
