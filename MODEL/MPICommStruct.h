/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _MPICOMMSTRUCT_H_
#define _MPICOMMSTRUCT_H_


#include <stdexcept>
#include <string>

using std::string;

#include <fstream>
using std::ofstream;
#include <cstdlib>

/*
struct Signalstruct
   {
   long    NumberOfPoints;
   double * Mx;
   double * My;
   double * Mz;
   double * Kx;
   double * Ky;
   double * Kz;
   };
*/
struct Signalsubstruct
   {
   double * Mx;
   double * My;
   double * Mz;
   };
/*
struct Samplestruct
   {
   long   NumberOfPoints;
   double * M0;
   double * RT1;
   double * RT2;
   double * Xcoord;
   double * Ycoord;
   double * Zcoord;
   };
*/
struct Protocolstruct
   {
   long          lNumber_of_Items;
   char          sTitle[50];
   char          **ItemName;
   double        *ItemValue;
   char          **ItemUnit;
   };

#endif
