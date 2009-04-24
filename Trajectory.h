/*
 * Trajectory.h
 *
 *  Created on: Feb 27, 2009
 *      Author: dpflug
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

//#include "SequenceTree.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>


using namespace std;

class Trajectory {
public:
	Trajectory();
	virtual ~Trajectory();

	virtual void ReadFile(const string filename);

	double GetData(const double time);


protected:
	/**
	 * @brief find data index for interpolation
	 * get lower index for given timepoint using hunt search algorithm.
	 */
	int GetLowerIndex(const double time);

	int m_LastHuntIndex;

	vector<double> m_time;
	vector<double> m_data;


};

#endif /* TRAJECTORY_H_ */
