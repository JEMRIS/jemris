/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _XML2SEQ_H_
#define _XML2SEQ_H_

#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
XERCES_CPP_NAMESPACE_USE

#include "AllSequences.h"
#include "AllPulses.h"


class XmlSequence {
 
public:
 //constructor parses xml sequence file to a new DOM
 XmlSequence(string xmlfile){
	XMLPlatformUtils::Initialize();
	m_pXDP = new XercesDOMParser();
	m_bOK=ifstream(xmlfile.c_str());
	if (m_bOK)
	{
		try { m_pXDP->parse( xmlfile.c_str() ); }
		catch (const XMLException& toCatch) { m_bOK=false; return; }
		m_nSeq = (DOMNode*)m_pXDP->getDocument();
		m_nSeq = m_nSeq -> getFirstChild();
		if (m_nSeq==NULL) {m_bOK=false; return;} 
		string name = XMLString::transcode(m_nSeq->getNodeName()) ;
		m_bOK = (name=="ConcatSequence");
	}
 };

 //decon deletes DOM and terminates xerces-c 
 ~XmlSequence(){ delete m_pXDP; XMLPlatformUtils::Terminate(); };

 bool Status(){return m_bOK; };

 //public function to get the sequence object
 ConcatSequence* getSequence(bool verbose = true);

 
protected:

 DOMNode* getRoot(){ return m_nSeq; };

//basic function: transforms DOM sequence to new created Sequence object
 Sequence* Transform(DOMNode* node);

 //every sequence class needs to have a create function
 void CreateConcatSequence(Sequence** pSeq, DOMNode* node);
 void CreateAtomicSequence(Sequence** pSeq, DOMNode* node);
 void CreateDelayAtom(Sequence** pSeq, DOMNode* node);
 void CreateGradientSpiralExtRfConcatSequence(Sequence** pSeq, DOMNode* node);

 //every pulseshape class needs to have a create function
 void CreatePulseShape(PulseShape** pPulse, int* iTreeSteps, DOMNode* node);
 void CreateEmptyPulse(PulseShape** pPulse, DOMNode* node);
 void CreateExternalPulseShape(PulseShape** pPulse, DOMNode* node);
 void CreateSincRfPulseShape(PulseShape** pPulse, DOMNode* node);
 void CreateHardRfPulseShape(PulseShape** pPulse, DOMNode* node);
 void CreateRfReceiverPhase(PulseShape** pPulse, DOMNode* node);
 void CreateRfPhaseCycling(PulseShape** pPulse, DOMNode* node);
 void CreateRfSpoiling(PulseShape** pPulse, DOMNode* node);
 void CreateTGPS(PulseShape** pPulse, DOMNode* node);
 void CreateRO_TGPS(PulseShape** pPulse, DOMNode* node);
 void CreatePE_TGPS(PulseShape** pPulse, DOMNode* node);
 void CreateGradientSpiral(PulseShape** pPulse, DOMNode* node);

private:
	XercesDOMParser* m_pXDP;
	DOMNode* m_nSeq;	//the sequence represented as a DOM (given by the top node)
	bool m_bOK;
};


#endif
