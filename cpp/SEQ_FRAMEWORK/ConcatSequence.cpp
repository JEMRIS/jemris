/*
  This file is part of the MR simulation project
  Date: 03/2006
  Authors:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include <iostream>
using namespace std;
using std::ofstream;
using std::cout;
#include <iomanip>
using std::setw;
using std::setiosflags;

#include "ConcatSequence.h"

/******************************************************************/
bool ConcatSequence::InsertChild ( Sequence* const sSeqOldChildPtr,
								   Sequence* const sSeqNewChildPtr) {
	
	if (sSeqNewChildPtr== NULL )  return false;
	if (getNumberOfChildren() == CHILDRENMAX ) return false;
	
  	//insert the first child
	if ( (m_cChildrenPtr[0] == NULL || m_cChildrenPtr[0] == this) && sSeqOldChildPtr==NULL) {
		m_cChildrenPtr[0] = sSeqNewChildPtr;
		sSeqNewChildPtr->setParent(this);
		return true;
	}
	
  	//insert somewhere in the tree
	int pos=-1; 
	for (int i=0;i<getNumberOfChildren();++i)
		if ( getChild(i) == sSeqOldChildPtr) { pos=i; break; }
	for (int i=getNumberOfChildren()-1; i>pos; i--)
		m_cChildrenPtr[i+1] = m_cChildrenPtr[i];
	m_cChildrenPtr[pos+1] = sSeqNewChildPtr;
	sSeqNewChildPtr->setParent(this);
	return true;
};

/******************************************************************/
PulseShape* ConcatSequence::FindPulse (string sPulseName) {
	PulseShape* pPulse=NULL;
	for (int i=0;i<getNumberOfChildren();++i) {
		pPulse = getChild(i)->FindPulse(sPulseName);
		if (pPulse!=NULL)
			return pPulse;
	}
	return pPulse;
};

/******************************************************************/
int ConcatSequence::getLoop()   {
	ConcatSequence* S=this;
	int prev_loops=0;
	if (getTreeSteps()>0)
	{
		for(int i=0;i<getTreeSteps() ;++i){ S=(ConcatSequence*)S->getParent(); }  
		prev_loops = getRepetitions()* S->getLoop();
	}
	return m_iLoop+prev_loops;
};

/******************************************************************/
void ConcatSequence::setRepetitions(const int iRepetition) {
	m_iRepetitions = iRepetition ;
	ConcatSequence* S=this;
	if (getTreeSteps()>0)
		for(int i=0;i<getTreeSteps() ;++i){ S=(ConcatSequence*)S->getParent(); }  
	if (S!=this && S!=NULL)
	{
		int par_rep = (int) iRepetition/((int) getFactor());
		int my_rep = (int) getFactor();
		if (my_rep * par_rep != iRepetition )
		{
			cout << getName() << " warning: set repetitions failed" << endl;
			return;
		}
		S->setRepetitions(par_rep);
		m_iRepetitions = my_rep ;
	}
};
/******************************************************************/
bool ConcatSequence::Prepare (bool verbose) {
	
	bool btag=true;
	
	if (NewParam()) { //get repetitions from root parameter 
		if (getLoopMethod() == 1) 
			setRepetitions( (int) ( getFactor()*getRoot()->getParameter()->getNx() ) );
		if (getLoopMethod() == 2)
			setRepetitions( (int) ( getFactor()*getRoot()->getParameter()->getNy() ) );
		NewParam(false);
	}
	
	for (int i=0; i<getNumberOfChildren(); ++i)
		btag = (btag && getChild(i)->Prepare(verbose));
	if (getParent()==this && !btag && verbose) //only the top node of the tree cries out
		cout << "warning in Prepare() of sequence: " << getName() << endl;
	return btag;

};

