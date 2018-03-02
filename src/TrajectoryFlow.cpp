/*
 * TrajectoryFlow.cpp
 *
 *  Created on: feb 26, 2014
 *      Author: afortin, in collaboration with edurand (adapted from dpflug's TrajectoryMotion class)
 */


/*
 *  JEMRIS Copyright (C)
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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


#include "TrajectoryFlow.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"
//MODIF
#include "World.h"
#include "BinaryContext.h"
//MODIF***

#define CERR {cout<<"Error in flow trajectory file: "<< filename <<"; exit!"<<endl;exit(-1);}
#define PX cout<<"x="<<x<<endl;
//MODIF
#include <iostream>
#include <fstream>
#include <string>
//#include <vector>
//MODIF***

TrajectoryFlow::TrajectoryFlow() {
//MODIF
	m_currentSpinIndex=0;
	m_LastHuntIndexActivation=0;
	m_trajLoopDuration=0;
	m_trajLoopNumber=0;
//MODIF***

}

TrajectoryFlow::~TrajectoryFlow() {
}


/***********************************************************/
void TrajectoryFlow::LoadFile(string filename) {

//MODIF
    //Check whether .h5 extension is present
    std::size_t ext_pos = filename.rfind(".");
    std::size_t h5_pos = filename.rfind(".h5");

    //Load file (HDF5 or ASCII)
    if((h5_pos != string::npos) && (h5_pos == ext_pos))     LoadTrajectoriesHDF5(filename);
    else                                                    LoadTrajectoriesASCII(filename);

    //Get number of trajectories loaded
    long siz_full=m_time_full.size();
    m_TotalTrajNumber=siz_full;
    World *pW(World::instance());
    cout<<pW->m_myRank<<" number of trajectories loaded : "<<siz_full<<endl;

    //Optional: display the trajectories loaded
 /*   long siz,siz_act;
    long traj,trajLine;
    for(traj=0;traj<siz_full;traj++) {
        siz = m_time_full[traj].size() ;
        siz_act = m_activation_full[traj].first.size() ;
        cout<<"Spin "<<traj<<" trajectory"<<endl;
        for(trajLine=0;trajLine<siz;trajLine++) {
            double zz = m_time_full[traj][trajLine] ;
            cout<<zz<<" ";
            double aa = m_trans_x_full[traj][trajLine] ;
            cout<<aa<<" ";
            double bb = m_trans_y_full[traj][trajLine] ;
            cout<<bb<<" ";
            double cc = m_trans_z_full[traj][trajLine] ;
            cout<<cc<<" "<<endl;
        }
        cout<<"Spin "<<traj<<" activation"<<endl;
        for(trajLine=0;trajLine<siz_act;trajLine++) {
            double dd = m_activation_full[traj].first[trajLine];
            cout<<dd<<" ";
            double ee = m_activation_full[traj].second[trajLine];
            cout<<ee<<endl;
        }
        cout<<endl;
        }*/
//MODIF***
}



