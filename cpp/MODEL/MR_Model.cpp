/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include "MR_Model.h"

//set the current spin of the sample
void MR_Model::setSpin(int k){
	initSolver();
	double dx=m_pSam->sSample.Xcoord[k];
	double dy=m_pSam->sSample.Ycoord[k];
	double dz=m_pSam->sSample.Zcoord[k];
	m_pSpin->M0 = m_pSam->sSample.M0[k];
	m_pSpin->R1 = 1.0/m_pSam->sSample.RT1[k];
	m_pSpin->R2 = 1.0/m_pSam->sSample.RT2[k];
	m_pSpin->r[0] = dx;
	m_pSpin->r[1] = dy;
	m_pSpin->r[2] = dz;

	m_pSpin->B0overGMAX = m_B0overGMAX;
	m_pSpin->deltaB = getDeltaB(dx,dy,dz);
	m_pSpin->CHI = m_SusceptFact * m_pSam->sSample.CHI[k];
	m_pSpin->CS = m_ChemShiftFact * m_pSam->sSample.CS[k];
	m_pSpin->deltaB += (m_pSpin->CHI + m_pSpin->CS); //TMPTMP !!!

	m_pSam->sSample.Mxy[k]  = 0.0;
	m_pSam->sSample.Mphi[k] = 0.0;
	m_pSam->sSample.Mz[k]   = m_pSpin->M0;

	m_MagCylCord[0] = 0.0;		//  equilibrium 
	m_MagCylCord[1] = 0.0;		//  magnetisation
	m_MagCylCord[2] = m_pSpin->M0; 	//  vector 
};

/****************************************************************/
void MR_Model::useSpin(int k){
	initSolver();
	double dx=m_pSam->sSample.Xcoord[k];
	double dy=m_pSam->sSample.Ycoord[k];
	double dz=m_pSam->sSample.Zcoord[k];
	m_pSpin->M0 = m_pSam->sSample.M0[k];
	m_pSpin->R1 = 1.0/m_pSam->sSample.RT1[k];
	m_pSpin->R2 = 1.0/m_pSam->sSample.RT2[k];
	m_pSpin->r[0] = dx;
	m_pSpin->r[1] = dy;
	m_pSpin->r[2] = dz;

	m_pSpin->B0overGMAX = m_B0overGMAX;
	m_pSpin->deltaB = getDeltaB(dx,dy,dz);
	m_pSpin->CHI = m_SusceptFact * m_pSam->sSample.CHI[k];
	m_pSpin->CS = m_ChemShiftFact * m_pSam->sSample.CS[k];
	m_pSpin->deltaB += (m_pSpin->CHI + m_pSpin->CS); //TMPTMP !!!

	m_MagCylCord[0] = m_pSam->sSample.Mxy[k]; // sqrt(pow(m_pSam->sSample.Mx[k],2)+pow(m_pSam->sSample.My[k],2));
	m_MagCylCord[1] = m_pSam->sSample.Mphi[k]; // atan(m_pSam->sSample.My[k]/m_pSam->sSample.Mx[k]);
	m_MagCylCord[2] = m_pSam->sSample.Mz[k]; //
};
/****************************************************************/
void MR_Model::setFieldTerm(int iTerm, double dFactor, double dRadius, double dX0, double dY0, double dZ0){
	int i=m_iNumOfDbTerms;
	m_iTerm[i]=iTerm;
	m_dFactor[i]=dFactor;
	m_dRadius[i]=dRadius;
	m_dX0[i]=dX0; m_dY0[i]=dY0; m_dZ0[i]=dZ0;
	m_iNumOfDbTerms++;
};

/****************************************************************/
void MR_Model:: setNonLinG(double* qt, double* ct){
 m_pSpin->bNonLinG=true;
 for (int i=0;i<3;++i) {m_pSpin->qt[i]=qt[i]; m_pSpin->ct[i]=ct[i];} 
};