/******************************************************************/
  bool ConcatSequence::RemoveChild(Sequence * const sSeqToRemovePtr)
      {
        int Number_of_Children=getNumberOfChildren();
        int removechild_position=-1;

        if (Number_of_Children<=1)
	{
		#ifdef  DO_TUT
		cout << "Last Child can not be removed" << endl;
		#endif
		return false;
	}
        
        for (int i=0;i<Number_of_Children;i++)
       		if ( m_cChildrenPtr[i] == sSeqToRemovePtr) { removechild_position = i; } 

	if (removechild_position<0)
	{
		#ifdef  DO_TUT
		cout << "SeqToRemove is not in child list" << endl;
		#endif
		return false;
	}

        for (int i=removechild_position;i<Number_of_Children;i++)
		m_cChildrenPtr[i] = m_cChildrenPtr[i+1];

	sSeqToRemovePtr->Destroy();

        return  true;
};

/******************************************************************************/
/*  get duration of this concat sequence by adding the durations of all       */
/*  child nodes                                                               */
/******************************************************************************/
double ConcatSequence::getDuration() {

	double dDuration = 0.0;

	//run through the repetitions
	for (int i=0; i<getRepetitions(); ++i) {
		
		//set the accessible loop counter
		setLoop(i);

		//accumulate duration of ALL child nodes
		for (int j=0; j<getNumberOfChildren() ; ++j)
			dDuration += m_cChildrenPtr[j]->getDuration();
		
	}

	return( dDuration );

};

/*******************************************************************************/
/*  get value of this sequence for a given time, time and put it into dVal     */
/*******************************************************************************/

bool  ConcatSequence::getValue (const double time, double * const pdVal) {

	//only do something, when within duration of this sqeuence
	if (time < 0.0 || time > getDuration()) { return false; }
	
	double dRemTime  =  time;
	
	//run through the repetitions
	for (int i = 0; i < getRepetitions(); ++i) {
		setLoop(i);
		
		// run through all child nodes
		for (int j = 0; j < getNumberOfChildren(); ++j) {
			
			//get value tuple from the particular LONGEST time
			if (dRemTime < m_cChildrenPtr[j]->getDuration()) {
				m_cChildrenPtr[j]->getValue(dRemTime,pdVal);
				return true ; 
			}
			dRemTime -= m_cChildrenPtr[j]->getDuration();
		}
	}

	cout << "???" << endl; return false ; //this should never happen !!!

};


/******************************************************************/
void ConcatSequence::writeADCs (ofstream* pfout) {

	int    iN  = getRepetitions();
	double dT  = getDuration()/iN;
	int    iNC = getNumberOfChildren();
	
	*pfout	<< "<concat duration=\"" << dT << "\" children=\"" << iNC 
			<< "\" repetitions=\"" << iN << "\"> " << endl;
	
	for (int i=0;i<iNC;i++) { m_cChildrenPtr[i]->writeADCs(pfout); }
	
	*pfout << "</concat> " << endl;
	
};

/******************************************************************/
void ConcatSequence::Destroy() { //destructor call: recursive delete of the sequence tree
	
	for (int i=0; i< getNumberOfChildren() ; ++i)
		m_cChildrenPtr[i]->Destroy();
	if (getParent()!=this) delete this;
	
};

/******************************************************************/
void ConcatSequence::writeSeqDiagram (string filename) {

	ofstream fout (filename.c_str(), ios::binary);
	double   dTime          =  0.0;
	writeSeqVal(dTime, &fout);
	double   dEndOfFileFlag = -1.0;
	fout.write((char *)(&(dEndOfFileFlag)), sizeof(dEndOfFileFlag));
	fout.close();

};

/******************************************************************/
void ConcatSequence::writeSeqVal (double& dTimeShift, ofstream* pfout) {
	
	for (int i=0; i<getRepetitions(); ++i) {
		setLoop(i);
		for (int j=0; j<getNumberOfChildren(); ++j)
			m_cChildrenPtr[j]->writeSeqVal(dTimeShift, pfout);
		
	}
};