/***********************************************************/
void TrajectoryFlow::LoadTrajectoriesHDF5(string filename) {


    BinaryContext bc (filename, IO::IN);
    NDData<double> data;
    std::vector<double> trajectories;

    // Retrieve data from file
    bc.Read(data, "trajectories", "/flow");
    if (bc.Status() != IO::OK)
        {   cout << "Unable to open flow trajectories file: " << filename<< "; exit!"<<endl;exit(-1);    }

    trajectories = data.Data();

    int kk;
    //cout<<"HDF5"<<endl;
    //for(kk=0;kk<trajectories.size();kk=kk+4)    cout<<trajectories[kk]<<" "<<trajectories[kk+1]<<" "<<trajectories[kk+2]<<" "<<trajectories[kk+3]<<endl;
    cout<<"HDF5"<<endl;
    //LoadTrajectoriesASCII("./Flow.dat");


    double buf;
    double t_mem;
    long traj,trajLine,dataLine;
    long Nspins;
    long NdataLines=trajectories.size();
    bool spinChange;
	SequenceTree* pSeqTree = SequenceTree::instance();
	double seqDuration = pSeqTree->GetRootConcatSequence()->GetDuration();

    World *pW(World::instance());
    if(pW->m_myRank<=0)     Nspins=pW->TotalSpinNumber;
    else                    Nspins=pW->getTrajNumber();
    //if(pW->m_myRank<=0)     { trajBegin=0; Nspins = pW->TotalSpinNumber;   }//World::instance()->TotalSpinNumber;
    //else                    { trajBegin= pW->getTrajBegin(); Nspins=trajBegin + pW->getTrajNumber(); }
    cout<<pW->m_myRank<<" CALLS "<<Nspins<<" TRAJECTORIES"<<endl;
    //cout<<"pW->getTrajNumber() "<<pW->getTrajNumber()<<endl;

    //Clear previous trajectories loaded
    m_time_full.clear();
    m_trans_x_full.clear();
    m_trans_y_full.clear();
    m_trans_z_full.clear();
    m_activation_full.clear();

    dataLine=0;

    for(traj=0;traj<Nspins;traj++)
    {
//MODIF***
    // read HDF5 file:
    // format:
    // time [ms]  translation x/y/z [mm]


    trajLine=0;
    t_mem=0;
    spinChange=true;
    cout<<"Loop begin."<<endl;
    while (dataLine<NdataLines) {

        buf=trajectories[dataLine];
        //cout<<"buf "<<buf<<" dataLine "<<dataLine<<" NdataLines "<<NdataLines<<endl;
        //Manage activation states
        if(buf==-111)  {
            dataLine++;
            if(dataLine<NdataLines) {
                buf=trajectories[dataLine];
                if(buf==-111 || buf==-222) continue;
                m_activation_time.push_back(t_mem);
                m_activation_state.push_back(true);
                //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl; }
                }
            else    {
                m_activation_time.push_back(t_mem);
                m_activation_state.push_back(true);
                //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl;
                break;
                }
            }
        if(buf==-222)  {
            dataLine++;
            if(dataLine<NdataLines) {
                buf=trajectories[dataLine];
                if(buf==-111 || buf==-222) continue;
                m_activation_time.push_back(t_mem);
                m_activation_state.push_back(false);
                //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl; }
                }
            else    {
                m_activation_time.push_back(t_mem);
                m_activation_state.push_back(false);
                //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl;
                break;
                }
            }


        //Check spin change (don't change again if multiple successive separators)
        if (buf==-999999)   {
            dataLine++;
            if(!spinChange)   break;
            else              continue;
            }
        spinChange=false;


        //Manage trajectory coordinates
        if (buf>=t_mem)   m_time.push_back(buf);
        else { cout<<"Trajectory error: decreasing time ("<<t_mem<<" and "<<buf<<")."<<endl;CERR; }
        t_mem=buf;

        dataLine++;
        if (dataLine<NdataLines)    {   buf=trajectories[dataLine]; m_trans_x.push_back(buf);    }
        else { CERR; }

        dataLine++;
        if (dataLine<NdataLines)    {   buf=trajectories[dataLine]; m_trans_y.push_back(buf);    }
        else CERR;

        dataLine++;
        if (dataLine<NdataLines)    {   buf=trajectories[dataLine]; m_trans_z.push_back(buf);    }
        else { CERR; }

        //cout<<"Spin "<<traj<<"   "<<m_time.back()<<" "<<m_trans_x.back()<<" "<<m_trans_y.back()<<" "<<m_trans_z.back()<<endl;
        trajLine++;
        dataLine++;
    }
    //No more trajectory to read before the last spin was reached out
    if(trajLine==0)   {
        //No trajectory loop
        if(m_trajLoopDuration==0 && m_trajLoopNumber==0)   {
            cout<<"Flow file error. Insufficient number of trajectories in the input file."<<endl;
            CERR;
            }
        //Trajectory loop
        else   {
            if(m_trajLoopDuration==0)   { cout<<"Flow loop error. The loop duration was not specified."<<endl; CERR; }
            if(m_trajLoopNumber==0)     { cout<<"Flow loop error. The number of trajectories in the loop was not specified."<<endl; CERR; }
            if(m_trajLoopNumber>traj)     { cout<<"Flow file error. Insufficient number of trajectories for the loop in the input file."<<endl; CERR; };
            cout<<pW->m_myRank<<": flow loop activated on the "<<m_trajLoopNumber<<" last trajectories. Loop duration : "<<m_trajLoopDuration<<" ms"<<endl;
            break;
            }
    }

    //cout<<"Writing traj."<<endl;
    // check input:
    // input starts at t=0?
    if (m_time[0] > 0) {
        //cout<<m_time[0]<<endl;
    	m_time.insert(m_time.begin(),0.0);
    	m_trans_x.insert(m_trans_x.begin(),m_trans_x[0]);
    	m_trans_y.insert(m_trans_y.begin(),m_trans_y[0]);
    	m_trans_z.insert(m_trans_z.begin(),m_trans_z[0]);
    	m_activation_time.insert(m_activation_time.begin(),m_activation_time[0]);
    	m_activation_state.insert(m_activation_state.begin(),m_activation_state[0]);
    }
    if (m_activation_time[0] > 0) {
        //cout<<m_activation_time[0]<<endl;
    	m_activation_time.insert(m_activation_time.begin(),0.0);
    	m_activation_state.insert(m_activation_state.begin(),m_activation_state[0]);
    }

    // data up to the end of trajectory?
    if (m_time.back() < seqDuration) {
    	m_time.push_back(seqDuration + 1);
    	m_trans_x.push_back(m_trans_x.back());
    	m_trans_y.push_back(m_trans_y.back());
    	m_trans_z.push_back(m_trans_z.back());
    }
    if (m_activation_time.back() < seqDuration) {
    	m_activation_time.push_back(seqDuration + 1);
        m_activation_state.push_back(m_activation_state.back());
    }


    //add this trajectory to the full flow container
    m_time_full.push_back(m_time);
    m_trans_x_full.push_back(m_trans_x);
    m_trans_y_full.push_back(m_trans_y);
    m_trans_z_full.push_back(m_trans_z);
    m_activation_full.push_back(make_pair(m_activation_time,m_activation_state));
    //cout<<traj+1<<" trajectories loaded"<<endl;


    //clear current containers
    //if(traj+1<Nspins) {
    m_time.clear();
    m_trans_x.clear();
    m_trans_y.clear();
    m_trans_z.clear();
    m_activation_time.clear();
    m_activation_state.clear();
    //}

    //cout<<"Writing traj. End."<<endl;
    }
//MODIF***
}


