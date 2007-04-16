/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _DELAYATOM_H_
#define _DELAYATOM_H_

#include "AtomicSequence.h"

enum DelayType {DELAY_B2E, DELAY_C2C, DELAY_B2C, DELAY_C2E};

class DelayAtom: public AtomicSequence{

public:
  DelayAtom(double dRTime=0.0, Sequence* pS1=NULL, Sequence* pS2=NULL,
	    DelayType DT=DELAY_B2E, int iNADC=0, string sName = "DelayAtom" ) {
	m_dRTime=dRTime;
	m_pS1=pS1;
	m_pS2=pS2;
	m_iNADC=iNADC;
	m_DT=DT;
	m_bSearchStartStopSeq=false;
	m_bUseHalfTE=false;
	m_bUseTE=false;
	m_bUseTR=false;
	m_bUseTI=false;
	m_bUseTD=false;
	m_dFactor=-1.0;
	setName(sName); 
	m_sPulse="NULL";
  };
  ~DelayAtom(){};
  
  void setStartStopSeq(string S1, string S2){ m_S1=S1; m_S2=S2; m_bSearchStartStopSeq=true; };
  void setDelayFromPulse(string S1){ m_sPulse=S1;};
  void useTE(){ m_bUseTE=true; };
  void useHalfTE(){ m_bUseHalfTE=true; };
  void useTR(){ m_bUseTR=true; };
  void useTD(){ m_bUseTD=true; };
  void useTI(){ m_bUseTI=true; };
  void setFactor(double val){ m_dFactor=val; };

  //Overload Prepare(). This atom is configured after the tree is build!
  bool Prepare(bool verbose) { 
	//search pointers of start and stop sequence in sequence tree
	if (m_bSearchStartStopSeq) searchStartStopSeq();
	//take rest time from parameter of the root sequence
	if (m_bUseTR)     m_dRTime = getRoot()->getParameter()->getTR();
	if (m_bUseTE)     m_dRTime = getRoot()->getParameter()->getTE();
	if (m_bUseHalfTE) m_dRTime = 0.5*getRoot()->getParameter()->getTE();
	if (m_bUseTI)     m_dRTime = getRoot()->getParameter()->getTI();
	if (m_bUseTD)     m_dRTime = getRoot()->getParameter()->getTD();
	PulseShape* pLinkedPulse = NULL;
	if (m_sPulse!="NULL") pLinkedPulse = getRoot()->FindPulse(m_sPulse);
	if (pLinkedPulse!=NULL) m_dRTime = pLinkedPulse->getDuration();
	if (m_dFactor>0.0) m_dRTime *= m_dFactor;
	//clean up first (then start from scratch)
	m_dDuration=0.0;
	for (int i=0;i<getNumberOfPulses();++i) {delete getPulse(i);};
	m_pPulseShapes[0]=NULL;
	//trivial case: no seqs defined => RTime specifies the complete delay
	if (m_pS1==NULL && m_pS2==NULL)
	{
		m_dDelayTime = m_dRTime;
  		setPulse(new EmptyPulse(m_dDelayTime,m_iNADC,"DELAYTIME") );
		return true;
	}
	//check for my parent
	Sequence* pSeq=getParent();
	if (pSeq==this && verbose){ cout << "Prepare() warning: not in tree " << getName() << endl; return false; }
	//calculate delay according to the DelayType
	double dT1=0.0, dT2=0.0;
	if (m_pS1!=NULL)
		switch (m_DT)
		{
			case DELAY_B2E : dT1=m_pS1->getDuration();	break;
			case DELAY_C2E : dT1=0.5*m_pS1->getDuration();	break;
			case DELAY_B2C : dT1=m_pS1->getDuration();	break;
			case DELAY_C2C : dT1=0.5*m_pS1->getDuration();	break;
			default: cout << "?? unkown DelayType ??" << endl; return false;
		}
	if (m_pS2!=NULL)
		switch (m_DT)
		{
			case DELAY_B2E : dT2=m_pS2->getDuration();	break;
			case DELAY_C2E : dT2=m_pS2->getDuration();	break;
			case DELAY_B2C : dT2=0.5*m_pS2->getDuration();	break;
			case DELAY_C2C : dT2=0.5*m_pS2->getDuration();	break;
			default: cout << "?? unkown DelayType ??" << endl; return false;
		}
	m_dDelayTime = m_dRTime-dT1-dT2;
	
	//subtract duration of other sequences between pS1 (pS2) and myself
	int iMYpos, iS1pos=CHILDRENMAX, iS2pos=-1;
	for (int i=0;i<pSeq->getNumberOfChildren();++i)
	{
		if(this == pSeq->getChild(i)) iMYpos=i;
		if(m_pS1== pSeq->getChild(i)) iS1pos=i;
		if(m_pS2== pSeq->getChild(i)) iS2pos=i;
	}
	for (int i=iS1pos+1;i<iMYpos;++i) m_dDelayTime -= pSeq->getChild(i)->getDuration();
	for (int i=iMYpos+1;i<iS2pos;++i) m_dDelayTime -= pSeq->getChild(i)->getDuration();
	//error conditions
	if ( verbose &&  ( (m_pS1!=NULL && iS1pos>=iMYpos)|| (m_pS2!=NULL && iS2pos<=iMYpos) ) )
		{ cout << "Prepare() warning. wrong ordering : " << getName() << endl; return false; }
	if ( verbose && m_dDelayTime<0 )
		{ cout << "Prepare() warning. negative delaytime : " << getName() << endl; return false;}
	//finally set pulse
  	setPulse(new EmptyPulse(m_dDelayTime,m_iNADC,"DELAYTIME") );
	return true;
  };


protected:
 void searchStartStopSeq() {
	Sequence* pSeq=getParent();
	int i1=0, i2=pSeq->getNumberOfChildren();
	for (int i=0;i<pSeq->getNumberOfChildren();++i)
	{
		if( m_S1 == pSeq->getChild(i)->getName() ) { m_pS1 = pSeq->getChild(i); i1=i; }
		if( m_S2 == pSeq->getChild(i)->getName() ) { m_pS2 = pSeq->getChild(i); i2=i; }
	}
	if (i1>=i2) cout << getName() << " warning: setting Start/Stop Sequence not successful" << endl;
 };

 void getInfo(int ident){
	cout << "DelayAtom: Name = '" << getName() << "' , duration = "<< getDuration()
	     << " , ADCs = " << getNumOfADCs() << endl;
 };

private:
	Sequence* m_pS1;
	Sequence* m_pS2;
	double m_dRTime;
	double m_dDelayTime, m_dFactor;
	int m_iNADC;
	DelayType m_DT;
	bool m_bSearchStartStopSeq, m_bUseTR, m_bUseHalfTE, m_bUseTE, m_bUseTI, m_bUseTD;
	string m_S1, m_S2, m_sPulse;
};

#endif