/****************************************************************/
double MR_Model:: getDeltaB(double dx,double dy, double dz){
	double dB=0.0;
	//random part
        extern float gasdev(long *idum);
	dB += m_dNoiseLevel*gasdev(&m_i_rand_init);
	//inhomogeneity terms (up to second order)
	if (m_iNumOfDbTerms>0)
	{
		double dX=0.0,dY=0.0,dZ=0.0;
		double* dim = m_pSam->getSampleDim();
		double d1=(dim[1]-dim[0]), d2=(dim[3]-dim[2]), d3=(dim[5]-dim[4]);
		for (int i=0; i<m_iNumOfDbTerms; ++i)
		{
			if (d1>0.0) dX=(dx-m_dX0[i])/d1;
			if (d2>0.0) dY=(dy-m_dY0[i])/d2;
			if (d3>0.0) dZ=(dz-m_dZ0[i])/d3;
			double r[10]={1.0,dX,dY,dZ,dX*dX,dY*dY,dZ*dZ,dX*dY,dX*dZ,dY*dZ};
			if ( pow(dx-m_dX0[i],2)+pow(dy-m_dY0[i],2)+pow(dz-m_dZ0[i],2)
			     < pow(m_dRadius[i],2) || m_dRadius[i]<0.0)
				dB += m_dFactor[i]*r[m_iTerm[i]];
		}
	}
	return dB;
};
/****************************************************************/
Signal*  MR_Model::Solve(bool cont, bool remember, string* fn, int iStep) {

   ofstream* pfout=NULL;
   if (fn!=NULL) { pfout = new ofstream(fn->c_str(), ios::binary); }

   long lIndexShift;
   double dTimeShift;
   for (long k=0;k<m_pSam->sSample.NumberOfPoints;k++)
   {
//m_bTMP = (k==1000);
	if (cont)	{ useSpin(k); lIndexShift=m_lIndexShift; dTimeShift=m_dTimeShift; }
	else		{ setSpin(k); lIndexShift=0; dTimeShift=0;  m_dTimeShift=0; }
	if (!cont && !remember) { freeSolver(); continue; } //if neither cont nor remember mode, do not calculate
	//recursively travel the sequence and solve the MR model
	TravelSequence(dTimeShift, lIndexShift, m_pSeq, k, pfout, iStep);
	m_pSam->sSample.Mm[k]  = m_MagCylCord[0]; // always
	m_pSam->sSample.Mp[k]  = m_MagCylCord[1]; //  remember
	m_pSam->sSample.Ml[k]  = m_MagCylCord[2]; //   magnetisation
	//remember final magnetisation state of this spin and number of ADCs-acquired and duration-past so far
	if (remember)
	{
		m_pSam->sSample.Mxy[k]  = m_MagCylCord[0];
		m_pSam->sSample.Mphi[k] = m_MagCylCord[1];
		m_pSam->sSample.Mz[k]   = m_MagCylCord[2];
    		m_lIndexShift=lIndexShift;
		m_dTimeShift=dTimeShift; 
	}
	freeSolver();
   }

   if (fn!=NULL)
   {
	double dNSP = ((double) m_pSam->sSample.NumberOfPoints);
	pfout->write((char *) &dNSP, sizeof dNSP );
	pfout->close();
	delete pfout;
   }
   return m_pSig;
};

void  MR_Model:: TravelSequence( double& dTimeShift, long& lIndexShift, Sequence* pSeq , long& lSpin, ofstream* pfout, int iStep) {
	
	int iNnumberOfChildren=pSeq->getNumberOfChildren();
	if (iNnumberOfChildren>0) //ConcatSequence
	{
		if (pSeq==m_pSeq) {if( !pSeq->Prepare(false) ) return;} //if root, prep sequence
		for (int i=0; i<((ConcatSequence *)pSeq)->getRepetitions(); ++i)
		{
			((ConcatSequence *)pSeq)->setLoop(i);
			for (int j=0; j<iNnumberOfChildren; ++j)
				TravelSequence(dTimeShift, lIndexShift, pSeq->getChild(j) , lSpin, pfout, iStep);
		}
	}
	else //AtomicSequence
	{
		MRCalculate(dTimeShift, lIndexShift, (AtomicSequence*) pSeq, lSpin, pfout, iStep);
	}
	//finally, the sequence top-node writes the total number of ADCs to the signal
	if (pSeq==m_pSeq) m_pSig->sSignal.NumberOfPoints = lIndexShift;
};