/***********************************************************/
void TrajectoryFlow::LoadTrajectoriesASCII(string filename)     {

    ifstream inFile;
    inFile.open(filename.c_str(), ifstream::in);
    if (!inFile.is_open()) {
        cout << "Unable to open flow trajectories file: " << filename<< "; exit!"<<endl;
        exit(-1); // terminate with error
    }


    double buf;
    double t_mem;
    long traj,trajLine;
    long Nspins;
    bool spinChange;
	SequenceTree* pSeqTree = SequenceTree::instance();
	double seqDuration = pSeqTree->GetRootConcatSequence()->GetDuration();

    World *pW(World::instance());
    if(pW->m_myRank<=0)     Nspins=pW->TotalSpinNumber;
    else                    Nspins=pW->getTrajNumber();
    //if(pW->m_myRank<=0)     { trajBegin=0; Nspins = pW->TotalSpinNumber;   }//World::instance()->TotalSpinNumber;
    //else                    { trajBegin= pW->getTrajBegin(); Nspins=trajBegin + pW->getTrajNumber(); }
    cout<<pW->m_myRank<<" CALLS "<<Nspins<<" TRAJECTORIES"<<endl;
    //cout<<"pW->getTrajNumber() "<<pW->getTrajNumber()<<endl;

    //Clear previous trajectories loaded
    m_time_full.clear();
    m_trans_x_full.clear();
    m_trans_y_full.clear();
    m_trans_z_full.clear();
    m_activation_full.clear();

    for(traj=0;traj<Nspins;traj++)
    {
//MODIF***
    // read ASCII file:
    // format:
    // time [ms]  translation x/y/z [mm]


    trajLine=0;
    t_mem=0;
    spinChange=true;
    while ((inFile >> buf)) {
        //Manage activation states
        if(buf==-111)  {
            m_activation_time.push_back(t_mem);
            m_activation_state.push_back(true);
            //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl;
            if ((inFile >> buf)) {
                if(buf==-111 || buf==-222) CERR; }
            else break;
            }
        if(buf==-222)  {
            m_activation_time.push_back(t_mem);
            m_activation_state.push_back(false);
            //cout<<"         "<<m_activation_time.back()<<" "<<m_activation_state.back()<<endl;
            if ((inFile >> buf)) {
                if(buf==-111 || buf==-222) CERR; }
            else break;
            }

        //Check spin change (don't change again if multiple successive separators)
        if (buf==-999999)   {
            if(!spinChange)   break;
            else              continue;
            }
        spinChange=false;


        //Manage trajectory coordinates
        if (buf>=t_mem)   m_time.push_back(buf);
        else { cout<<"Trajectory error: decreasing time ("<<t_mem<<" and "<<buf<<")."<<endl;CERR; }
        t_mem=buf;

        if ((inFile >> buf))      m_trans_x.push_back(buf);
        else { CERR; }

        if (inFile >> buf)        m_trans_y.push_back(buf);
        else CERR;

        if (inFile >> buf)        m_trans_z.push_back(buf);
        else { CERR; }

        //cout<<"Spin "<<traj<<"   "<<m_time.back()<<" "<<m_trans_x.back()<<" "<<m_trans_y.back()<<" "<<m_trans_z.back()<<endl;
        trajLine++;
    }
    //No more trajectory to read, but the last spin was not reached out
    if(trajLine==0)   {
        //No trajectory loop
        if(m_trajLoopDuration==0 && m_trajLoopNumber==0)   {
            cout<<"Flow file error. Insufficient number of trajectories in the input file."<<endl;
            CERR;
            }
        //Trajectory loop
        else   {
            if(m_trajLoopDuration==0)   { cout<<"Flow loop error. The loop duration was not specified."<<endl; CERR; }
            if(m_trajLoopNumber==0)     { cout<<"Flow loop error. The number of trajectories in the loop was not specified."<<endl; CERR; }
            if(m_trajLoopNumber>traj)     { cout<<"Flow file error. Insufficient number of trajectories for the loop in the input file."<<endl; CERR; };
            cout<<pW->m_myRank<<": flow loop activated on the last "<<m_trajLoopNumber<<" trajectories. Loop duration : "<<m_trajLoopDuration<<" ms"<<endl;
            break;
            }
    }

    // check input:
    // input starts at t=0?
    if (m_time[0] > 0) {
        //cout<<m_time[0]<<endl;
    	m_time.insert(m_time.begin(),0.0);
    	m_trans_x.insert(m_trans_x.begin(),m_trans_x[0]);
    	m_trans_y.insert(m_trans_y.begin(),m_trans_y[0]);
    	m_trans_z.insert(m_trans_z.begin(),m_trans_z[0]);
    	m_activation_time.insert(m_activation_time.begin(),m_activation_time[0]);
    	m_activation_state.insert(m_activation_state.begin(),m_activation_state[0]);
    }
    if (m_activation_time[0] > 0) {
        //cout<<m_activation_time[0]<<endl;
    	m_activation_time.insert(m_activation_time.begin(),0.0);
    	m_activation_state.insert(m_activation_state.begin(),m_activation_state[0]);
    }

    // data up to the end of trajectory?
    if (m_time.back() < seqDuration) {
    	m_time.push_back(seqDuration + 1);
    	m_trans_x.push_back(m_trans_x.back());
    	m_trans_y.push_back(m_trans_y.back());
    	m_trans_z.push_back(m_trans_z.back());
    }
    if (m_activation_time.back() < seqDuration) {
    	m_activation_time.push_back(seqDuration + 1);
        m_activation_state.push_back(m_activation_state.back());
    }


    //add this trajectory to the full flow container
    m_time_full.push_back(m_time);
    m_trans_x_full.push_back(m_trans_x);
    m_trans_y_full.push_back(m_trans_y);
    m_trans_z_full.push_back(m_trans_z);
    m_activation_full.push_back(make_pair(m_activation_time,m_activation_state));
    //cout<<traj+1<<" trajectories loaded"<<endl;


    //clear current containers
    //if(traj+1<Nspins) {
  /*  vector<double>().swap(m_time);
    vector<double>().swap(m_trans_x);
    vector<double>().swap(m_trans_y);
    vector<double>().swap(m_trans_z);
    vector<double>().swap(m_rot_x);
    vector<double>().swap(m_rot_y);
    vector<double>().swap(m_rot_z);
    */
    m_time.clear();
    m_trans_x.clear();
    m_trans_y.clear();
    m_trans_z.clear();
    m_activation_time.clear();
    m_activation_state.clear();
  //}

    }
    inFile.close();

//MODIF***
}


