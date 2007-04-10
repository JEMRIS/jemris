/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#define SAMPLEMAXPOINTS 1000000
#include <stdexcept>
#include <string>

using std::string;

#include <fstream>
using std::ofstream;
#include <cstdlib>

//! A structure, containing the information of the sample.
/*! This struct is needed for sending a sample with MPI.
 *  All variables concerning the state of the magnetisation
 *  (Mxy,Mphi,Mz & Mm,Mp,Ml) are not part of the
 *  corresponding MPI structure for sending the sample,
 *  since they are always of local interest. 
 */
struct Samplestruct {
//! number of points (spins) in the sample
   long   NumberOfPoints;
//! equilibrium magnetisation
   double * M0;
//longitudinal relaxation
   double * RT1;
//! transversal relaxation
   double * RT2;
//! susceptibility
   double * CHI;
//! chemical shift
   double * CS;	
//! positional vector x coordinate
   double * Xcoord;
//! positional vector y coordinate
   double * Ycoord;
//! positional vector z coordinate
   double * Zcoord;
//  / to remember		| these are *not* within the MPI datatype, i.e.
// <  a specific 		| not intended for send/receive, since
//  \ magnetisation state	| the information is only of local interest.
//! transverse magnitude component of magnetisation (only updated on request from MR_Model !)
   double * Mxy; 
//! transverse phase component of magnetisation (only updated on request from MR_Model !)
   double * Mphi;
//! longitudinal component of magnetisation (only updated on request from MR_Model !)
   double * Mz;  
//! transverse magnitude component of magnetisation (always updated from MR_Model !)
   double * Mm;
//! transverse phase component of magnetisation (always updated from MR_Model !)
   double * Mp;
//! longitdunal component of magnetisation (always updated from MR_Model !)
   double * Ml;
};

 //! A simple class, containing the information of the sample.
class  Sample {
public:
 //! create empty sample of size N (all values zero)
 Sample(long N=SAMPLEMAXPOINTS) { CreateSamplestruct(N); };
 //! create sample from file
 Sample(string filename) { readBinaryFile(filename); };
 //! the decon deletes double arrays of the sample structure
 ~Sample() {
 	delete sSample.M0;
 	delete sSample.RT1;
 	delete sSample.RT2;
 	delete sSample.CHI;
 	delete sSample.CS;
 	delete sSample.Xcoord;
 	delete sSample.Ycoord;
 	delete sSample.Zcoord;
 	delete sSample.Mxy;
 	delete sSample.Mphi;
 	delete sSample.Mz;
 	delete sSample.Mm;
 	delete sSample.Mp;
 	delete sSample.Ml;
  };

//! creates an empty sample struct of size NPoints (all values equal zero)
 void CreateSamplestruct(long NPoints);
//! creates an empty sample of size NPoints (all values equal zero)
 Sample* getSubSample(long n,int size);
 void writeBinaryFile(string filename);

//! computes {minX,maxX,minY,maxY,minZ,maxZ} and returns pointer to double [6] array
 double* getSampleDim();

 struct Samplestruct sSample;

private:
 void readBinaryFile(string filename);

 double m_dSampleDim[6];

};//end Class Sample

#endif
