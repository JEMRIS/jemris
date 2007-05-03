/*
  This file is part of the MR simulation project
  Date: 03/2006
  Authors:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _CONCATSEQUENCE_H_
#define _CONCATSEQUENCE_H_

#include "Sequence.h"
#include "PulseShape.h"

#define CHILDRENMAX 4096

class ConcatSequence :public Sequence {
	
 public:
	
	ConcatSequence(string sName = "ConcatSequence" , int iRepetitions=1) { 
        setParent(this);
        setName(sName); 
       	setRepetitions(iRepetitions);
		for (int i=0; i<=CHILDRENMAX; ++i) 
			m_cChildrenPtr[i]=NULL;
	};

	//delete seq-tree from the top
	~ConcatSequence() { 
		if (getParent()==this)
			Destroy(); 
	}; 
	
	Sequence* getChild      (int ChildID) {
		return m_cChildrenPtr[ChildID];
	};
	
	void setRepetitions     (const int iRepetition); 
	
	int  getRepetitions     () {
		return  m_iRepetitions;
	};
	
	int getNumberOfChildren () {
		int i; 
		for (i=0;i<CHILDRENMAX; i++) {
			if (m_cChildrenPtr[i]==NULL) {break;}
		}; return i;
	};
	
	void getInfo(int ident) {
		cout << "ConcatSequence: Name = '" << getName() 
			 << "' , duration = "          << getDuration()
			 << " , Repetitions = "        << getRepetitions() 
			 << " , Children = "           << getNumberOfChildren() 
			 << endl;
		for (int i=0;i<getNumberOfChildren(); i++) { 
			for (int j=0;j<ident;j++) cout << " ";
			cout << " Child " << i+1 << ": ";
			if (m_cChildrenPtr[i]->getNumberOfChildren() > 0)
				m_cChildrenPtr[i]->getInfo(ident+i);
			else
				m_cChildrenPtr[i]->getInfo(ident);
		}
	};
	
  double getDuration ();
  bool   getValue    (const double time, double * const d_AllValuesPtr);
  bool   InsertChild (Sequence* const sSeqOldChildPtr,
					  Sequence* const sSeqNewChildPtr);
  bool   RemoveChild (Sequence * const sSeqToRemovePtr);
  bool   Prepare     (bool verbose);
  void   Destroy     ();
  void   writeADCs   (ofstream* pfout);

  PulseShape* FindPulse(string sPulseName);

  void   getLoopMethod (int val)
	  {m_getLoopMethod=val;};
  int    getLoopMethod ()
	  {return m_getLoopMethod;};

  void   setLoop       (int value )
	  {m_iLoop=value;};
  int    getLoop       ();

  void writeSeqDiagram (string filename);
  void writeSeqVal     (double& dTimeShift, ofstream* pfout);

  //inserting more than one child at a time ... only up to 10 
  bool InsertChild (Sequence* const pSeqOld, Sequence* const pSeqNew1, 
					Sequence* const pSeqNew2) {
	  bool btag1 = InsertChild(pSeqOld, pSeqNew1);
	  bool btag2 = InsertChild(pSeqNew1, pSeqNew2);
	  return (btag1 && btag2);
  };

  bool InsertChild (Sequence* const pSeqOld, Sequence* const pSeqNew1, 
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3) {
	  bool btag1 = InsertChild(pSeqOld, pSeqNew1, pSeqNew2);
	  bool btag2 = InsertChild(pSeqNew2, pSeqNew3);
	  return (btag1 && btag2);
  };

  bool InsertChild (Sequence* const pSeqOld,  Sequence* const pSeqNew1, 
					Sequence* const pSeqNew2, Sequence* const pSeqNew3,
					Sequence* const pSeqNew4) {
	  bool btag1 = InsertChild(pSeqOld, pSeqNew1, pSeqNew2, pSeqNew3);
	  bool btag2 = InsertChild(pSeqNew3, pSeqNew4);
	  return (btag1 && btag2);
  };

  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5) {
	  bool btag1 = InsertChild(pSeqOld, pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4);
	  bool btag2 = InsertChild(pSeqNew4, pSeqNew5);
	  return (btag1 && btag2);
  };

  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5,
				   Sequence* const pSeqNew6) {
	  bool btag1 = InsertChild(pSeqOld, pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4,
							   pSeqNew5);
	  bool btag2 = InsertChild(pSeqNew5, pSeqNew6);
	  return (btag1 && btag2);
  };

  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5,
				   Sequence* const pSeqNew6, Sequence* const pSeqNew7) {
	  bool btag1 = InsertChild(pSeqOld,  pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4,
							   pSeqNew5, pSeqNew6);
	  bool btag2 = InsertChild(pSeqNew6, pSeqNew7);
	  return (btag1 && btag2);
  };

  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5,
				   Sequence* const pSeqNew6, Sequence* const pSeqNew7,
				   Sequence* const pSeqNew8) {
	  bool btag1 = InsertChild(pSeqOld,  pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4,
							   pSeqNew5, pSeqNew6, pSeqNew7 );
	  bool btag2 = InsertChild(pSeqNew7, pSeqNew8);
	  return (btag1 && btag2);
  };
  
  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5,
				   Sequence* const pSeqNew6, Sequence* const pSeqNew7,
				   Sequence* const pSeqNew8, Sequence* const pSeqNew9) {
	  bool btag1 = InsertChild(pSeqOld,  pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4,
							   pSeqNew5, pSeqNew6,pSeqNew7, pSeqNew8 );
	  bool btag2 = InsertChild(pSeqNew8, pSeqNew9);
	  return (btag1 && btag2);
  };
  
  bool InsertChild(Sequence* const pSeqOld,  Sequence* const pSeqNew1,
				   Sequence* const pSeqNew2, Sequence* const pSeqNew3,
				   Sequence* const pSeqNew4, Sequence* const pSeqNew5,
				   Sequence* const pSeqNew6, Sequence* const pSeqNew7,
				   Sequence* const pSeqNew8, Sequence* const pSeqNew9,
				   Sequence* const pSeqNew10) {
	  bool btag1 = InsertChild(pSeqOld,  pSeqNew1, pSeqNew2, pSeqNew3, pSeqNew4,
							   pSeqNew5, pSeqNew6, pSeqNew7, pSeqNew8, pSeqNew9);
	  bool btag2 = InsertChild(pSeqNew9, pSeqNew10);
	  return (btag1 && btag2);
  };
  
  
 private:
  int 			m_iRepetitions;
  Sequence* 	m_cChildrenPtr[CHILDRENMAX+1]; //we need one addtional NULL after full list
  int			m_getLoopMethod;
  int           m_iLoop; 
  
};

#endif
