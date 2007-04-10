/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _MR_MODEL_H_
#define _MR_MODEL_H_

//system includes
#include <iostream>
#include <string>
#include <iomanip>
#include <math.h>
using namespace std;

//SEQ_FRAMEWORK includes
#include "AtomicSequence.h"
#include "AllPulses.h" //currently, only for PhaseLock() of the signal needed
#include "Signal.h"
#include "Sample.h"
#include "Spin.h"

//! The MR model class is an ABC, it has no model solver implemented
/*! This class provides basic functionality to travel through the MR
 *  sequence, subsequently for each spin of the sample.
 *  Further, it has methods to set up the spins and to calculate certain
 *  off-resonance effects.
 */
class  MR_Model
{
public:

  //! The constructor creates a new signal, which is returned by the solve() method
  MR_Model(){
	m_pSig = new Signal; 
	m_pSpin = new Spin;
	m_dNoiseLevel = 0.0;
  	m_iNumOfDbTerms =0;
	m_B0overGMAX = -1.0; //negative means: ignore concomitant field term
	m_SusceptFact   = 0.0;
	m_ChemShiftFact = 0.0;
	m_i_rand_init = (-1) * (long ) clock(); //long int which initialise the random number generator
  };
  ~MR_Model(){delete m_pSpin; };

//! The most important public function to solve an MR model.
/*! This method loops over all spins in the sample and call methods for
 *  setting up (or changing) spins, followed by travelling the sequence.
 *  The solver can	1. continue from the last state (cont = true),
 * 			2. remember the result state (remember = true), and
 * 			3. store intermediate states to a binary result file
 */
  Signal* Solve(bool cont=false, bool remember=true, string* fn=NULL, int iStep=0);
  
  //! Get pointer to the current transverse magnitudes of the sample
  double* getMm() {return m_pSam->sSample.Mm;  };
  //! Get pointer to the current transverse phases of the sample
  double* getMp() {return m_pSam->sSample.Mp;  };
  //! Get pointer to the magnitude components, which where stored in the last call to Solve(cont,true,...)
  double* getMxy() {return m_pSam->sSample.Mxy;  };
  //! Get pointer to the phase components, which where stored in the last call to Solve(cont,true,...)
  double* getMphi(){return m_pSam->sSample.Mphi; };
  //! Get pointer to the longitudinal components, which where stored in the last call to Solve(cont,true,...)
  double* getMz()  {return m_pSam->sSample.Mz;   };

  //! set the MR sequence for this MR model
  void setSequence(Sequence* seq){m_pSeq=seq;};

  //! set a factor for susceptibility induced off-resonance
  void setSusceptFact(double dVal){m_SusceptFact = dVal;};
  //! set a factor for chemical shift induced off-resonance
  void setChemShiftFact(double dVal){m_ChemShiftFact = dVal;};
  //! set main-field-over-gradient-strength-ratio for concomitant fields
  void setB0overGMAX(double dVal){m_B0overGMAX = dVal;};
  //! set noise level for random B field fluctations (T2* effect) [kHz] 
  void setNoiseLevel(double dVal){m_dNoiseLevel = dVal;};
  //! set a deterministic field inhomogeneity term
  void setFieldTerm(int iTerm, double dFactor, double dRadius= - 1.0,
		    double dX0=0.0, double dY0=0.0, double dZ0=0.0);
  //! set pointers to doubles for quadratic and cubic nonlinear gradient terms
  void setNonLinG(double* qt, double* ct);

protected:
//!  Method for recursevely travelling the sequence.
/*! This method calls the (pure virtual) method MRCalculete whenever an AtomicSequence
 *  is reached in the sequence tree.
 */
  void TravelSequence( double& dTimeShift, long& lIndexShift, Sequence* pSeq , long& lSpin, ofstream* pfout, int iStep);

//!  Method to solve the MR Model for a specific AtomicSequence.
  virtual void MRCalculate(double& dTimeShift, long& lIndexShift, AtomicSequence* pA , long& lSpin, ofstream* pfout, int iStep )=0;
//!  Method to print out final statistics of the solver.
  virtual void PrintFinalStats()=0;

  //! set up a spin from the sample
  void setSpin(int k);
  //! Calculate field inhomogeneity for a spin at position (dx,dy,dz) 
  double getDeltaB(double dx,double dy, double dz);
  //! re-use a spin, with potentially non-equilibrium magnetisation
  void useSpin(int k);
  //! empty virutal method for solver init, which is called from setSpin() and useSpin() 
  virtual void initSolver(){ };
  //! empty virutal method, which is called everytime the solver has finished one spin 
  virtual void freeSolver(){ };

//! Method to add contriubtions to the MR signal. The method needs to be called from 
  void AddToSignal(int n, double Tadc,AtomicSequence* pA);
//! Method to save intermediate state of the sample to a result file.
  void SaveEvolution(ofstream* pfout, long lSpin, double time);

//private:
  Sequence* m_pSeq; 
  Signal* m_pSig; 
  Sample* m_pSam; 
  Spin* m_pSpin;
  double m_dADCs[4096];
  double m_MagCarCord[3];
  double m_MagCylCord[3];
  //for random field fluctuations
  long m_i_rand_init ;
  double m_dNoiseLevel;
  //field inhomogeneities terms
  int m_iNumOfDbTerms;
  double m_dFactor[256],m_dRadius[256],m_dX0[256], m_dY0[256], m_dZ0[256];
  int m_iTerm[256];

  double m_B0overGMAX    ;
  double m_SusceptFact   ;
  double m_ChemShiftFact ;

  double m_dTimeShift;
  long m_lIndexShift ;
  bool m_bTMP;  
};//end Class MR_Model 


#endif
