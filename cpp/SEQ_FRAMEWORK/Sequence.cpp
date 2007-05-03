/*
  This file is part of the MR simulation project
  Date: 03/2006
  Authors:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/


#include <iostream>
using namespace std;

#include "Sequence.h"

void Sequence::writeBinaryFile(string filename, double dT, string adcfile) {

	// open binary output for matlab diagrams
	ofstream fout(filename.c_str(), ios::binary);

	int      iNumber;

	//set of values for each point 
	double   dVal[5];

	double   dSampling;
	
	iNumber = (int) (getDuration()/dT);
	
	for (int i =0;i<iNumber;i++) {

		dSampling = i*dT;
		for (int j=0;j<5;++j) {dVal[j] = 0.0; };
		getValue(dSampling, dVal);	
		
		fout.write((char *)(&(dSampling)), sizeof(dSampling));
		fout.write((char *)(&(dVal[0])),   sizeof(dVal[0]));
		fout.write((char *)(&(dVal[1])),   sizeof(dVal[1]));
		fout.write((char *)(&(dVal[2])),   sizeof(dVal[2]));
		fout.write((char *)(&(dVal[3])),   sizeof(dVal[3]));
		fout.write((char *)(&(dVal[4])),   sizeof(dVal[4]));
		fout.write((char *)(&(dVal[5])),   sizeof(dVal[5]));
	}
	fout.close();
	
	//write ADC file
	ofstream fout2(adcfile.c_str());
	fout2 << "<?xml version=\"1.0\" ?>" << endl << endl;
	writeADCs(&fout2);
	fout2.close();

}

