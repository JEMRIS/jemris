/** @file Sample.cpp
 *  @brief Implementation of JEMRIS Sample
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

#include "Sample.h"
#include "World.h"
#include "SampleReorderStrategyInterface.h"
#include "SampleReorderShuffle.h"
#include "CoilArray.h"
#include "BinaryContext.h"

#include <math.h>


template <class T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& data) {
	for (size_t i = 0; i < data.size(); ++i)
		std::cout << data[i] << " " ;
	return os;
}

/**********************************************************/
void Sample::Prepare (const std::string& fname) {

	m_offset.resize(3);
	m_res.resize(3);

    m_r2prime              = 0.0;
    m_pos_rand_perc        = 1.0;
	
    for (int i=0;i<3;++i) { 
		m_res[i]           = 0.0; 
		m_offset[i]        = 0.0; 
	}
	
    m_reorder_strategy     = NULL;
	
    m_max_paket_size       = 10000;
    m_min_paket_size       = 10;
    m_next_spin_to_send    = 0;
    m_is_restart           = false;
    m_sent_interval        = 30;
	m_no_spins_done        = 0;
	m_total_cpu_time       = 0.0;

	// Standard sample has only one compartment
	m_no_spin_compartments = 1;

    InitRandGenerator();

	if (fname != "")
		Populate(fname);
	
}


/**********************************************************/
void Sample::ClearSpins() {
	
	m_ensemble.Clear();
	
}


/**********************************************************/
void Sample::CreateSpins(const size_t l) {
	
	m_ensemble.Init (l);
	
}

/**********************************************************/
void Sample::CreateSpins(const size_t np, const size_t l) {

	m_ensemble.Init (np, l);

}

/**********************************************************/
Sample::Sample () {

	Prepare();

}


/**********************************************************/
Sample::~Sample() {

	m_ensemble.Clear();

	if (m_reorder_strategy != NULL)
		delete m_reorder_strategy;

}


/**********************************************************/
void Sample::CreateHelper (const size_t l) {

	if (l > 0)
		m_helper.resize(l);

}


/**********************************************************/
Sample::Sample (const size_t size) {

	Prepare();

}


/**********************************************************/
Sample::Sample (const string& fname, const int multiple) {

	Prepare (fname);
	CropEnumerate();
	MultiplySample(multiple);

}

/**********************************************************/
IO::Status Sample::Populate (const string& fname) {

	m_index.resize(3);

	// Binary interface
	BinaryContext bc (fname, IO::IN);
	if (bc.Status() != IO::OK)
		return bc.Status();

	NDData<double> data;

	std::vector<size_t> dims;
	std::vector<double> tmpdat;
	bool          grid = false;

	// ----------------------------------------------------
	// Physical parameters of spins

	// Retrieve data from file
	bc.Read(data, "data", "/sample");
	if (bc.Status() != IO::OK)
		return bc.Status();

	size_t tmpndim = data.NDim();

	dims          = data.DimVec();
	tmpdat        = data.DVec();

	size_t size   = data.Size();
	size_t nprops = data.Dims(0);
	size = size / nprops;

	for (int i = tmpndim; i < 4; i++)
		dims.push_back(1);

	memcpy (&m_index[0], &dims[1], 3*sizeof(size_t));

	// ----------------------------------------------------

	bc.Read (data, "resolution", "/sample");
	grid = (bc.Status() == IO::OK);
	m_res = data.DVec();


	bc.Read (data, "offset", "/sample");
	m_offset = data.DVec();

	// ----------------------------------------------------

	if (grid) {

		m_ensemble.Init (dims, size);
		
		int  nprop = dims[0] + 4;
		size_t n     = 0;
		
		for (size_t nz = 0; nz < m_index[ZC]; nz++)
			for (size_t ny = 0; ny < m_index[YC]; ny++)
				for (size_t nx = 0; nx < m_index[XC]; nx++, n++) {
					
					size_t epos = n * nprop;
					size_t spos = n * dims[0];
					std::vector<double>::const_iterator sposi = tmpdat.begin() + spos;
					std::vector<double>::iterator eposi = m_ensemble.At(M0 + epos);

					if (tmpdat[spos] > 0) {
						
						// Copy values over
						std::copy (sposi, sposi + dims[0], eposi);

						// Interpolate spatial position
						m_ensemble[XC+epos] = (nx-0.5*(m_index[XC]-1))*m_res[XC]+m_offset[XC];
						m_ensemble[YC+epos] = (ny-0.5*(m_index[YC]-1))*m_res[YC]+m_offset[YC];
						m_ensemble[ZC+epos] = (nz-0.5*(m_index[ZC]-1))*m_res[ZC]+m_offset[ZC];
						
					}

				}
		
	} else {

		dims[0] -= 4;
		m_ensemble.Init (dims, size);
		
		memcpy (&m_ensemble[0], &tmpdat[0], m_ensemble.Size()*sizeof(double));


	}

	return bc.Status();
	
}