/***********************************************************/
void TrajectoryFlow::GetPosition(double time, double &trans_x, double &trans_y, double &trans_z, long traj_number) {


	int ilo = GetLowerIndex(time,m_time_full[traj_number],m_LastHuntIndex);
	double step = m_time_full[traj_number][ilo+1] - m_time_full[traj_number][ilo];
	double b = (time - m_time_full[traj_number][ilo])/step;
   /* if(time==0) {
        World *pW(World::instance());
        int m_currentSpinIndex=pW->SpinNumber+pW->getTrajBegin();
        cout<<pW->m_myRank<<" simulates spin "<<m_currentSpinIndex<<endl;
    }*/

	// linear interpolation:
	//MODIF
	trans_x = m_trans_x_full[traj_number][ilo] + ((m_trans_x_full[traj_number][ilo + 1] - m_trans_x_full[traj_number][ilo]) * b);
	trans_y = m_trans_y_full[traj_number][ilo] + ((m_trans_y_full[traj_number][ilo + 1] - m_trans_y_full[traj_number][ilo]) * b);
	trans_z = m_trans_z_full[traj_number][ilo] + ((m_trans_z_full[traj_number][ilo + 1] - m_trans_z_full[traj_number][ilo]) * b);
    //MODIF***

}


/***********************************************************/
void TrajectoryFlow::GetValueDerived(double time,double *values)   {

	double t_traj,trans_x, trans_y, trans_z;
	long traj;


    //Get trajectory number
    if(m_currentSpinIndex+1>m_TotalTrajNumber)
    {
        //Trajectory index different from spin number in periodic flow mode
        traj = (m_currentSpinIndex-m_TotalTrajNumber) % m_trajLoopNumber + m_TotalTrajNumber - m_trajLoopNumber;
        //Add a time shift to the trajectory for each new loop
        long loop=floor((m_currentSpinIndex-m_TotalTrajNumber) / m_trajLoopNumber) + 1;
        t_traj=time-loop*m_trajLoopDuration;
        if(t_traj<0)    t_traj=0;       //Spin stays at initial position until the loop starts
    }
    else
    {
        traj=m_currentSpinIndex;
        t_traj=time;
    }

    //Get new position
    GetPosition(t_traj, trans_x,trans_y,trans_z, traj);

	//Update new position
    	values[0] = trans_x;
	values[1] = trans_y;
	values[2] = trans_z;

    //Set spin activation
    int ilo = GetLowerIndex(t_traj,m_activation_full[traj].first,m_LastHuntIndexActivation);
    m_spinActive=m_activation_full[traj].second[ilo];

    //Log file
    if(World::instance()->logFile)
    {
        fstream log("FLOW.log",ios::out|ios::app);
        log<<"spin "<<m_currentSpinIndex<<" time "<<time<<" t_traj "<<t_traj<<" trans_x "<<trans_x<<" trans_y "<<trans_y<<" trans_z "<<trans_z<<endl;
        log.close();
    }

}



