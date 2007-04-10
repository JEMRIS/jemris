/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/


#include <iostream>
using namespace std;

#include "Sample.h"

void  Sample::CreateSamplestruct(long NPoints)
{
  sSample.NumberOfPoints = NPoints;
  sSample.M0 = new double[NPoints];
  sSample.RT1 = new double[NPoints];
  sSample.RT2 = new double[NPoints];
  sSample.CHI = new double[NPoints];
  sSample.CS  = new double[NPoints];
  sSample.Xcoord = new double[NPoints];
  sSample.Ycoord = new double[NPoints];
  sSample.Zcoord = new double[NPoints];
  sSample.Mxy  = new double[NPoints];
  sSample.Mphi = new double[NPoints];
  sSample.Mz   = new double[NPoints];
  sSample.Mm   = new double[NPoints];
  sSample.Mp   = new double[NPoints];
  sSample.Ml   = new double[NPoints];
  for (long i=0;i<NPoints; i++)
  {
         sSample.M0[i] = 0;    sSample.RT1[i] = 0;    sSample.RT2[i] = 0;
         sSample.CS[i] = 0;    sSample.CHI[i] = 0;
     sSample.Xcoord[i] = 0; sSample.Ycoord[i] = 0;  sSample.Zcoord[i] = 0;
        sSample.Mxy[i] = 0;   sSample.Mphi[i] = 0;	sSample.Mz[i] = 0;
	 sSample.Mm[i] = 0;	sSample.Mp[i] = 0;	sSample.Ml[i] = 0;
  }
};



 Sample * Sample::getSubSample(long n,int size) //size : 1..SUMsize-1  No.n node 
{
   long N=sSample.NumberOfPoints;
   if (n >size || size>N) return NULL;
   
   long k = N/size;
   long l = N%size;
   long  ibeg = (n-1)*k; 
   long iend = n*k-1 + (n<size?0:l); 

   Sample *pSubSample;
   pSubSample = new Sample(iend-ibeg+1);
   for (long i=ibeg ,u=0;i<=iend;i++,u++)
   {
         pSubSample->sSample.M0[u]=sSample.M0[i];
         pSubSample->sSample.RT1[u]=sSample.RT1[i];
         pSubSample->sSample.RT2[u]=sSample.RT2[i];
         pSubSample->sSample.CHI[u]=sSample.CHI[i];
         pSubSample->sSample.CS[u]=sSample.CS[i];
         pSubSample->sSample.Xcoord[u]=sSample.Xcoord[i];
         pSubSample->sSample.Ycoord[u]=sSample.Ycoord[i];
         pSubSample->sSample.Zcoord[u]=sSample.Zcoord[i];
   }
   return (pSubSample);
};

void Sample::writeBinaryFile(string filename)
{
	ofstream fout(filename.c_str(), ios::binary);
        for (long v=0;v<sSample.NumberOfPoints;v++)
        {
		fout.write((char *)(&(sSample.M0[v])), sizeof(sSample.M0[v]));
		fout.write((char *)(&(sSample.RT1[v])), sizeof(sSample.RT1[v]));
		fout.write((char *)(&(sSample.RT2[v])), sizeof(sSample.RT2[v]));
		fout.write((char *)(&(sSample.CHI[v])), sizeof(sSample.CHI[v]));
		fout.write((char *)(&(sSample.CS[v])), sizeof(sSample.CS[v]));
		fout.write((char *)(&(sSample.Xcoord[v])), sizeof(sSample.Xcoord[v]));
		fout.write((char *)(&(sSample.Ycoord[v])), sizeof(sSample.Ycoord[v]));
		fout.write((char *)(&(sSample.Zcoord[v])), sizeof(sSample.Zcoord[v]));
	}
	fout.close();
};

void Sample::readBinaryFile(string filename)
{
	long N;
	ifstream fin(filename.c_str(), ios::binary);
	fin.read((char *)(&(N)), sizeof(long));
	CreateSamplestruct(N);

        for (long v=0;v<sSample.NumberOfPoints;v++)
        {
		fin.read((char *)(&(sSample.M0[v])), sizeof(double));
		fin.read((char *)(&(sSample.RT1[v])), sizeof(double));
		fin.read((char *)(&(sSample.RT2[v])), sizeof(double));
		fin.read((char *)(&(sSample.CHI[v])), sizeof(double));
		fin.read((char *)(&(sSample.CS[v])), sizeof(double));
		fin.read((char *)(&(sSample.Xcoord[v])), sizeof(double));
		fin.read((char *)(&(sSample.Ycoord[v])), sizeof(double));
		fin.read((char *)(&(sSample.Zcoord[v])), sizeof(double));
	}
	fin.close();
};

 double* Sample::getSampleDim(){
	for (int i=0;i<6; ++i) {m_dSampleDim[i]=0.0;};
	for (int i=0;i<sSample.NumberOfPoints; ++i)
	{
		m_dSampleDim[0]=(m_dSampleDim[0]<sSample.Xcoord[i])?m_dSampleDim[0]:sSample.Xcoord[i];	
		m_dSampleDim[1]=(m_dSampleDim[1]>sSample.Xcoord[i])?m_dSampleDim[1]:sSample.Xcoord[i];	
		m_dSampleDim[2]=(m_dSampleDim[2]<sSample.Ycoord[i])?m_dSampleDim[2]:sSample.Ycoord[i];	
		m_dSampleDim[3]=(m_dSampleDim[3]>sSample.Ycoord[i])?m_dSampleDim[3]:sSample.Ycoord[i];	
		m_dSampleDim[4]=(m_dSampleDim[4]<sSample.Zcoord[i])?m_dSampleDim[4]:sSample.Zcoord[i];	
		m_dSampleDim[5]=(m_dSampleDim[5]>sSample.Zcoord[i])?m_dSampleDim[5]:sSample.Zcoord[i];	
	}
	return &m_dSampleDim[0];
};
