/*
 * Trajectory.cpp
 *
 *  Created on: Feb 27, 2009
 *      Author: dpflug
 */

#include "Trajectory.h"


Trajectory::Trajectory() {
	m_LastHuntIndex=0;
}

Trajectory::~Trajectory() {
}
/****************************************************************/
void Trajectory::ReadFile(const string filename) {
	ifstream inFile;
    inFile.open(filename.c_str(), ifstream::in);
    if (!inFile.is_open()) {
        cout << "Unable to open Trajectory file: " << filename<< "; exit!"<<endl;
        exit (-1); // terminate with error
    }

    double t,x;
    bool firstValue = true;
    while (inFile >> t) {
    	if (!(inFile >> x)) {cout << "Error in Trajectory file: " << filename<< "; exit!"<<endl; exit(-1);}
    	if (t > 0) {
    		if (firstValue) {
    		    m_time.push_back(0.0);
    		    m_data.push_back(x);
    		    firstValue = false;
    		}
		    m_time.push_back(t);
		    m_data.push_back(x);
    	}
    } // end while

 /*   SequenceTree* pSeqTree = SequenceTree::instance();
   double duration = pSeqTree->GetRootConcatSequence()->GetDuration();
    if (m_time.back()< duration ) {
    	m_data.push_back(m_data.back());
    	m_time.push_back(duration + 1);
    }
*/
}
/****************************************************************/
double Trajectory::GetData(const double time) {

	if (time > m_time.back()) return m_data.back();

	int ilo = GetLowerIndex(time);
	double step = m_time[ilo+1] - m_time[ilo];
	double b = (time - m_time[ilo])/step;
	double data;

	// linear interpolation:
	data = m_data[ilo] + ((m_data[ilo + 1] - m_data[ilo]) * b);

	return data;
}


/****************************************************************/
int Trajectory::GetLowerIndex(const double t) {
	int ihi;
	int ilo;

	// test bounds:
	if ((t<= m_time[0]) || (t>=m_time.back())) {
		if (t == m_time[0]) {ilo = 0;  return ilo;};
		if (t == m_time.back()) {ilo = m_time.size()-2;  return ilo;};
		cout << "Interpolation out of bounds! exit."<< endl;
		exit(-1);
	}

	// hunt phase:
	int iHuntStep = 1;
	int iend = m_time.size()-1;

	if (m_time[m_LastHuntIndex]<t) {
		// hunt up:
		ilo = m_LastHuntIndex;
		ihi = ilo + 1;
		while (m_time[ihi] < t ) {
			ilo = ihi;
			iHuntStep << 1;
			ihi = ilo + iHuntStep;
			if (ihi > iend) ihi = iend;
		}
	} else {
		// hunt down:
		ihi = m_LastHuntIndex;
		ilo = ihi - 1;
		while (m_time[ilo] > t ) {
			ihi = ilo;
			iHuntStep << 1;
			ilo = ihi - iHuntStep;
			if (ilo < 0 ) ilo = 0;
		}
	}
	// bisection phase:
	int middle;
	while ( (ihi-ilo) > 1) {
		middle=(ihi+ilo) >> 1;
		if (m_time[middle] > t) ihi = middle; else ilo = middle;
	}

	m_LastHuntIndex = ilo;
	return ilo;
}









