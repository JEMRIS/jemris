/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include "XmlSimulation.h"

/*****************************************************************************/
 Sample* XmlSimulation::getSample(bool verbose){

	DOMNode* child ;
	DOMNamedNodeMap *pAttributes;
	string name,item,value,shape,samplefile="sample.bin";
	Sample* pSam = NULL;
	double dr=1,dR=100,M0=1,T1=100,T2=50,CS=0;
	double dR_2=50,M0_2=0.5,T1_2=50,T2_2=25,CS_2=0;
	bool createSphere = false;
	bool createSphere_2sub = false;

	for (child = m_nSim->getFirstChild(); child != 0; child=child->getNextSibling())
	{
		name = XMLString::transcode(child->getNodeName()) ;
		pAttributes = child->getAttributes();
		if (name=="Sample" && pAttributes)
		{
			int nSize = pAttributes->getLength();
			for(int i=0;i<nSize;++i)
			{
				DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
				item = XMLString::transcode(pAttributeNode->getName());
				value = XMLString::transcode(pAttributeNode->getValue());
				if (item=="InFile" && ifstream(value.c_str())) pSam = new Sample(value);
				if (item=="InFile")      shape=value;
				if (item=="OutFile")	samplefile = value;
				if (item=="Delta")	dr = atof(value.c_str());
				if (item=="Radius")	dR = atof(value.c_str());
				if (item=="M0")		M0 = atof(value.c_str());
				if (item=="T1")		T1 = atof(value.c_str());
				if (item=="T2")		T2 = atof(value.c_str());
				if (item=="CS")		CS = atof(value.c_str());
				if (item=="Radius_1")	dR = atof(value.c_str());
				if (item=="M0_1")	M0 = atof(value.c_str());
				if (item=="T1_1")	T1 = atof(value.c_str());
				if (item=="T2_1")	T2 = atof(value.c_str());
				if (item=="CS_1")	CS = atof(value.c_str());
				if (item=="Radius_2")	dR_2 = atof(value.c_str());
				if (item=="M0_2")	M0_2 = atof(value.c_str());
				if (item=="T1_2")	T1_2 = atof(value.c_str());
				if (item=="T2_2")	T2_2 = atof(value.c_str());
				if (item=="CS_2")	CS_2 = atof(value.c_str());
				if (item=="Shape")      shape=value;
				if (item=="Shape" && value=="Sphere2D")	createSphere = true;
				if (item=="Shape" && value=="2Spheres2D") createSphere_2sub = true;
			}
			if (createSphere) pSam = new Sphere2D(dr,dr,dR,M0,T1,T2,CS);
			if (createSphere_2sub) pSam = new Sphere2D_2sub(dr,dr,dR,M0,T1,T2,CS,dR_2,M0_2,T1_2,T2_2,CS_2);
		}
	}
	m_pSam = pSam;
	if (verbose)
	{
		pSam->writeBinaryFile(samplefile);
		double* sdim=pSam->getSampleDim();
		cout	<< endl << "Sample: '"<< shape << "'" << endl
				<< "              number of spins: " << m_pSam->sSample.NumberOfPoints << endl
				<< "              x,y,z-dimensions: [ (" << sdim[0] << "," << sdim[1]
				<< ") ; (" << sdim[2] << "," << sdim[3] << ") ; (" << sdim[4] << "," << sdim[5] << ") ] " << endl; 
	}
	return pSam;
 };

/*****************************************************************************/
 MR_Model* XmlSimulation::getModel(bool verbose){

	DOMNode* child ;
	DOMNamedNodeMap *pAttributes;
	string name,item,value;
	MR_Model* pMod = NULL;
	double dr=1.0,dR=100.0,M0=1.0,T1=100.0,T2=50.0,qt[3]={0.0,0.0,0.0},ct[3]={0.0,0.0,0.0};
	bool createSphere = false,bNonLinG=false;

	for (child = m_nSim->getFirstChild(); child != 0; child=child->getNextSibling())
	{
		name = XMLString::transcode(child->getNodeName()) ;
		pAttributes = child->getAttributes();
		if (name=="Model" && pAttributes)
		{
			pMod = new BCV_MR_Model(m_pSeq,m_pSam);
			int nSize = pAttributes->getLength();
			for(int i=0;i<nSize;++i)
			{
				DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
				item = XMLString::transcode(pAttributeNode->getName());
				value = XMLString::transcode(pAttributeNode->getValue());
				if (item=="FieldFluctuations")		pMod->setNoiseLevel( atof(value.c_str()) );
				if (item=="ConcomitantFields")		pMod->setB0overGMAX( atof(value.c_str()) );
				if (item=="SusceptibilityFactor")	pMod->setSusceptFact( atof(value.c_str()) );
				if (item=="ChemicalShiftFactor")	pMod->setChemShiftFact( atof(value.c_str()) );
				if (item=="FactorGX2")	{ bNonLinG=true; qt[0]=atof(value.c_str()) ; }
				if (item=="FactorGY2")	{ bNonLinG=true; qt[1]=atof(value.c_str()) ; }
				if (item=="FactorGZ2")	{ bNonLinG=true; qt[2]=atof(value.c_str()) ; }
				if (item=="FactorGX3")	{ bNonLinG=true; ct[0]=atof(value.c_str()) ; }
				if (item=="FactorGY3")	{ bNonLinG=true; ct[1]=atof(value.c_str()) ; }
				if (item=="FactorGZ3")	{ bNonLinG=true; ct[2]=atof(value.c_str()) ; }
				if (item=="SaveEvolution" )	m_iEvol = atoi(value.c_str());
			}
			if (bNonLinG) pMod->setNonLinG(&qt[0],&ct[0]);
		}
	}
	if (verbose && pMod != NULL)
	{
		cout << endl << "Model: numerical integration of Bloch equation for each spin" << endl;
	}

	return pMod;
 };
