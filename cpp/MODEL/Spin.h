/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _SPIN_H_
#define _SPIN_H_

#include "AtomicSequence.h"

//! A very simple spin class: everything is public!
/*! The Spin carries information on its physical properties and location.
 *  Further, it knows the off-resonance (Delta B) at its position and it
 *  has some methods to calculate gradient-dependent off-resonances.
 */
class Spin {
public:
 Spin(){bNonLinG=false;};
 ~Spin(){};

 //! equilibrium magnetisation [a.u.]
 double M0;
 //! longitudinal relaxation rate (1/T1) [ms]
 double R1;
 //! transverse relaxation rate (1/T2) [ms]
 double R2;
 //! susceptibility [TMPTMP]
 double CHI;
 //! chemical shift [%]
 double CS;
 //! Position-vector of the spin [mm]
 double r[3];
 //! Variable containing all static off-resonance effects (not depending on gradients)
 double deltaB;
 //! Variable defining the strength of concomitant gradients on the spin
 double B0overGMAX ;
 //! Defines, whether nonlinear gradients should be considered
 bool bNonLinG;
 //! Vectors for quadratic and cubic nonlinearity terms of the gradients
 double qt[3],ct[3];
 //! The sequence atom in action 
 AtomicSequence* pASeq;
 //! Anything, which the solver has to connect to the spin
 void* SolverSettings;

 //! Function to get the concomitant field off-resonance term
 double getConcomitantFieldTerm(double* G){
	if (B0overGMAX<=0.0) return 0.0;
	double cft = (0.5/B0overGMAX)*( pow(G[0]*r[2]-0.5*G[2]*r[0],2) + pow(G[1]*r[2]-0.5*G[2]*r[1],2) ) ; 
	return cft;
 };

 //! Function to get the nonlinear gradient off-resonance term
 double getNonLinGradTerm(double* G){
	if (! bNonLinG) return 0.0;
	double nlg =	qt[0]*G[0]*pow(r[0],2) + qt[1]*G[1]*pow(r[1],2) + qt[2]*G[2]*pow(r[2],2) + 
			ct[0]*G[0]*pow(r[0],3) + ct[1]*G[1]*pow(r[1],3) + ct[2]*G[2]*pow(r[2],3) ;
	return nlg;
 };
 
};

#endif