/**********************************************************/
void Sample::CropEnumerate () {
	
	int  nsize = 0;
	long osize = m_ensemble.NSpins();
 	int nprops = m_ensemble.NProps();

	double* tmp = (double*) malloc (osize * nprops * sizeof(double));
	memcpy (tmp, m_ensemble.Data(), osize * nprops * sizeof(double));

	for (int i = 0; i < osize; i++)
		if (m_ensemble[i * nprops + M0] > 0)
			nsize++;

	m_ensemble.ClearData();
	m_ensemble.Init(nsize);
	
	int n = 0;
	for (int i = 0; i < osize; i++) 
		if (tmp[i * nprops + M0] > 0) {

			long npos = n * nprops;

			memcpy (&m_ensemble[npos], &tmp[i * nprops] , nprops * sizeof(double));
			m_ensemble[npos + nprops - 1] = n;
			m_spin_state.push_back(0);
			n++;

		}

	free (tmp);
}

/**********************************************************/
void Sample::MultiplySample (int multiple) {
if (multiple>1){
	int  nsize = m_ensemble.NSpins();
 	int nprops = m_ensemble.NProps();

	double* tmp = (double*) malloc (nsize * nprops * sizeof(double));
	memcpy (tmp, m_ensemble.Data(), nsize * nprops * sizeof(double));

	m_ensemble.ClearData();
	m_ensemble.Init(nsize*multiple);
	
	for (int i=0;i<multiple;i++){
		memcpy (&m_ensemble[i*nprops*nsize], &tmp[0] , nprops *nsize* sizeof(double));
	}

	for (int i=0;i<nsize*multiple;i++){
		m_ensemble[i*nprops + nprops - 1] = i;
	}

	m_spin_state.clear();
	m_spin_state.resize(nsize*multiple,0);


	free (tmp);
}


}

/**********************************************************/
size_t  Sample::GetSize   ()     const  {
	return m_ensemble.NSpins();
}

/**********************************************************/
void Sample::GetValues (const size_t l, double* val) {

	//copy the properties of the l-th spin to m_val

	memcpy (val, &m_ensemble[l*m_ensemble.NProps()], m_ensemble.NProps() * sizeof (double));

	//add position randomness of spin position
	val[XC] += m_rng.normal()*m_res[XC]*m_pos_rand_perc/100.0;
	val[YC] += m_rng.normal()*m_res[YC]*m_pos_rand_perc/100.0;
	val[ZC] += m_rng.normal()*m_res[ZC]*m_pos_rand_perc/100.0;

}

/**********************************************************/
double  Sample::GetDeltaB (size_t pos) {

	//get off-resonance : convert m_val from [Hz] to [kHz] and add the Lorentzian random offset
	double r2prime = (
					  (World::instance()->Values[R2S] > World::instance()->Values[R2]) ?
					  (World::instance()->Values[R2S] - World::instance()->Values[R2]) : 0.0);

	return ( 0.001*World::instance()->Values[DB] + tan(PI*(m_rng.uniform()-.5))*r2prime );

}

/**********************************************************/
void  Sample::ReorderSample() {
	/*if (m_reorder_strategy != NULL) {
		m_reorder_strategy->Execute(&spins);
		}*/
}

/**********************************************************/
void Sample::SetReorderStrategy(string strat){
	if (m_reorder_strategy!=NULL) delete m_reorder_strategy;

	//if (strat=="shuffle") m_reorder_strategy = new SampleReorderShuffle();

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
			if (NoSpins > EndNoSpins) NoSpins = EndNoSpins;
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

/*********************************************************/
void Sample::GetHelper (double* target) {	
	memcpy (target, &m_helper[0], m_helper.size() * sizeof (double));
}

/*********************************************************/
double* Sample::GetHelper () {	
	return &m_helper[0];
}

/*********************************************************/
size_t Sample::GetHelperSize () {
	return m_helper.size();
}

/*********************************************************/
int Sample::GetNoSpinCompartments () {	
	return m_no_spin_compartments;
}

/*********************************************************/
void Sample::SetNoSpinCompartments (int n) {	
	m_no_spin_compartments = n;
}

/*********************************************************/
void Sample::CopyHelper (double* out) {
	
	if (GetHelperSize())
		memcpy (out, &m_helper[0], GetHelperSize() * sizeof(double));
	
}
