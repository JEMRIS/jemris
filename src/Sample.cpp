/** @file Sample.cpp
 *  @brief Implementation of JEMRIS Sample
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

#include "Sample.h"
#include <math.h>


/**********************************************************/
Sample::Sample () {

    spins.size = 0;
    m_r2prime = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<8;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; }

}

/**********************************************************/
Sample::~Sample() { ClearSpins(); }

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
		spins.data[i].nn = 0.0;
    }

}

/**********************************************************/
Sample::Sample (long size) {

    m_r2prime = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<8;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; m_index[i] = 0.0; }

    CreateSpins(size);
}

/**********************************************************/
Sample::Sample (string fname) {

    InitRandGenerator();
    m_r2prime = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<8;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; }

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
	            //fin->read((char*)(&(spins.data[pos].nn)), sizeof(double));

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
               //fin->read((char*)(&(spins.data[pos].nn)), sizeof(double));

        }

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
         subSample->spins.data[u].nn = spins.data[i].nn;
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
         m_val[NN]  = spins.data[l].nn;

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