/***********************************************************/

int TrajectoryFlow::GetLowerIndex(double t, vector<double> timeArray, int LastHuntIndex) {
	int ihi;
	int ilo;

	// test bounds:
	if ((t<= timeArray[0]) || (t>=timeArray.back())) {
		if (t == timeArray[0]) {ilo = 0;  return ilo;};
		if (t == timeArray.back()) {ilo = timeArray.size()-2;  return ilo;};
		cout << "Interpolation out of bounds! exit.(t= "<<t<<"; timeArray[0]="<<timeArray[0]<<"; timeArray.back()="<<timeArray.back()<< endl;
		exit(-1);
	}

	// hunt phase:
	int iHuntStep = 1;
	int iend = timeArray.size()-1;

	if (timeArray[LastHuntIndex]<t) {
		// hunt up:
		ilo = LastHuntIndex;
		ihi = ilo + 1;
		while (timeArray[ihi] < t ) {
			ilo = ihi;
			//iHuntStep << 1;
			ihi = ilo + iHuntStep;
			if (ihi > iend) ihi = iend;
		}
	} else {
		// hunt down:
		ihi = LastHuntIndex;
		ilo = ihi - 1;
		while (timeArray[ilo] > t ) {
			ihi = ilo;
			//iHuntStep << 1;
			ilo = ihi - iHuntStep;
			if (ilo < 0 ) ilo = 0;
		}
	}
	// bisection phase:
	int middle;
	while ( (ihi-ilo) > 1) {
		middle=(ihi+ilo) >> 1;
		if (timeArray[middle] > t) ihi = middle; else ilo = middle;
	}

	LastHuntIndex = ilo;
	return ilo;
}
/****************************************************************/