//write ADC to the signal 
void MR_Model::AddToSignal(int n,double dTadc,AtomicSequence* pA){
	double dLockedPhase=0.0;
	double dVol=m_pSam->sSample.NumberOfPoints;
	//is there a phase-locking RfPulseShape?
	for (int i=0; i<pA->getNumberOfPulses() ; ++i)
		if (pA->getPulse(i)->getAxis()==AXIS_RF)
			if ( ((RfPulseShape*) pA->getPulse(i))->getPhaseLock() )
			{
				dLockedPhase = -((RfPulseShape*) pA->getPulse(i))->getPhase()*PI/180.0; 
				break;//only one phaselock can be considered
			}
	m_pSig->sSignal.t[n] = dTadc;
	m_pSig->sSignal.Mx[n]+=m_MagCylCord[0] * cos(dLockedPhase+m_MagCylCord[1])/dVol;
	m_pSig->sSignal.My[n]+=m_MagCylCord[0] * sin(dLockedPhase+m_MagCylCord[1])/dVol;
	m_pSig->sSignal.Mz[n]+=m_MagCylCord[2]/dVol ;
};

//***********************************************************
void MR_Model::SaveEvolution(ofstream* pfout, long lSpin, double time) {
	double	dM1=m_MagCylCord[0] * cos( m_MagCylCord[1] ),
		dM2=m_MagCylCord[0] * sin( m_MagCylCord[1] ),
		dM3=m_MagCylCord[2] , dX = m_pSam->sSample.Xcoord[lSpin],
		dY = m_pSam->sSample.Ycoord[lSpin], dZ = m_pSam->sSample.Zcoord[lSpin];
	pfout->write((char *) &time, sizeof time );
	pfout->write((char *) &dX, sizeof dX );
	pfout->write((char *) &dY, sizeof dY );
	pfout->write((char *) &dZ, sizeof dZ );
	pfout->write((char *) &dM1, sizeof dM1 );
	pfout->write((char *) &dM2, sizeof dM2 );
	pfout->write((char *) &dM3, sizeof dM3 );
};


/*****************************************************
 functions for random number generation (from NUMRREP)
 *****************************************************/
#define RAND_IA 16807
#define RAND_IM 2147483647
#define RAND_AM (1.0/RAND_IM)
#define RAND_IQ 127773
#define RAND_IR 2836
#define RAND_NTAB 32
#define RAND_NDIV (1+(RAND_IM-1)/RAND_NTAB)
#define RAND_EPS 1.2e-7
#define RAND_RNMX (1.0-RAND_EPS)

float ran1(long *idum) {
	int j;
	long k;
	static long iy=0;
	static long iv[RAND_NTAB];
	float temp;

	if (*idum <= 0 || !iy) {
		if (-(*idum) < 1) *idum=1;
		else *idum = -(*idum);
		for (j=RAND_NTAB+7;j>=0;j--) {
			k=(*idum)/RAND_IQ;
			*idum=RAND_IA*(*idum-k*RAND_IQ)-RAND_IR*k;
			if (*idum < 0) *idum += RAND_IM;
			if (j < RAND_NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}
	k=(*idum)/RAND_IQ;
	*idum=RAND_IA*(*idum-k*RAND_IQ)-RAND_IR*k;
	if (*idum < 0) *idum += RAND_IM;
	j=iy/RAND_NDIV;
	iy=iv[j];
	iv[j] = *idum;
	if ((temp=RAND_AM*iy) > RAND_RNMX) return RAND_RNMX;
	else return temp;
};

#undef RAND_IA
#undef RAND_IM
#undef RAND_AM
#undef RAND_IQ
#undef RAND_IR
#undef RAND_NTAB
#undef RAND_NDIV
#undef RAND_EPS
#undef RAND_RNMX


float gasdev(long *idum) {
	float ran1(long *idum);
	static int iset=0;
	static float gset;
	float fac,rsq,v1,v2;

	if (*idum < 0) iset=0;
	if  (iset == 0) {
		do {
			v1=2.0*ran1(idum)-1.0;
			v2=2.0*ran1(idum)-1.0;
			rsq=v1*v1+v2*v2;
		} while (rsq >= 1.0 || rsq == 0.0);
		fac=sqrt(-2.0*log(rsq)/rsq);
		gset=v1*fac;
		iset=1;
		return v2*fac;
	} else {
		iset=0;
		return gset;
	}
};

