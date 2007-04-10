/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _BCV_MR_MODEL_H_
#define _BCV_MR_MODEL_H_

//system includes
#include <iostream>
#include <string>
#include <iomanip>
#include <math.h>
using namespace std;

#include "MR_Model.h"

//CVODE include, macros, and constans 
#include "cvode.h"
#define Ith(v,i)    N_VIth(v,i-1)         // Ith numbers components 1..NEQ 
#include "nvector.h"  // definitions of type N_Vector and macro N_VIth,    
#define NEQ   3            // number of equations  
#define RTOL  1e-4         // scalar relative tolerance            
#define ATOL1 1e-6         // vector absolute tolerance components 
#define ATOL2 1e-6
#define ATOL3 1e-6

//! A small helper structure, keeping CVODE N_vectors
struct nvec { N_Vector y; N_Vector abstol; }; //structure keeping the vectors for cvode 

//! The BCV-Model class - solves Bloch equation with CVode library
/*! A derived class, which solves the MR model with a complete general
 *  numerical solution using a variable time stepping solver.
 */
class  BCV_MR_Model :public MR_Model{
public:
  //! The BCV_MR_Model is invoked with pointers to the sequence and the sample
  BCV_MR_Model(Sequence* pSeq, Sample* pSam){
	m_pSeq = pSeq;
	m_pSam = pSam;
	m_pSpin->SolverSettings = new nvec;
	for (int i=0;i<OPT_SIZE;i++) {m_iopt[i]=0; m_ropt[i]=0.0;}
	m_iopt[MXSTEP]=100000;
	m_ropt[HMAX]=100000.0;// the maximum stepsize in msec of the integrator
	m_reltol=RTOL;
  };
  ~BCV_MR_Model(){ };

protected:
  //! intitaliase N_vector and attach it to the spin
  void initSolver ( ) { ((nvec*) (m_pSpin->SolverSettings))->y = N_VNew(NEQ, NULL); }
  //! clear N_vector 
  void freeSolver ( ) { N_VFree( ( ((nvec*) (m_pSpin->SolverSettings))->y) ); }

  //! Bloch Eq. numerical integration for an atomic sequence
  void MRCalculate(double& dTimeShift, long& lIndexShift, AtomicSequence* pA , long& lSpin, ofstream* pfout, int iStep );
  //! Prints some statistics of the numerics
  void PrintFinalStats();

private:
  //CVODE related
  long m_iopt[OPT_SIZE];
  real m_ropt[OPT_SIZE], m_reltol;

};


#endif
