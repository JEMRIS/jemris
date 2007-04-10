/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/


/************add for Sequence*/
#include <iostream>
using namespace std;


#include "AtomicSequence.h"

#include <iomanip>
using std::setw;
using std::setiosflags;


#include <stdio.h>
#include <math.h>

//cvode includes
#include "llnltyps.h"
#include "cvode.h"
#include "nvector.h"

//jemris includes
#include "Spin.h"
#include "BCV_MR_Model.h"

//This macro is defined in order to write code which exactly matches
//   the mathematical problem. 
#define Ith(v,i)    N_VIth(v,i-1)         // Ith numbers components 1..NEQ 

// Problem Constants 
#define NEQ   3            // number of equations  
#define RTOL  1e-4         // scalar relative tolerance            
#define ATOL1 1e-6         // vector absolute tolerance components 
#define ATOL2 1e-6
#define ATOL3 1e-6


//this is used as the f_data of cvode, to pass information to the user function (bloch)
static void bloch(integer N, real t, N_Vector y, N_Vector ydot, void *f_data);

void * BlochMalloc( Spin* pSpin,real &reltol,long int iopt[],real ropt[] );

static void bloch(integer N, real t, N_Vector y, N_Vector ydot, void *f_data)
{
  real Mxy,phi,Mz; /*cylndrical components of mangetization*/
  real s,c,Mx,My,Mx_dot,My_dot,Mz_dot;
  Spin* pSpin = (Spin*) f_data;

  real  d_SeqVal[5]={0.0,0.0,0.0,0.0,0.0}; //[B1magn,B1phase,Gx,Gy,Gz]
  if(! pSpin->pASeq->getValue(t,d_SeqVal) ) return;

  real EPS=1e-10;
  real Bx=0,By=0,Bz=0;
  Bx = d_SeqVal[0]*cos(d_SeqVal[1]);  //RF field (real part)
  By = d_SeqVal[0]*sin(d_SeqVal[1]);  //RF field (imaginary part)
  Bz = pSpin->r[0]*d_SeqVal[2]+ pSpin->r[1]*d_SeqVal[3]+ pSpin->r[2]*d_SeqVal[4];

  Bz += pSpin->deltaB;					//add field inhomogenities
  Bz += pSpin->getConcomitantFieldTerm(&d_SeqVal[2]);	//add cocomitant field term
  Bz += pSpin->getNonLinGradTerm(&d_SeqVal[2]);		//add gradient nonlinearity

  //important: restrict phase to [0, 2*PI]
  Ith(y,2) = fmod(Ith(y,2),6.28318530717958);
  Mxy = Ith(y,1); phi = Ith(y,2); Mz = Ith(y,3);

  //avoid CVODE warnings (does not change physics!)
  if (Mxy<ATOL1 && d_SeqVal[0]<EPS)
  {
	Bz=0.0;
	if (fabs(pSpin->M0 - Mz)<ATOL3)
	{
		Ith(y,1) = 0; Ith(y,2) = 0; Ith(y,3) = pSpin->M0;
		Ith(ydot,1) = 0; Ith(ydot,2) = 0; Ith(ydot,3) = 0;
		return;
	}
  }

  //compute cartesian components of transversal magnetization
   c = cos(phi); s = sin(phi);
  Mx = c*Mxy;   My = s*Mxy;

  //compute bloch equations
  Mx_dot = -pSpin->R2*Mx      +Bz*My           -By*Mz;
  My_dot =      -Bz*Mx    -pSpin->R2*My        +Bx*Mz;
  Mz_dot =       By*Mx      - Bx*My      +pSpin->R1*(pSpin->M0 - Mz);

  //compute derivatives in cylindrical coordinates
  Ith(ydot,1) = c*Mx_dot + s*My_dot;
  Ith(ydot,2) = (c*My_dot - s*Mx_dot) / (Mxy>EPS?Mxy:EPS); //avoid division by zero
  Ith(ydot,3) = Mz_dot;
}

//create  cvode_mem 
extern void * BlochMalloc( Spin* pSpin,real &reltol,long int iopt[],real ropt[] )
{
     ((nvec*) (pSpin->SolverSettings))->abstol = N_VNew(NEQ, NULL);

     reltol = RTOL;               // Set the scalar relative tolerance
     Ith( ((nvec*) (pSpin->SolverSettings))->abstol,1 ) = ATOL1;       // Set the vector absolute tolerance 
     Ith( ((nvec*) (pSpin->SolverSettings))->abstol,2 ) = ATOL2;
     Ith( ((nvec*) (pSpin->SolverSettings))->abstol,3 ) = ATOL3;

     void * cvode_mem ;
     cvode_mem = CVodeMalloc(NEQ, bloch, 0, ((nvec*) (pSpin->SolverSettings))->y, ADAMS, FUNCTIONAL, SV, &reltol,
                             ((nvec*) (pSpin->SolverSettings))->abstol,  pSpin, NULL, TRUE, iopt, ropt, NULL);
     return  cvode_mem;

}

