/*
 * TrajectoryMotion.cpp
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#include "TrajectoryMotion.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"

#define CERR {cout<<"Error in motion trajectory file: "<< filename <<"; exit!"<<endl;exit(-1);}
#define PX cout<<"x="<<x<<endl;

TrajectoryMotion::TrajectoryMotion() {
	btx = true;
	bty = true;
	btz = true;
	brx = true;
	bry = true;
	brz = true;
	m_rot_origin_x = 0;
	m_rot_origin_y = 0;
	m_rot_origin_z = 0;

}

TrajectoryMotion::~TrajectoryMotion() {
}

/***********************************************************/
void TrajectoryMotion::LoadFile(string filename) {
	ifstream inFile;
    inFile.open(filename.c_str(), ifstream::in);
    if (!inFile.is_open()) {
        cout << "Unable to open motion trajectory file: " << filename<< "; exit!"<<endl;
        exit(-1); // terminate with error
    }

    // read ASCII file:
    // format:
    // first line: rotation origin x/y/z;
    // following lines:
    // time [ms]  translation x/y/z [mm] rotation around axis x/y/z [degree]
    // order of transformations: first rotate around x, then y, then z-axis; afterwards translate;
    // rotation around point m_rot_origin
    double x;
    if (!(inFile >> m_rot_origin_x)) CERR;
    if (!(inFile >> m_rot_origin_y)) CERR;
    if (!(inFile >> m_rot_origin_z)) CERR;

    while ((inFile >> x)) {
        m_time.push_back(x);
        if ((inFile >> x)) m_trans_x.push_back(x); else CERR;
        if (inFile >> x) m_trans_y.push_back(x); else CERR;
        if (inFile >> x) m_trans_z.push_back(x); else CERR;

        if (inFile >> x) m_rot_x.push_back(x/180*PI); else CERR;
        if (inFile >> x) m_rot_y.push_back(x/180*PI); else CERR;
        if (inFile >> x) m_rot_z.push_back(x/180*PI); else CERR;
    }
    inFile.close();

    // check input:
    // input starts at t=0?
    if (m_time[0] > 0) {
    	m_time.insert(m_time.begin(),0.0);
    	m_trans_x.insert(m_trans_x.begin(),m_trans_x[0]);
    	m_trans_y.insert(m_trans_y.begin(),m_trans_y[0]);
    	m_trans_z.insert(m_trans_z.begin(),m_trans_z[0]);

    	m_rot_x.insert(m_rot_x.begin(),m_rot_x[0]);
    	m_rot_y.insert(m_rot_y.begin(),m_rot_y[0]);
    	m_rot_z.insert(m_rot_z.begin(),m_rot_z[0]);
    }

    // data up to the end of trajectory?
	SequenceTree* pSeqTree = World::instance()->pSeqTree;
	double seqDuration = pSeqTree->GetRootConcatSequence()->GetDuration();
    if (m_time.back() < seqDuration) {
    	m_time.push_back(seqDuration + 1);
    	m_trans_x.push_back(m_trans_x.back());
    	m_trans_y.push_back(m_trans_y.back());
    	m_trans_z.push_back(m_trans_z.back());

    	m_rot_x.push_back(m_rot_x.back());
    	m_rot_y.push_back(m_rot_y.back());
    	m_rot_z.push_back(m_rot_z.back());
    }

    // test if motion is zero for a whole axis:
    btx = false;bty = false;btz = false;brx = false;bry = false;brz = false;
    for (unsigned int i=0; i < m_time.size(); i++) {
    	if (m_trans_x[i] != 0) btx = true;
    	if (m_trans_y[i] != 0) bty = true;
    	if (m_trans_z[i] != 0) btz = true;
    	if (m_rot_x[i]   != 0) brx = true;
    	if (m_rot_y[i]   != 0) bry = true;
    	if (m_rot_z[i]   != 0) brz = true;
    }

    // erase empty data:
    if (!btx) m_trans_x.clear();
    if (!bty) m_trans_y.clear();
    if (!btz) m_trans_z.clear();
    if (!brx) m_rot_x.clear();
    if (!bry) m_rot_y.clear();
    if (!brz) m_rot_z.clear();

}


/***********************************************************/
void TrajectoryMotion::GetPosition(double time, double &trans_x, double &trans_y, double &trans_z, double &rot_x, double &rot_y, double &rot_z) {

	int ilo = GetLowerIndex(time);
	double step = m_time[ilo+1] - m_time[ilo];
	double b = (time - m_time[ilo])/step;

	// linear interpolation:
	if (btx) {trans_x = m_trans_x[ilo] + ((m_trans_x[ilo + 1] - m_trans_x[ilo]) * b);} else {trans_x = 0;}
	if (bty) {trans_y = m_trans_y[ilo] + ((m_trans_y[ilo + 1] - m_trans_y[ilo]) * b);} else {trans_y = 0;}
	if (btz) {trans_z = m_trans_z[ilo] + ((m_trans_z[ilo + 1] - m_trans_z[ilo]) * b);} else {trans_z = 0;}

	if (brx) {rot_x = m_rot_x[ilo] + ((m_rot_x[ilo + 1] - m_rot_x[ilo]) * b);} else {rot_x = 0;}
	if (bry) {rot_y = m_rot_y[ilo] + ((m_rot_y[ilo + 1] - m_rot_y[ilo]) * b);} else {rot_y = 0;}
	if (brz) {rot_z = m_rot_z[ilo] + ((m_rot_z[ilo + 1] - m_rot_z[ilo]) * b);} else {rot_z = 0;}

}


/***********************************************************/
void TrajectoryMotion::GetValueDerived(double time,double *values){
    // order of transformations: first rotate around x, then y, then z-axis; afterwards translate;
    // rotation around point m_rot_origin

	double trans_x, trans_y, trans_z;
	double rot_x, rot_y, rot_z;
	double cosx, sinx;

	GetPosition(time, trans_x,trans_y,trans_z,rot_x,rot_y,rot_z);
	double old_x=values[0];
	double old_y=values[1];
	double old_z=values[2];
	double x,y,z;

	// transform to rotation point:
	x = old_x - m_rot_origin_x;
	y = old_y - m_rot_origin_y;
	z = old_z - m_rot_origin_z;

	// rotation around x-axis:
	if (rot_x != 0) {
		cosx = cos(rot_x);
		sinx = sin(rot_x);
		old_y = y;
		old_z = z;
		y = old_y * cosx - old_z * sinx;
		z = old_y * sinx + old_z * cosx;
	}

	// rotation around y-axis:
	if (rot_y != 0) {
		cosx = cos(rot_y);
		sinx = sin(rot_y);
		old_x = x;
		old_z = z;
		x = old_z * sinx + old_x * cosx;
		z = old_z * cosx - old_x * sinx;
	}

	// rotation around z-axis:
	if (rot_z != 0) {
		cosx = cos(rot_z);
		sinx = sin(rot_z);
		old_x = x;
		old_y = y;
		x = old_x * cosx - old_y * sinx;
		y = old_x * sinx + old_y * cosx;
	}

	// translation (from rotation origin + motion translation):
	values[0] = x + m_rot_origin_x + trans_x;
	values[1] = y + m_rot_origin_y + trans_y;
	values[2] = z + m_rot_origin_z + trans_z;

}

