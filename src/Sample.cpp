/** @file Sample.cpp
 *  @brief Implementation of JEMRIS Sample
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

#include "Sample.h"
#include <math.h>
#include "World.h"
#include "SampleReorderStrategyInterface.h"
#include "SampleReorderShuffle.h"
#include "CoilArray.h"


/**********************************************************/
void Sample::Init(){
    spins.size = 0;
    m_r2prime = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<NO_SPIN_PROPERTIES;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; }

    m_reorder_strategy=NULL;

    m_max_paket_size=10000;
    m_min_paket_size=10;
    m_next_spin_to_send=0;
    m_is_restart=false;
    m_sent_interval=30;

}
/**********************************************************/
Sample::Sample () {
	Init();
}

/**********************************************************/
Sample::~Sample() {
	ClearSpins();
	if (m_reorder_strategy != NULL)
		delete m_reorder_strategy;
}

/**********************************************************/
void Sample::ClearSpins() {

 spins.size = 0;
 delete[] spins.data;

}

/**********************************************************/
void Sample::CreateSpins(long size) {
    spins.size = size;
    spins.data = new Spin_data[size];

    for (long i = 0; i < size; i++) {
		spins.data[i].x  = 0.0;
		spins.data[i].y  = 0.0;
		spins.data[i].z  = 0.0;
		spins.data[i].m0 = 0.0;
		spins.data[i].r1 = 0.0;
		spins.data[i].r2 = 0.0;
		spins.data[i].r2s= 0.0;
		spins.data[i].db = 0.0;
		spins.data[i].index = 0.0;
    }
}

/**********************************************************/
Sample::Sample (long size) {
	Init();

    CreateSpins(size);
}

/**********************************************************/
Sample::Sample (string fname) {
    Init();

    InitRandGenerator();

    // create new array of spins and new multiarray for spacial distribution
    ifstream fin (fname.c_str(), ios::binary);
    Populate(&fin);
    fin.close();

}

/**********************************************************/
void Sample::Populate(string fname) {

    if ( GetSize() > 0 ) ClearSpins();

    ifstream fin (fname.c_str(), ios::binary);
    Populate(&fin);
    fin.close();

}
/**********************************************************/
void Sample::Populate (ifstream* fin) {

    long pos = 0;

    double read = 0.0;

    for (short i = 0; i < 3; i++) {
        fin->read((char*)(&(read)),        sizeof(double));
        m_index[i] = (long) read;
        fin->read((char*)(&(m_res[i])),    sizeof(double));
        fin->read((char*)(&(m_offset[i])), sizeof(double));
    }

    //case 1 : spins stored on cartesian grid
    if (m_index[1] > 0) {

	CreateSpins(m_index[XC]*m_index[YC]*m_index[ZC]); //creates too much spins ... who cares

	 for (long z=0; z < m_index[ZC]; z++)
	    for (long y=0; y < m_index[YC]; y++)
	        for (long x=0; x < m_index[XC]; x++) {

	            fin->read((char*)(&(spins.data[pos].m0)), sizeof(double));
	            fin->read((char*)(&(spins.data[pos].r1)), sizeof(double));
	            fin->read((char*)(&(spins.data[pos].r2)), sizeof(double));
	            fin->read((char*)(&(spins.data[pos].r2s)),sizeof(double));
	            fin->read((char*)(&(spins.data[pos].db)), sizeof(double));

	            spins.data[pos].x = (x-0.5*(m_index[XC]-1))*m_res[XC]+m_offset[XC];
	            spins.data[pos].y = (y-0.5*(m_index[YC]-1))*m_res[YC]+m_offset[YC];
	            spins.data[pos].z = (z-0.5*(m_index[ZC]-1))*m_res[ZC]+m_offset[ZC];

	            if (spins.data[pos].m0 > 0.0) { pos++; }
	        }
		spins.size = pos;

    //case 2 : spins stored as tuple (POSx,POSy,POSz,Mo,R1,R2,DB,CS) in binary file
    } else {

	CreateSpins(m_index[0]); //first 'index-dimension' now is the number of spins!
        for (pos=0; pos < m_index[0]; pos++) {

               fin->read((char*)(&(spins.data[pos].x)) , sizeof(double));
               fin->read((char*)(&(spins.data[pos].y)) , sizeof(double));
               fin->read((char*)(&(spins.data[pos].z)) , sizeof(double));
               fin->read((char*)(&(spins.data[pos].m0)), sizeof(double));
               fin->read((char*)(&(spins.data[pos].r1)), sizeof(double));
               fin->read((char*)(&(spins.data[pos].r2)), sizeof(double));
               fin->read((char*)(&(spins.data[pos].r2s)),sizeof(double));
               fin->read((char*)(&(spins.data[pos].db)), sizeof(double));

        }

    }
    for (int i=0; i<spins.size; i++){
    	spins.data[i].index=(double) i;
    	m_spin_state.push_back(0);
    }


}

