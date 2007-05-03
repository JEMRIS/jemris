/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _ATOMICSEQUENCE_H_
#define _ATOMICSEQUENCE_H_

#define MAXPULSENUMBER 100

// CVODE doesn't like the first pulse at time 0
#define TIME_ERR_TOL 1e-6 

#include <cmath>

#include "Sequence.h"
#include "ConcatSequence.h"
#include "PulseShape.h"
#include "ConcatPulseShape.h"
#include "EmptyPulse.h"

class AtomicSequence  :public Sequence{

public:
  AtomicSequence(string sName = "AtomicSequence" ) {
        setParent(this);
        setName(sName); 
        m_dDuration= 0.0;
	for (int i=0;i<MAXPULSENUMBER;i++) { m_pPulseShapes[i]=NULL; }
	m_bDoSetListOfTPs=true;
  };
  ~AtomicSequence(){};

  //public methods implemented here
  double       getDuration() {return m_dDuration;};
  void         setDuration(const double dDuration) { if (dDuration >m_dDuration) m_dDuration = dDuration; };
  PulseShape*  getPulse(int i) { return  (m_pPulseShapes[i]); };
  int getNumberOfPulses(){ for (int j=0;j<MAXPULSENUMBER;j++) { if (getPulse(j)==NULL) {return j;} } };
  int getNumOfTPs() {return m_iTPs;};
  int getNumOfADCs() {int i=0;for (int j=0;j<getNumOfTPs();j++) { if (m_bTPs[j]) i++;} return i;}
  double* getdTPs() {return &m_dTPs[0]; };
  bool* getbTPs() {return &m_bTPs[0]; };
  void getInfo(int ident){
	cout << "AtomicSequence: Name = '" << getName() << "' , duration = "<< getDuration()
	     << " , ADCs = " << getNumOfADCs() << " , Pulses = " << getNumberOfPulses() << endl;
  	for (int i=0;i<getNumberOfPulses();i++)
	{
		for (int j=0;j<ident;j++) cout << " ";
		cout << "  Pulse " << i+1 << " -> ";
		cout << "PulseShape: Name = '" << m_pPulseShapes[i]->getName()
		     << "' , duration = "<< m_pPulseShapes[i]->getDuration()
		     << " , ADCs = " << m_pPulseShapes[i]->getNumOfADCs() << endl;
	}
  };

  //public methods implemented in cpp file
  bool getValue(const double time , double * const d_AllValuesPtr);
  bool setPulse(PulseShape  *pPulse , int iTreeSteps=0 );
  bool JoinAtom(AtomicSequence * pAtom, double delay );
  bool Prepare(bool verbose);
  void Destroy();
  void writeADCs(ofstream* pfout);
  void setListOfTimepoints(bool force);
  void writeSeqVal(double& dTimeShift,ofstream* pfout);
  PulseShape* FindPulse(string sPulseName);

protected:
	double		m_dDuration;
   	PulseShape*	m_pPulseShapes[MAXPULSENUMBER];
	int		m_iTPs;
	bool		m_bTPs[2*PULSESHAPE_MAXPOINTS];
	double		m_dTPs[2*PULSESHAPE_MAXPOINTS];
	bool		m_bDoSetListOfTPs;
};

#endif
