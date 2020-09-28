/*
 * Trajectory1D.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
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

#include "Trajectory1D.h"
#include "SequenceTree.h"
#include "ConcatSequence.h"

Trajectory1D::Trajectory1D() {

}

Trajectory1D::~Trajectory1D() {
}

/***********************************************************/
void Trajectory1D::LoadFile(string filename) {
	ifstream inFile;
    inFile.open(filename.c_str(), ifstream::in);
    if (!inFile.is_open()) {
        cout << "Unable to open trajectory file: " << filename<< "; exit!"<<endl;
        exit(-1); // terminate with error
    }

    // read ASCII file:
    // format:
    // time  Value

    double x;
    while ((inFile >> x)) {
        m_time.push_back(x);
        if ((inFile >> x)) m_data.push_back(x); else {cout << "Error in Trajectory file: "<<filename<<". Exit." << endl; exit (-1);};
    }
    inFile.close();


    // check input:
    // input starts at t=0?
    if (m_time[0] > 0) {
    	m_time.insert(m_time.begin(),0.0);
    	m_data.insert(m_data.begin(),m_data[0]);
    }

    // data up to the end of trajectory?
    // if not: keep last data point until end.
	SequenceTree* pSeqTree = World::instance()->pSeqTree;
	double seqDuration = pSeqTree->GetRootConcatSequence()->GetDuration();
    if (m_time.back() < seqDuration) {
    	m_time.push_back(seqDuration + 1);
    	m_data.push_back(m_data.back());
    }
/*
    cout << filename;
    cout << " datapoints: time = " <<m_time.size()  << "; data: " << m_data.size()<<endl;
    for (int i=0; i<m_time.size();i++) {
    	cout <<  m_time[i] << " " << m_data[i] << endl;
    }
*/
    return;
}
/***********************************************************/
void Trajectory1D::LinearInterpolation(double time, double *value) {

	int ilo = GetLowerIndex(time);
	double step = m_time[ilo+1] - m_time[ilo];
	double b = (time - m_time[ilo])/step;
	// linear interpolation:
	value[0] = m_data[ilo] + ((m_data[ilo + 1] - m_data[ilo]) * b);
	return;
}

/***********************************************************/
void Trajectory1D::GetValueDerived(double time, double *value) {
	double data;
	LinearInterpolation(time,&data);
	value[0]*=data;
	return;
}

