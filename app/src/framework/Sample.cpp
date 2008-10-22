/** @file Sample.cpp
 *  @brief Implementation of JEMRIS Sample
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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
    m_lff = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<8;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; }

}

/**********************************************************/
Sample::~Sample() { ClearSpins(); }

/**********************************************************/
void Sample::ClearSpins() {

 spins.size = 0;
 delete[] spins.x;
 delete[] spins.y;
 delete[] spins.z;
 delete[] spins.m0;
 delete[] spins.r1;
 delete[] spins.r2;
 delete[] spins.db;
 delete[] spins.nn;

}

/**********************************************************/
void Sample::CreateSpins(long size) {
    spins.size = size;
    spins.x   = new double[size];
    spins.y   = new double[size];
    spins.z   = new double[size];
    spins.m0  = new double[size];
    spins.r1  = new double[size];
    spins.r2  = new double[size];
    spins.db  = new double[size];
    spins.nn  = new double[size];

    for (long i = 0; i < size; i++) {
	spins.x[i] = 0.0;
	spins.y[i] = 0.0;
	spins.z[i] = 0.0;
	spins.m0[i]= 0.0;
	spins.r1[i]= 0.0;
	spins.r2[i]= 0.0;
	spins.db[i]= 0.0;
	spins.nn[i]= 0.0;
    }

}

/**********************************************************/
Sample::Sample (long size) {

    m_lff = 0.0;
    m_pos_rand_perc = 1.0;
    for (int i=0;i<8;++i) m_val[i] = 0.0;
    for (int i=0;i<3;++i) { m_res[i] = 0.0; m_offset[i] = 0.0; m_index[i] = 0.0; }

    CreateSpins(size);
}

/**********************************************************/
Sample::Sample (string fname) {

    InitRandGenerator();
    m_lff = 0.0;
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

	            fin->read((char*)(&(spins.m0[pos])), sizeof(double));
	            fin->read((char*)(&(spins.r1[pos])), sizeof(double));
	            fin->read((char*)(&(spins.r2[pos])), sizeof(double));
	            fin->read((char*)(&(spins.db[pos])), sizeof(double));
	            fin->read((char*)(&(spins.nn[pos])), sizeof(double));

	            spins.x[pos] = (x-0.5*(m_index[XC]-1))*m_res[XC]+m_offset[XC];
	            spins.y[pos] = (y-0.5*(m_index[YC]-1))*m_res[YC]+m_offset[YC];
	            spins.z[pos] = (z-0.5*(m_index[ZC]-1))*m_res[ZC]+m_offset[ZC];

	            if (spins.m0[pos] > 0.0) { pos++; }
	        }
		spins.size = pos;

    //case 2 : spins stored as tuple (POSx,POSy,POSz,Mo,R1,R2,DB,CS) in binary file
    } else {

	CreateSpins(m_index[0]); //first 'index-dimension' now is the number of spins!
        for (pos=0; pos < m_index[0]; pos++) {

               fin->read((char*)(&(spins.x[pos])) , sizeof(double));
               fin->read((char*)(&(spins.y[pos])) , sizeof(double));
               fin->read((char*)(&(spins.z[pos])) , sizeof(double));
               fin->read((char*)(&(spins.m0[pos])), sizeof(double));
               fin->read((char*)(&(spins.r1[pos])), sizeof(double));
               fin->read((char*)(&(spins.r2[pos])), sizeof(double));
               fin->read((char*)(&(spins.db[pos])), sizeof(double));
               fin->read((char*)(&(spins.nn[pos])), sizeof(double));

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
         subSample->spins.x[u]  = spins.x[i];
         subSample->spins.y[u]  = spins.y[i];
         subSample->spins.z[u]  = spins.z[i];
         subSample->spins.m0[u] = spins.m0[i];
         subSample->spins.r1[u] = spins.r1[i];
         subSample->spins.r2[u] = spins.r2[i];
         subSample->spins.db[u] = spins.db[i];
         subSample->spins.nn[u] = spins.nn[i];
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
         m_val[XC]  = spins.x[l];
         m_val[YC]  = spins.y[l];
         m_val[ZC]  = spins.z[l];
         m_val[M0]  = spins.m0[l];
         m_val[R1]  = spins.r1[l];
         m_val[R2]  = spins.r2[l];
         m_val[DB]  = spins.db[l];
         m_val[NN]  = spins.nn[l];

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

	//get off-resonance : convert [Hz] to [kHz] with factor 1/1000
	return ( 0.001 * ( m_val[DB] + m_lff*m_rng.normal() ) );
}