/**********************************************************/
Sample* Sample::GetSubSample (int n, long size){

   long N    = GetSize();

   if (n >size || size>N) return NULL;

   long l    = N%(size);
   long k    = N/(size)+(n>l?0:1);
   long ibeg = (n-1) * k+ (n>l?l:0);
   long iend =  n    * k - 1 + (n>l?l:0);

   Sample* subSample = new Sample(iend-ibeg+1);

   for (long i=ibeg ,u=0;i<=iend;i++,u++) {
         subSample->spins.data[u].x  = spins.data[i].x;
         subSample->spins.data[u].y  = spins.data[i].y;
         subSample->spins.data[u].z  = spins.data[i].z;
         subSample->spins.data[u].m0 = spins.data[i].m0;
         subSample->spins.data[u].r1 = spins.data[i].r1;
         subSample->spins.data[u].r2 = spins.data[i].r2;
         subSample->spins.data[u].r2s= spins.data[i].r2s;
         subSample->spins.data[u].db = spins.data[i].db;
         subSample->spins.data[u].index = spins.data[i].index;

   }

   return (subSample);

}

/**********************************************************/
long    Sample::GetSize   ()       {
	return spins.size;
}

/**********************************************************/
double* Sample::GetValues (long l) {

	//copy the properties of the l-th spin to m_val
         m_val[XC]  = spins.data[l].x;
         m_val[YC]  = spins.data[l].y;
         m_val[ZC]  = spins.data[l].z;
         m_val[M0]  = spins.data[l].m0;
         m_val[R1]  = spins.data[l].r1;
         m_val[R2]  = spins.data[l].r2;
         m_val[R2S] = spins.data[l].r2s;
         m_val[DB]  = spins.data[l].db;
         m_val[ID]  = spins.data[l].index;

	//add position randomness of spin position
	m_val[XC] += m_rng.normal()*m_res[XC]*m_pos_rand_perc/100.0;
	m_val[YC] += m_rng.normal()*m_res[YC]*m_pos_rand_perc/100.0;
	m_val[ZC] += m_rng.normal()*m_res[ZC]*m_pos_rand_perc/100.0;

	return m_val;
}

/**********************************************************/
double  Sample::GetDeltaB (long pos) {

	//copy the properties of the l-th spin to m_val (if p<0 use the last copy)
	if (pos>=0) GetValues(pos);

	//get off-resonance : convert m_val from [Hz] to [kHz] and add the Lorentzian random offset
	double r2prime = ((m_val[R2S]>m_val[R2])?(m_val[R2S]-m_val[R2]):0.0);
	return ( 0.001*m_val[DB] + tan(PI*(m_rng.uniform()-.5))*r2prime );
}

