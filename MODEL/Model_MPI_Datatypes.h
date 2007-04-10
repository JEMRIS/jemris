/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _MODEL_MPI_DATATYPES_H_
#define _MODEL_MPI_DATATYPES_H_

#include "Signal.h"
#include "Sample.h"

MPI_Datatype  MPISample(long NPoints, struct Samplestruct &sSample ) {

  MPI_Datatype Sampletype ;
  MPI_Datatype  type[9] =
       {MPI_LONG, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
  int          blocklen[9] = {1, NPoints,NPoints,NPoints,NPoints,NPoints,NPoints,NPoints,NPoints};
  MPI_Aint     disp[9];
  int          base;
  MPI_Address( &(sSample.NumberOfPoints), disp);
  MPI_Address( &(sSample.M0[0]), disp+1);
  MPI_Address( &(sSample.RT1[0]), disp+2);
  MPI_Address( &(sSample.RT2[0]), disp+3);
  MPI_Address( &(sSample.CHI[0]), disp+4);
  MPI_Address( &(sSample.CS[0]), disp+5);
  MPI_Address( &(sSample.Xcoord[0]), disp+6);
  MPI_Address( &(sSample.Ycoord[0]), disp+7);
  MPI_Address( &(sSample.Zcoord[0]), disp+8);
  base = disp[0];
  for (int i=0; i <9; i++) disp[i] -= base;
  MPI_Type_struct( 9, blocklen, disp,
               type, &Sampletype);
  MPI_Type_commit( &Sampletype);
  return  Sampletype;
};

MPI_Datatype  MPISignal(long NPoints, struct Signalstruct &sSignal )
{
  MPI_Datatype Signaltype ;
  MPI_Datatype  type[5] = {MPI_LONG, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE  };
  int          blocklen[5] = {1, NPoints,NPoints,NPoints,NPoints};
  MPI_Aint     disp[5];
  int          base;

  MPI_Address( &(sSignal.NumberOfPoints), disp);
  MPI_Address( sSignal.t, disp+1);
  MPI_Address( sSignal.Mx, disp+2);
  MPI_Address( sSignal.My, disp+3);
  MPI_Address( sSignal.Mz, disp+4);
  base = disp[0];
  for (int i=0; i <5; i++) disp[i] -= base;
  MPI_Type_struct( 5, blocklen, disp, type, &Signaltype);
  MPI_Type_commit( &Signaltype);

  return  Signaltype;
};

#endif
