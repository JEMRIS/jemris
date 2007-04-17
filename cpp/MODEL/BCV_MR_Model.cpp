/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include "BCV_MR_Model.h"

/****************************************************************/
void  BCV_MR_Model:: MRCalculate( double& dTimeShift, long& lIndexShift, AtomicSequence* pA , long& lSpin, ofstream* pfout, int iStep)
{
	Ith( ((nvec*) (m_pSpin->SolverSettings))->y,1 ) = m_MagCylCord[0] ;
	Ith( ((nvec*) (m_pSpin->SolverSettings))->y,2 ) = m_MagCylCord[1] ;
	Ith( ((nvec*) (m_pSpin->SolverSettings))->y,3 ) = m_MagCylCord[2] ;
	extern void * BlochMalloc( Spin* pSpin,real &reltol,long int iopt[],real ropt[]);
	real tpoint;
	int i,iNadc,iNa;
	pA->setListOfTimepoints(false);
	double* dTP=pA->getdTPs(); //time points of interest (ADCs&NLPs) in an atomic sequence
	bool* bTP=pA->getbTPs();   //corresponding ADC bits: if true, result is added to signal
	m_pSpin->pASeq = pA;
	void* cvode_mem = BlochMalloc(m_pSpin,m_reltol,m_iopt,m_ropt);
	if (cvode_mem == NULL) { printf("CVodeMalloc failed.\n"); return; }
		
	for (i=0; i<pA->getNumOfTPs() ; ++i)
	{
			CVode(cvode_mem, dTP[i] , ((nvec*) (m_pSpin->SolverSettings))->y, &tpoint, NORMAL);
			m_MagCylCord[0]=Ith( ((nvec*) (m_pSpin->SolverSettings))->y,1 );
			m_MagCylCord[1]=Ith( ((nvec*) (m_pSpin->SolverSettings))->y,2 );
			m_MagCylCord[2]=Ith( ((nvec*) (m_pSpin->SolverSettings))->y,3 );
			if (pfout != NULL && bTP[i] && lIndexShift%iStep == 0) //write time evolution
				SaveEvolution( pfout, lSpin, dTimeShift + dTP[i] );
			if (bTP[i]) //write signal
				AddToSignal(lIndexShift++,dTimeShift+dTP[i],pA);
	}
	dTimeShift += pA->getDuration();
	N_VFree( ((nvec*) (m_pSpin->SolverSettings))->abstol);  //free the abstol vector
	CVodeFree(cvode_mem);
};

/* Print some final statistics located in the iopt array */
void BCV_MR_Model::PrintFinalStats() {
  printf("\nFinal Statistics.. \n\n");
  printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n",
         m_iopt[NST], m_iopt[NFE], m_iopt[NSETUPS]);
  printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n",
         m_iopt[NNI], m_iopt[NCFN], m_iopt[NETF]);
};