/**********************************************************/
void  Sample::ReorderSample() {
	if (m_reorder_strategy != NULL) {
		m_reorder_strategy->Execute(&spins);
	}
}
/**********************************************************/
void Sample::SetReorderStrategy(string strat){
	if (m_reorder_strategy!=NULL) delete m_reorder_strategy;

	if (strat=="shuffle") m_reorder_strategy = new SampleReorderShuffle();

}
/**********************************************************/
void  Sample::GetScatterVectors(int *sendcount, int *displs, int size) {
	World* pw = World::instance();
	m_spins_sent.resize(size);
	m_last_offset_sent.resize(size);

	timeval dummy;
	gettimeofday(&dummy,NULL);

	if (!m_is_restart) {
		if (!(pw->m_useLoadBalancing)) {
				// send all at once:
				int count = (int) (( (double) GetSize() ) / ((double) (size - 1) ) + 0.01);
				int rest  = (int) (fmod((double) GetSize() , (double) (size - 1) ) + 0.01);

				// no spin data for master:
				displs[0]	= 0;
				sendcount[0]= 0;

				for (int i=1;i<size;i++) {
					sendcount[i] 	= count;
					if (rest > 0) {
						sendcount[i]++;
						rest--;
					}
					displs[i]=displs[i-1]+sendcount[i-1];
					// bookkeeping
					m_spins_sent[i]=sendcount[i];
					m_last_offset_sent[i]=displs[i];
					if (sendcount[i]>0)
						ReportSpin(displs[i],displs[i]+sendcount[i]-1,1);
				}
				m_next_spin_to_send = GetSize();
				m_last_time.resize(size,dummy);

				return;
			}
		}
	// now get scatter in case of restart/loadbalancing:
	displs[0]	= 0;
	sendcount[0]= 0;

	int spins_left = pw->TotalSpinNumber;
	if (m_is_restart) spins_left = SpinsLeft();

	int maxNoSpins = spins_left / size / 2;
	if (maxNoSpins<m_min_paket_size) maxNoSpins=m_min_paket_size;
	if (maxNoSpins>m_max_paket_size) maxNoSpins=m_max_paket_size;

	for (int i=1;i<size;i++) {
		int noToSent = i*m_min_paket_size;
		if (noToSent > maxNoSpins) {
			noToSent=(int) m_rng.uniform(m_min_paket_size,maxNoSpins);
		}
		displs[i]=displs[i-1]+sendcount[i-1];
		if ((noToSent + displs[i])>GetSize()) noToSent = GetSize()-displs[i];
		if (displs[i]==GetSize()) noToSent = 0;
		if ((m_is_restart) && (noToSent!=0)) {
			while(m_spin_state[displs[i]] == 2) {
				if (displs[i]==GetSize()) {
					noToSent = 0;
					break;
				}
				displs[i]++;
			}
			int count;
			for (count=1; count<=noToSent;count++){
				if (m_spin_state[displs[i]+count] != 0) {count++;break;}
				if ((displs[i]+count) ==GetSize()) {count++;break;}
			}
			count--;
			noToSent=count;
		}
		sendcount[i]=noToSent;
		// bookkeeping
		m_spins_sent[i]=sendcount[i];
		m_last_offset_sent[i]=displs[i];
		if (sendcount[i]>0)
			ReportSpin(displs[i],displs[i]+sendcount[i]-1,1);
	}
	m_last_time.resize(size,dummy);
	m_next_spin_to_send = displs[size-1] + sendcount[size-1];

	return;
}

