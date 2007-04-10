/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include <iostream>
using namespace std;

#include "AtomicSequence.h"

/*****************************************************************************/
bool  AtomicSequence::getValue(const double time , double * const pdVal)
{
	Sequence* sSeq;
	if (time >getDuration()+TIME_ERR_TOL || time< -TIME_ERR_TOL)  return false;

	for (int i=0;i<getNumberOfPulses();++i)
	{
		sSeq=(Sequence *)this;
		for (int j=0;j< getPulse(i)->getTreeSteps(); ++j) {sSeq = sSeq->getParent();}
		if (sSeq == this)
			getPulse(i)->getValue(&pdVal[0],time, 0 );
		else
			getPulse(i)->getValue(&pdVal[0],time, ((ConcatSequence*)sSeq)->getLoop() );
	}
	return true;
};

/*****************************************************************************/
bool AtomicSequence::JoinAtom(AtomicSequence * pAtom,double delay )
{
	if (delay<0 || delay > getDuration() || pAtom->getNumberOfPulses()<1) return false;

	for (int i=0;i<pAtom->getNumberOfPulses();++i)
	{
		setPulse(new ConcatPulseShape(new EmptyPulse(delay), pAtom->getPulse(i)));
        	getPulse(getNumberOfPulses()-1)->setName(pAtom->getPulse(i)->getName());
	}
	delete pAtom;
	return true;
};

/*****************************************************************************/
bool AtomicSequence::setPulse(PulseShape * pPulse,int iTreeSteps )
  {
        pPulse->setTreeSteps (iTreeSteps);
        pPulse->setAtomicSeq( this);
        setDuration(pPulse->getDuration());
        m_pPulseShapes[getNumberOfPulses()] = pPulse;
        return  true;
  };

/*****************************************************************************/
PulseShape* AtomicSequence::FindPulse(string sPulseName) {
	for (int i=0;i<getNumberOfPulses();++i)
		if (getPulse(i)->getName()==sPulseName) return getPulse(i);
	return NULL;
}

/*****************************************************************************/
  bool AtomicSequence::Prepare(bool verbose){
	bool btag=true;
        m_dDuration=0.0;
	for (int i=0;i<getNumberOfPulses();++i)
	{
		btag = (btag && getPulse(i)->Prepare(verbose));
        	setDuration(getPulse(i)->getDuration());
	}
	return btag;
  };

/*****************************************************************************/
  void AtomicSequence::Destroy() //destructor call: delete all pulses, then myself
  {
	for (int i=0;i<getNumberOfPulses() ;++i)
		delete getPulse(i);
	delete this;
  }

/*****************************************************************************/
void AtomicSequence::writeADCs(ofstream* pfout){
	int iNRO=0;
	int iNP=getNumberOfPulses();
	double dT= getDuration();
	double*      pdADCs;
	int iSeqFlag = 0; //I am an atomic sequence

	*pfout << "\t<atom duration=\"" << dT << "\" pulses=\"" << iNP << "\"> " << endl;
	for (int j=0;j<iNP;j++)
	{	
		iNRO = getPulse(j)->getNumOfADCs();
		//pfout->write((char *)(&iNRO), sizeof(iNRO));
		*pfout << "\t\t<pulse> " ;
		if (iNRO>0)
		{
			pdADCs=getPulse(j)->getADCarray();
			for (int i=0;i<iNRO;i++)//{pfout->write((char *)(pdADCs+i), sizeof(pdADCs[i])); }
				*pfout << pdADCs[i] << " ";
		}
		*pfout << "</pulse> " << endl;
	}
	*pfout << "\t</atom> " << endl;
};

/*****************************************************************************/
void AtomicSequence::setListOfTimepoints(bool force){

	if (force) m_bDoSetListOfTPs=true;
	if (!m_bDoSetListOfTPs) return;   //list already set -> nothing to do!

	int iN=0, iNtotal=0;
	double* pdArray;

	//collect NLPs and ADCs from PulseShapes
	for (int j=0;j<getNumberOfPulses();j++)
	{	
		iN = getPulse(j)->getNumOfADCs();
		pdArray=getPulse(j)->getADCarray();
		for (int i=0;i<iN;i++){ m_dTPs[i+iNtotal]=pdArray[i]; m_bTPs[i+iNtotal]=true; }
		iNtotal += iN;

		iN = getPulse(j)->getNumOfNLPs();
		pdArray=getPulse(j)->getNLParray();
		for (int i=0;i<iN;i++){ m_dTPs[i+iNtotal]=pdArray[i]; m_bTPs[i+iNtotal]=false; }
		if (getPulse(j)->getDuration() < getDuration())
		{ m_dTPs[iN+iNtotal]=getPulse(j)->getDuration()+TIME_ERR_TOL; m_bTPs[iN+iNtotal]=false; iN++; }
		iNtotal += iN;
	}
	
	//sort time points
	for(int iUp=iNtotal-1;iUp>0;--iUp)
		for(int i=0; i<iUp; ++i)
			if(m_dTPs[i]>m_dTPs[i+1])
			{
				double d=m_dTPs[i]; m_dTPs[i]=m_dTPs[i+1]; m_dTPs[i+1]=d;
				bool   b=m_bTPs[i]; m_bTPs[i]=m_bTPs[i+1]; m_bTPs[i+1]=b;
			}

	//remove multiple timepoints (according to TIME_ERR_TOL)
	m_iTPs=0; bool same=false, badc=false;
	for(int i=0; i<iNtotal; ++i)
		if( fabs(m_dTPs[i]-m_dTPs[i+1])>TIME_ERR_TOL)
		{
			m_dTPs[m_iTPs] = m_dTPs[i];
			m_bTPs[m_iTPs] = (same?badc:m_bTPs[i]);
			++m_iTPs;
			same=false;
			badc=false;
		} //remember multiples, to correctly set the ADC bits
		else
		{
			same=true;
			badc=(badc || m_bTPs[i] || m_bTPs[i+1]);
		}

	//increase zero 
	if(m_dTPs[0]==0) { m_dTPs[0]=TIME_ERR_TOL; } 

	m_bDoSetListOfTPs=false;
};

/******************************************************************/
void AtomicSequence::writeSeqVal(double& dTimeShift,ofstream* pfout){
	setListOfTimepoints(true);
	for (int i=0; i<m_iTPs; ++i)
	{
		//write all as double for faster reading from MATLAB
		double dVal[5]={0.0,0.0,0.0,0.0,0.0};
		double dTime=m_dTPs[i]+dTimeShift;
		double dADC=(m_bTPs[i]?1.0:-1.0);
		getValue(m_dTPs[i],&dVal[0]);
		(*pfout).write((char *)(&(dTime)), sizeof(double));
		(*pfout).write((char *)(&(dVal[0])), sizeof(double));
		(*pfout).write((char *)(&(dVal[1])), sizeof(double));
		(*pfout).write((char *)(&(dVal[2])), sizeof(double));
		(*pfout).write((char *)(&(dVal[3])), sizeof(double));
		(*pfout).write((char *)(&(dVal[4])), sizeof(double));
		(*pfout).write((char *)(&(dADC)), sizeof(double));
	}
	dTimeShift += getDuration();
};
