/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <stdexcept>
#include <string>
using std::string;
#include <fstream>
using std::ofstream;
#include <cstdlib>

#define  SIGNALMAXPOINTS 1000000

//! A structure, containing the signal.
/*! This struct is needed for sending a signal with MPI.
*/
struct Signalstruct {
   long    NumberOfPoints;
   double * t;
   double * Mx;
   double * My;
   double * Mz;
};

//! A class, containing the signal.
/*! Has methods reading/writing signals from/to binary files.
*/
class  Signal {
public:

 //! create empty signal of size N (all values zero)
 Signal(long N=SIGNALMAXPOINTS) { CreateSignalstruct(N); };
 ~Signal() { };

//! create empty signal structure of size NPoints (all values zero)
  void CreateSignalstruct(long NPoints);
//! write signal to binary file
  void writeBinaryFile(string filename);
//! read signal from binary file
  void readBinaryFile(string filename);

  struct Signalstruct sSignal;
};

#endif