/**********************************************************/
void Sample::GetNextPacket(int &NoSpins, int &NextSpinToSend, int ID) {
	// calc timings:
	timeval dummy;
	gettimeofday(&dummy,NULL);
	double time_used;
	time_used=((double) dummy.tv_sec - m_last_time[ID].tv_sec) + 0.000001*((double) dummy.tv_usec - (double) m_last_time[ID].tv_usec);
	m_total_cpu_time += time_used;
	m_last_time[ID] = dummy;
	m_no_spins_done += m_spins_sent[ID];

	//bookkeeping:
	if (m_spins_sent[ID] > 0)
		ReportSpin(m_last_offset_sent[ID],m_last_offset_sent[ID]+m_spins_sent[ID]-1,2);

	// spins left to send?
	if (m_next_spin_to_send < GetSize()) {
		while (m_spin_state[m_next_spin_to_send] != 0) {
			m_next_spin_to_send++;
			if (m_next_spin_to_send == GetSize())
				break;
		}
	}
	if (GetSize() == m_next_spin_to_send) {
		NoSpins = 0;
		NextSpinToSend=-1;
		return;
	} else {
		int spinsleft;
		if (!m_is_restart) {	spinsleft = GetSize() - m_next_spin_to_send;} else {spinsleft=SpinsLeft();}
		if (m_no_spins_done == 0) {NoSpins = m_min_paket_size;} else {
			World* pw=World::instance();
			NoSpins = int ( (m_no_spins_done / m_total_cpu_time) * m_sent_interval * (pw->m_no_processes - 1));
			// make sample size partly random to avoid syncronisation of slaves:
			NoSpins += (m_rng.uniform()-0.5)*0.1*NoSpins;
			//decrease NoSpins towards end of simulation:
			int EndNoSpins = floor(((double) spinsleft)/2.0/((double) m_last_time.size()-1));
			if(NoSpins > EndNoSpins) NoSpins = EndNoSpins;
		}
		if (NoSpins > m_max_paket_size) NoSpins = m_max_paket_size;
		if (NoSpins < m_min_paket_size)	NoSpins = m_min_paket_size;
		if (NoSpins > spinsleft) NoSpins = spinsleft;
		if (m_is_restart) {
			int count;
			for (count=1;count<=NoSpins;count++) {
				if (m_spin_state[m_next_spin_to_send+count]!=0) {
					count ++;
					break;
				}
			}
			count--;
			NoSpins = count;
		}
		NextSpinToSend=m_next_spin_to_send;
		m_next_spin_to_send += NoSpins;

		// bookkeeping:
		m_last_offset_sent[ID] = NextSpinToSend;
		m_spins_sent[ID] = NoSpins;
		if (NoSpins > 0)
			ReportSpin(m_last_offset_sent[ID],m_last_offset_sent[ID]+m_spins_sent[ID]-1,1);
	}
}

/**********************************************************/
void Sample::DumpRestartInfo(CoilArray* RxCA) {
	static time_t lasttime = time(NULL);

	// wait at least 10 seconds to dump restart info again. (-> in parallel jemris and syncron slaves prevent excessive disk writing...)
	if (abs(time(NULL) - lasttime) > 10) {
		lasttime = time(NULL);
		RxCA->DumpSignals(".tmp_sig", false);
		string fname(".spins_state.dat");
		ofstream fout(fname.c_str() , ios::binary);
		fout.write(&m_spin_state.at(0), sizeof(char)*m_spin_state.size());
		fout.close();
	}
}

/**********************************************************/
int Sample::ReadSpinsState() {

	ifstream spinsFile(".spins_state.dat", ifstream::binary);

	if (!spinsFile.is_open()) return (-2);
	// get length of file:
	spinsFile.seekg (0, ios::end);
	unsigned int length = spinsFile.tellg();
	if (length != m_spin_state.size()) {spinsFile.close();  return (-1);}
	spinsFile.seekg (0, ios::beg);
	spinsFile.read (&m_spin_state.at(0),length);
	spinsFile.close();


	World* pw = World::instance();
	int start=0;
	while (m_spin_state[start]==2) start++;
	for (unsigned int i=start; i<m_spin_state.size(); i++) {
		// clean up spin_state:
		if (m_spin_state[i]==1) m_spin_state[i]=0;
	}
	//serial jemris: cannot restart parallel jemris restart files.
	if (pw->m_myRank<0) {
		for (int i=start; i < GetSize(); i++) {
			if (m_spin_state[i] == 2) {
				ClearSpinsState();
				return -1;
			}
		}
	}
	pw->m_startSpin = start;
	m_is_restart=true;

	return (0);
}
/**********************************************************/
void Sample::ClearSpinsState() {
	World* pw = World::instance();
	pw->m_startSpin = 0;
	for (unsigned int i=0; i<m_spin_state.size();i++) m_spin_state[i]=0;
	m_is_restart = false;
}
/**********************************************************/
int Sample::SpinsLeft() {
	int count=0;
	for (unsigned int i=0; i<m_spin_state.size();i++ ){
		if (m_spin_state[i]==0) count++;
	}
	return count;
}
