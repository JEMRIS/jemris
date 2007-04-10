/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include <iostream>
using namespace std;
#include "Signal.h"

void Signal::CreateSignalstruct(long NPoints) {
  sSignal.NumberOfPoints = NPoints;
  sSignal.t  = new double[NPoints];
  sSignal.Mx = new double[NPoints];
  sSignal.My = new double[NPoints];
  sSignal.Mz = new double[NPoints];
  for (long i=0;i<NPoints; i++)
  {
     sSignal.t[i] = 0;
     sSignal.Mx[i] = 0;
     sSignal.My[i] = 0;
     sSignal.Mz[i] = 0;
  }
};

void Signal::writeBinaryFile(string filename) {
    ofstream fout(filename.c_str(), ios::binary);
    for (long v=0;v<sSignal.NumberOfPoints;v++)
    {
        fout.write((char *)(&(sSignal.t[v])), sizeof(sSignal.t[v]));
        fout.write((char *)(&(sSignal.Mx[v])), sizeof(sSignal.Mx[v]));
        fout.write((char *)(&(sSignal.My[v])), sizeof(sSignal.My[v]));
        fout.write((char *)(&(sSignal.Mz[v])), sizeof(sSignal.Mz[v]));
    }
     fout.close();
};


void Signal::readBinaryFile(string filename) {
    ifstream fin(filename.c_str(), ios::binary);
    for (long v=0;v<sSignal.NumberOfPoints;v++)
    {
        fin.read((char *)(&(sSignal.t[v])), sizeof(sSignal.t[v]));
        fin.read((char *)(&(sSignal.Mx[v])), sizeof(sSignal.Mx[v]));
        fin.read((char *)(&(sSignal.My[v])), sizeof(sSignal.My[v]));
        fin.read((char *)(&(sSignal.Mz[v])), sizeof(sSignal.Mz[v]));
    }
    fin.close();
};

