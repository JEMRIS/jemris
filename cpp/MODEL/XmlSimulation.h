/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _XmlSimulation_H_
#define _XmlSimulation_H_

#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
XERCES_CPP_NAMESPACE_USE

#include "BCV_MR_Model.h"		//MR Simulation (numerical solution of Bloch equations)
#include "Sphere2D.h"		//MR Sample (homogeneous sphere)
#include "Sphere2D_2sub.h"		//MR Sample (homogeneous sphere)
#include "Sequence.h"

class XmlSimulation {
 
public:
 //constructor parses xml sequence file to a new DOM
 XmlSimulation(string xmlfile, Sequence* pSeq){
	XMLPlatformUtils::Initialize();
	m_pXDP = new XercesDOMParser();
	m_pSeq = pSeq;
	m_bOK=ifstream(xmlfile.c_str());
	if (m_bOK)
	{
		try { m_pXDP->parse( xmlfile.c_str() ); }
		catch (const XMLException& toCatch) { m_bOK=false; return; }
		m_nSim= (DOMNode*)m_pXDP->getDocument();
		m_nSim= m_nSim -> getFirstChild();
		if (m_nSim==NULL) {m_bOK=false; return;} 
		string name = XMLString::transcode(m_nSim->getNodeName()) ;
		m_bOK = (name=="JMRI-SIM");
	}
	m_iEvol=0;
 };

 //decon deletes DOM and terminates xerces-c 
 ~XmlSimulation(){ delete m_pXDP; XMLPlatformUtils::Terminate(); };

 bool Status(){return m_bOK; };

 //public functions to parse sample and model objects
 Sample* getSample(bool verbose = true);
 void setSample(Sample* val ){m_pSam=val;};
 MR_Model* getModel(bool verbose = true);

 int getEvolution(){return m_iEvol;};

private:
	XercesDOMParser* m_pXDP;
	DOMNode* m_nSim;
	bool m_bOK;
	int m_iEvol;
	Sequence* m_pSeq;
	Sample* m_pSam;
};
#endif
