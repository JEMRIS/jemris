/*
  This file is part of the MR simulation project
  Date: 03/2006
  Authors:  T. Stoecker, J. Dai
  MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#include "XmlSequence.h"

/*****************************************************************************/
ConcatSequence* XmlSequence::getSequence (bool verbose) {

    ConcatSequence* pSeq = (ConcatSequence*)Transform(getRoot());
    pSeq->Prepare(false);
    bool bstatus = pSeq->Prepare(true);

    //verbose output
    if (verbose) {
		
        pSeq->writeSeqDiagram("seq.bin");
		
        cout << endl << "Sequence  '"
             << pSeq->getName() << "' created " ;
        if (bstatus) 
            cout << " sucessfully ! " << endl;
        else        
            cout << " with errors/warings (see above) " << endl;
        cout << " - total sequence duration = " << pSeq->getDuration() 
             << " msec " << endl;
        cout << " - sequence diagram stored to seq.bin " << endl;
        cout << endl << "Dump of sequence tree: " << endl << endl;
		
        pSeq->getInfo(0);
		
    }
	
    return pSeq;
	
};

/*****************************************************************************/
Sequence* XmlSequence::Transform (DOMNode* node) {

    string name;
    Sequence *pSeq       = NULL;
    Sequence *pChildLast = NULL;
    Sequence *pChildNext = NULL;
	
    if (!node) return NULL;
	
    name = XMLString::transcode(node->getNodeName()) ;
    if (name == "ConcatSequence") {
        CreateConcatSequence(&pSeq,node);
        DOMNode* child ;

        for (child=node->getFirstChild(); 
             child!=0; child=child->getNextSibling()) {

            pChildNext=Transform(child);
            if (pChildNext != NULL) {
                //cout << pSeq->getName() << ": inserting " 
                //     << pChildNext->getName();   
                //if (pChildLast!=NULL) cout << " after " 
                //     << pChildLast->getName();   
				
                ((ConcatSequence*)pSeq)->InsertChild( pChildLast, pChildNext );
                pChildLast = pChildNext;
				
                //cout << " successful!" << endl;
            }
        }
        return pSeq;
    }
	
    //add new seq types to this list and implement creator function below
    if (name == "AtomicSequence") { 
        CreateAtomicSequence(&pSeq,node);    
        return pSeq;}
    if (name == "DelayAtom") { 
        CreateDelayAtom(&pSeq,node);        
        return pSeq;}
    if (name == "GradientSpiralExtRfConcatSequence") {
        CreateGradientSpiralExtRfConcatSequence(&pSeq,node);
        return pSeq;}
	
    return NULL; //if the node name is not known
	
};

/******************************************************************************
  implementation of conversion for different sequence types: 
  new types need to be added accordingly
******************************************************************************/

/*****************************************************************************/
void XmlSequence::CreateConcatSequence (Sequence** pSeq, DOMNode* node) {

    string item; 
    string value;
    string name       = "ConcatSequence";
    int    reps       = 1;
    int    iTreeSteps = 0;
    double factor     = 1.0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0; i<nSize; ++i) {
            DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item  = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)             name = value;
            if (item == REPETITIONS)      reps = atoi(value.c_str() );
            if (item == CONNECT_TO_LOOP)  iTreeSteps = atoi(value.c_str());
            if (item == FACTOR)           factor = atof(value.c_str() );
        }
    }

    *pSeq = new ConcatSequence(name,reps);

    if (iTreeSteps>0) (*pSeq)->setTreeSteps(iTreeSteps);
    
    (*pSeq)->setFactor(factor);
    
    if (item == REPETITIONS && value == "Nx") {
        (*pSeq)->NewParam(true); ((ConcatSequence*)*pSeq)->getLoopMethod(1);}
    if (item == REPETITIONS && value == "Ny") {
        (*pSeq)->NewParam(true); ((ConcatSequence*)*pSeq)->getLoopMethod(2);}

    //get parameters for this ConcatSequence
    DOMNode* child ;
    for (child=node->getFirstChild(); child!=0; child=child->getNextSibling()) {
        name = XMLString::transcode(child->getNodeName()) ;
        pAttributes = child->getAttributes();
        if (name=="Parameter" && pAttributes) {
            int nSize = pAttributes->getLength();
            for (int i=0; i<nSize; ++i) {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                item = XMLString::transcode(pAttributeNode->getName());
                value = XMLString::transcode(pAttributeNode->getValue());
                if (item=="TR")
                    (*pSeq)->getParameter()->setTR(atof(value.c_str()));
                if (item=="TE")
                    (*pSeq)->getParameter()->setTE(atof(value.c_str()));
                if (item=="TI")
                    (*pSeq)->getParameter()->setTI(atof(value.c_str()));
                if (item=="TD")
                    (*pSeq)->getParameter()->setTD(atof(value.c_str()));
                if (item=="Nx")
                    (*pSeq)->getParameter()->setNx(atoi(value.c_str()));
                if (item=="Ny")
                    (*pSeq)->getParameter()->setNy(atoi(value.c_str()));
                if (item=="Nz")
                    (*pSeq)->getParameter()->setNz(atoi(value.c_str()));
                if (item=="FOVx")
                    (*pSeq)->getParameter()->setFOVx(atof(value.c_str()));
                if (item=="FOVy")
                    (*pSeq)->getParameter()->setFOVy(atof(value.c_str()));
                if (item=="FOVz")
                    (*pSeq)->getParameter()->setFOVz(atof(value.c_str()));
                if (item=="ReadBW")
                    (*pSeq)->getParameter()->setReadBW(atof(value.c_str()));
                if (item=="Gmax")
                    (*pSeq)->getParameter()->setMaxAmpl(atof(value.c_str()));
                if (item=="SlewRate")
                    (*pSeq)->getParameter()->setSlewRate(atof(value.c_str()));
            }
        }
    }

 };

/*****************************************************************************/
void XmlSequence::CreateAtomicSequence (Sequence** pSeq, DOMNode* node) {

    string name,item,value;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0;i<nSize;++i) {
            DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)
                name = value;
        }
    }

    *pSeq = new AtomicSequence(name);

    //set pulses
    DOMNode* child ;
    for (child=node->getFirstChild(); child!=0; child=child->getNextSibling()) {
        PulseShape* pPulse = NULL;
        int iTreeSteps = 0;
        CreatePulseShape(&pPulse, &iTreeSteps, child);
        if (pPulse!=NULL)
            ((AtomicSequence*)*pSeq)->setPulse(pPulse,iTreeSteps);
    }

};

/*****************************************************************************/
void XmlSequence::CreateDelayAtom (Sequence** pSeq, DOMNode* node) {

    string S1         = "NULL";
    string S2         = "NULL";
    string S3         = "NULL";
    string name       = "DelayAtom";
    string item;
    string value;
    double delay;
    double factor     =-1.0;
    int    iNADC      = 0;
    DelayType DT;
    bool   bUseTE     = false;
    bool   bUseHalfTE = false;
    bool   bUseTR     = false;
    bool   bUseTI     = false;
    bool   bUseTD     = false;
    DOMNamedNodeMap *pAttributes = node->getAttributes();
    
    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0; i<nSize; ++i) {
            DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item  = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());

            if (item==NAME)   name   = value;

            //set factor for delay
            if (item==FACTOR)    factor = atof(value.c_str()); 

            if (item==DELAY) {
                delay = atof(value.c_str()); //set numeric delay
                if (delay==0.0) S3=value; //remember string
            }

            //use TE delay from parameter
            if (item==DELAY && value=="TE")   bUseTE=true ;

            //use TE delay from parameter
            if (item==DELAY && value=="TE/2") bUseHalfTE=true ;
            
            //use TR delay from parameter
            if (item==DELAY && value=="TR")   bUseTR=true ;
            
            //use TI delay from parameter
            if (item==DELAY && value=="TI")   bUseTI=true ;
            
            //use TD delay from parameter
            if (item==DELAY && value=="TD")   bUseTD=true ;

            if (item=="StartSeq")             S1= value;

            if (item=="StopSeq")              S2= value;

            if (item=="ADCs")                 iNADC = atoi(value.c_str());

            if (item=="DelayType") {
                if ( value== "B2E" ) DT = DELAY_B2E ;
                if ( value== "C2E" ) DT = DELAY_C2E ;
                if ( value== "B2C" ) DT = DELAY_B2C ;
                if ( value== "C2C" ) DT = DELAY_C2C ;
            }
        }
    }

    *pSeq = new DelayAtom(delay,NULL,NULL,DT,iNADC,name);
    ((DelayAtom*)*pSeq)->setStartStopSeq(S1,S2);
    if (bUseHalfTE) ((DelayAtom*)*pSeq)->useHalfTE(); 
    if (bUseTE) ((DelayAtom*)*pSeq)->useTE();
    if (bUseTR) ((DelayAtom*)*pSeq)->useTR();
    if (bUseTI) ((DelayAtom*)*pSeq)->useTI();
    if (bUseTD) ((DelayAtom*)*pSeq)->useTD();
    if (factor>0.0) ((DelayAtom*)*pSeq)->setFactor(factor);
    if (S3!="NULL") ((DelayAtom*)*pSeq)->setDelayFromPulse(S3);
};

/*****************************************************************************/
void XmlSequence::CreateGradientSpiralExtRfConcatSequence (Sequence** pSeq, 
                                                           DOMNode* node) {
    string filename;
    string name   = "GradientSpiralExtRfConcatSequence";
    string item;
    string value;
    double dturns = 32.0;
    double dtune  =  0.2;
    double dres   =  1.0;
    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0;i<nSize;++i) {
            DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)    name = value;
            if (item=="RfFileName")    filename = value;
            if (item=="Turns")    dturns=atof(value.c_str());
            if (item=="Tune")    dtune=atof(value.c_str());
            if (item=="Resolution")    dres=atof(value.c_str());
        }
    }

    *pSeq = new GradientSpiralExtRfConcatSequence (filename, 
                                                   dturns,dtune,dres,name);
 };

/*****************************************************************************
  implementation of conversion for different pulseshape types:
  new types need to be added accordingly
******************************************************************************/

/*****************************************************************************/
void XmlSequence::CreatePulseShape (PulseShape** pPulse, int* iTreeSteps, 
                                    DOMNode* node) {
    string name;
    string item;
    string value;
    int    iNADC = 0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0; i<nSize; ++i) {
            DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==CONNECT_TO_LOOP) *iTreeSteps = atoi(value.c_str());
            if (item=="ADCs")           iNADC = atoi(value.c_str());
        }
    }

    name = XMLString::transcode(node->getNodeName()) ;

    //add new pulse shapes to this list and implement creator function below
    if ( name == "EmptyPulse"         ) CreateEmptyPulse         (pPulse, node);
    if ( name == "ExternalPulseShape" ) CreateExternalPulseShape (pPulse, node);
    if ( name == "HardRfPulseShape"   ) CreateHardRfPulseShape   (pPulse, node);
    if ( name == "SincRfPulseShape"   ) CreateSincRfPulseShape   (pPulse, node);
    if ( name == "RfReceiverPhase"    ) CreateRfReceiverPhase    (pPulse, node);
    if ( name == "RfPhaseCycling"     ) CreateRfPhaseCycling     (pPulse, node);
    if ( name == "RfSpoiling"         ) CreateRfSpoiling         (pPulse, node);
    if ( name == "TGPS"               ) CreateTGPS               (pPulse, node);
    if ( name == "PE_TGPS"            ) CreatePE_TGPS            (pPulse, node);
    if ( name == "SS_TGPS"            ) CreateSS_TGPS            (pPulse, node);
    if ( name == "RO_TGPS"            ) CreateRO_TGPS            (pPulse, node);
    if ( name == "GradientSpiral"     ) CreateGradientSpiral     (pPulse, node);

    //add ADCs to pulse shape, if not already done
    if (*pPulse != NULL)
        if (iNADC>0 && (*pPulse)->getNumOfADCs() == 0)
            (*pPulse)->setNumOfADCs(iNADC);


};

/*****************************************************************************/
 void XmlSequence::CreateEmptyPulse (PulseShape** pPulse, DOMNode* node) {

    string name="EmptyPulse";
    string item;
    string value;
    double duration =0.0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0;i<nSize;++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)     name     = value;
            if (item == DURATION) duration = atof(value.c_str());
        }
    }

    *pPulse = new EmptyPulse(duration, 0, name);

 };

/*****************************************************************************/
void XmlSequence::CreateExternalPulseShape (PulseShape** pPulse, DOMNode* node) {
    
    string    name       = "ExternalPulseShape";
    string    filename;
    string    item,value;
    double    factor     = 1.0;

    PulseAxis eAxis      = AXIS_GX;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());

            if (item==NAME)      name     = value;
            if (item==FILE_NAME) filename = value;
            if (item==FACTOR)    factor   = atof(value.c_str());
            if (item=="Axis") {
                if ( value == "RF" ) eAxis = AXIS_RF ;
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
                if ( value == "GZ" ) eAxis = AXIS_GZ ;
            }
        }
    }

    *pPulse = new ExternalPulseShape(filename, eAxis, factor, name);

 };

/*****************************************************************************/
void XmlSequence::CreateHardRfPulseShape (PulseShape** pPulse, DOMNode* node) {
    
    string name          = "HardRfPulseShape";
    string item;
    string value;
    double duration      = 0.0;
    double phase         = 0.0;
    double flipangle     = 0.0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();
	DOMNodeList     *npAngles    = node->getChildNodes();


	if (npAngles) {
		DOMNode *npAngle        = npAngles->item(0);
		if (npAngle) {

			char*   cpAngle     = XMLString::transcode(npAngle->getNodeValue());
			string  spAngle     = cpAngle;
			string  buf;
			XMLString::release(&cpAngle);

			stringstream ss(spAngle);
			vector<double> angles;

			while (ss >> buf) {
				angles.push_back(atof(buf.c_str()));
			}

			vector<double>::iterator iter;
			for (iter = angles.begin(); iter != angles.end(); iter++) {}
		}
	}

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item  = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)       name      = value;
            if (item == DURATION)   duration  = atof(value.c_str());
            if (item == FLIP_ANGLE) flipangle = atof(value.c_str()); 
            if (item == PHASE)      phase     = atof(value.c_str());
        }
    }

    if (flipangle == 0.0) cout << name << " warning: zero flipangle" << endl;
    if (duration  == 0.0) cout << name << " warning: zero duration"  << endl;

    *pPulse = new HardRfPulseShape(flipangle, phase, duration, name);

 };

/*****************************************************************************/
void XmlSequence::CreateSincRfPulseShape (PulseShape** pPulse, DOMNode* node) {

    string name          = "SincRfPulseShape";
    string item;
    string value;
    double factor        =  0.5;
    double phase         =  0.0;
    double flipangle     = 90.0;
    double bw            =  0.5;
    double offset        =  0.0;
    double gap           =  0.0;
    int    N             =  3;

    SLICE_ORDER SO = ASCENDING;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item =  XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)       name      = value;
            if (item == FLIP_ANGLE) flipangle = atof(value.c_str()); 
            if (item == PHASE)      phase     = atof(value.c_str());
            if (item == BAND_WIDTH) bw        = atof(value.c_str());
            if (item == ZEROS)      N         = atoi(value.c_str());
            if (item == FACTOR)     factor    = atof(value.c_str());
            if (item == GAP)        gap       = atof(value.c_str());
            if (item == SLICEORDER) {
                if ( value == "ASCENDING" ) SO = ASCENDING ;
                if ( value == "DESCENDING" ) SO = DESCENDING ;
                if ( value == "INTERLEAVED" ) SO = INTERLEAVED ;
            }
        }
    }

    if (flipangle == 0.0 ) cout << name << " warning: zero flipangle" << endl;

    *pPulse = new SincRfPulseShape(flipangle, phase, bw, N, factor, SO, gap, 
                                   name);

 };

/*****************************************************************************/
void XmlSequence::CreateRfReceiverPhase (PulseShape** pPulse, DOMNode* node) {

    string name          = "RfReceiverPhase";
    string item;
    string value;
    double phase         = 0.0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)  name = value;
            if (item==PHASE) phase = atof(value.c_str());
        }
    }

    *pPulse = new RfReceiverPhase(phase, name);

 };

/*****************************************************************************/
void XmlSequence::CreateRfPhaseCycling (PulseShape** pPulse, DOMNode* node) {
     
    string name          = "RfPhaseCycling";
    string item;
    string value;
    double phases[128];
    double duration      =  0.0;
    int    cycle         =  1;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)     name = value;
            if (item==DURATION) duration = atof(value.c_str());
            if (item=="Cycle")  cycle = atoi(value.c_str());
            if (item=="Phase1") phases[0] = atof(value.c_str());
            if (item=="Phase2") phases[1] = atof(value.c_str());
            if (item=="Phase3") phases[2] = atof(value.c_str());
            if (item=="Phase4") phases[3] = atof(value.c_str());
        }
    }

    *pPulse = new RfPhaseCycling(&phases[0], cycle, duration, name);

 };

/*****************************************************************************/
void XmlSequence::CreateRfSpoiling (PulseShape** pPulse, DOMNode* node) {

    string name          = "RfSpoiling";
    string item;
    string value;
    double phase         =  0.0;
    double duration      =  0.0;
    int startcycle       =  0;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)           name       = value;
            if (item=="QuadPhaseInc") phase      = atof(value.c_str());
            if (item==DURATION)       duration   = atof(value.c_str());
            if (item=="StartCycle")   startcycle = atoi(value.c_str());
        }
    }

    *pPulse = new RfSpoiling(phase, duration, startcycle, name);

 };

/*****************************************************************************/
void XmlSequence::CreateTGPS (PulseShape** pPulse, DOMNode* node) {
    
    string    name          = "TGPS";
    string    item;
    string    value;
    string    pulse_name;
    double    area          =  0.0;
    double    factor        =  1.0;
    double    duration      =  0.0;
    double    slewrate      = -1.0;
    double    maxampl       = -1.0;
    double    asymsr        =  0.0;
    PulseAxis eAxis         = AXIS_GX;
    int       getareamethod =0;
    
    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item  = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());

            if (item == NAME)                  name       = value;
            if (item == AREA)                  pulse_name = value;
            if (item == AREA)                  area       = atof(value.c_str());
            if (item == "Gmax")                maxampl    = atof(value.c_str());
            if (item == "SlewRate")            slewrate   = atof(value.c_str());
            if (item == "AsymSR")              asymsr     = atof(value.c_str());
            if (item == FACTOR)                factor     = atof(value.c_str());
            if (item == DURATION)              duration   = atof(value.c_str());

            if (item == AREA && value == "KMAXx") getareamethod = 1;
            if (item == AREA && value == "KMAXy") getareamethod = 2;
            if (item == AREA && value == "KMAXz") getareamethod = 3;
            if (item == AREA && value == "DKx")   getareamethod = 4;
            if (item == AREA && value == "DKy")   getareamethod = 5;
            if (item == AREA && value == "DKz")   getareamethod = 6;
            if (item == "Axis") {
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
                if ( value == "GZ" ) eAxis = AXIS_GZ ;
            }
        }
    }
    *pPulse = new TGPS(area, eAxis, name);
    ((GradientPulseShape*)*pPulse)->setFactor(factor);
    if (maxampl  > 0.0) ((GradientPulseShape*)*pPulse)->setMaxAmpl(maxampl);
    if (slewrate > 0.0) ((GradientPulseShape*)*pPulse)->setSlewRate(slewrate);
    if (asymsr  != 0.0) ((TGPS*)*pPulse)->setAsymSR(asymsr);
    if (duration > 0.0) ((GradientPulseShape*)*pPulse)->NewDuration(duration);

    //get area from another pulse
    ((GradientPulseShape*)*pPulse)->LinkToPulse(pulse_name);

    if (getareamethod>0) //get area from seq-root parameters (KMAXx or KMAXy)
        ((GradientPulseShape*)*pPulse)->getAreaMethod(getareamethod);

};

/*****************************************************************************/
void XmlSequence::CreateRO_TGPS (PulseShape** pPulse, DOMNode* node) {

    string    name          = "RO_TGPS";
    string    item;
    string    value;
    double    area          =  1.0;
    double    flat_top_duration = 1.0;
    double    factor        =  1.0;
    double    slewrate      = -1.0;
    double    maxampl       = -1.0;
    double    asymsr        =  0.0;
    int       iNADC         =  0;
    PulseAxis eAxis         = AXIS_GX;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)       name              = value;
            if (item == AREA)       area              = atof(value.c_str());
            if (item == FACTOR)     factor            = atof(value.c_str());
            if (item == "Gmax")     maxampl           = atof(value.c_str());
            if (item == "SlewRate") slewrate          = atof(value.c_str());
            if (item == "AsymSR")   asymsr            = atof(value.c_str());
            if (item == "FlatTop")  flat_top_duration = atof(value.c_str());
            if (item == "ADCs")     iNADC             = atoi(value.c_str());
            if (item == "Axis") {
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
                if ( value == "GZ" ) eAxis = AXIS_GZ ;
            }
        }
    }

    *pPulse = new RO_TGPS(area, flat_top_duration, iNADC, eAxis, name);
    ((GradientPulseShape*)*pPulse)->setFactor(factor);
    if (maxampl  > 0.0) ((GradientPulseShape*)*pPulse)->setMaxAmpl(maxampl);
    if (slewrate > 0.0) ((GradientPulseShape*)*pPulse)->setSlewRate(slewrate);
    if (asymsr  != 0.0) ((TGPS*)*pPulse)->setAsymSR(asymsr);
    ((TGPS*)*pPulse)->Prepare(false);
    if (iNADC==0) {
        ((GradientPulseShape*)*pPulse)->setFactor(factor);
        if (eAxis==AXIS_GX) ((GradientPulseShape*)*pPulse)->getAreaMethod(1);
        if (eAxis==AXIS_GY) ((GradientPulseShape*)*pPulse)->getAreaMethod(2);
        if (eAxis==AXIS_GZ) ((GradientPulseShape*)*pPulse)->getAreaMethod(3);
    }

};

/*****************************************************************************/
void XmlSequence::CreatePE_TGPS (PulseShape** pPulse, DOMNode* node) {
    string    name          = "PE_TGPS";
    string    item;
    string    value;
    double    area          =  0.0;
    double    factor        =  1.0;
    double    duration      = -1.0;
    double    slewrate      = -1.0;
    double    maxampl       = -1.0;
    double    asymsr        =  0.0;
    int       steps         =  0;
    bool      noramps       = false;
    PulseAxis eAxis         = AXIS_GY;
    PE_ORDER  order         = LINEAR_UP;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {

        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item == NAME)       name     = value;
            if (item == FACTOR)     factor   = atof(value.c_str());
            if (item == AREA)       area     = atof(value.c_str());
            if (item == "Gmax")     maxampl  = atof(value.c_str());
            if (item == "SlewRate") slewrate = atof(value.c_str());
            if (item == "AsymSR")   asymsr   = atof(value.c_str());
            if (item == "NoRamps" && value=="true")   noramps=true;
            if (item == DURATION)   duration = atof(value.c_str());
            if (item == "Steps")    steps    = atoi(value.c_str());
            if (item == "Axis") {
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
                if ( value == "GZ" ) eAxis = AXIS_GZ ;
            }
            if (item=="Order") {
                if ( value == "LINEAR_UP"   ) order = LINEAR_UP   ;
                if ( value == "LINEAR_DN"   ) order = LINEAR_DN   ;
                if ( value == "CENTRIC_OUT" ) order = CENTRIC_OUT ;
                if ( value == "CENTRIC_IN"  ) order = CENTRIC_IN  ;
            }
        }
    }

    *pPulse = new PE_TGPS(area, steps, order, eAxis, name);
    if (maxampl  > 0.0) ((GradientPulseShape*)*pPulse)->setMaxAmpl(maxampl);
    if (slewrate > 0.0) ((GradientPulseShape*)*pPulse)->setSlewRate(slewrate);
    if (asymsr  != 0.0) ((TGPS*)*pPulse)->setAsymSR(asymsr);
    if (noramps) ((TGPS*)*pPulse)->NoRamps();
    if (duration > 0.0) ((PE_TGPS*)*pPulse)->NewDuration(duration);
    ((GradientPulseShape*)*pPulse)->setFactor(factor);
    if (steps==0) {
        if (eAxis == AXIS_GX) ((GradientPulseShape*)*pPulse)->getAreaMethod(1);
        if (eAxis == AXIS_GY) ((GradientPulseShape*)*pPulse)->getAreaMethod(2);
        if (eAxis == AXIS_GZ) ((GradientPulseShape*)*pPulse)->getAreaMethod(3);
    }

};

/*****************************************************************************/
void XmlSequence::CreateSS_TGPS (PulseShape** pPulse, DOMNode* node) {
    string    name="SS_TGPS";
    string    item;
    string    value;
    double    slewrate       = -1.0;
    double    maxampl        = -1.0;
    double    slicethickness = -1.0;
    int       steps          =  0;
    bool      noramps        = false;
    PulseAxis eAxis          = AXIS_GZ;
    
    DOMNamedNodeMap *pAttributes = node->getAttributes();
    
    if (pAttributes) {
        int nSize = pAttributes->getLength();
        for (int i=0;i<nSize;++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)              name           = value;
            if (item=="SliceThickness")  slicethickness = atof(value.c_str());
            if (item=="Gmax")            maxampl        = atof(value.c_str());
            if (item=="SlewRate")        slewrate       = atof(value.c_str());
            if (item=="Axis") {
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
                if ( value == "GZ" ) eAxis = AXIS_GZ ;
            }
        }
    }
    
    *pPulse = new SS_TGPS(eAxis, slicethickness, name);
    if (maxampl>0.0) ((GradientPulseShape*)*pPulse)->setMaxAmpl(maxampl);
    if (slewrate>0.0) ((GradientPulseShape*)*pPulse)->setSlewRate(slewrate);
    
};

/*****************************************************************************/
void XmlSequence::CreateGradientSpiral (PulseShape** pPulse, DOMNode* node) {
    string    name        = "GradientSpiral";
    string    item;
    string    value;
    double    duration    =  1.0;
    double    turns       =  1.0;
    double    tune        =  0.5;
    double    res         =  1.0;
    PulseAxis eAxis       = AXIS_GX;

    DOMNamedNodeMap *pAttributes = node->getAttributes();

    if (pAttributes) {
        
        int nSize = pAttributes->getLength();

        for (int i=0; i<nSize; ++i) {
            DOMAttr* pAttributeNode = (DOMAttr*) pAttributes->item(i);
            item = XMLString::transcode(pAttributeNode->getName());
            value = XMLString::transcode(pAttributeNode->getValue());
            if (item==NAME)         name     = value;
            if (item==DURATION)     duration = atof(value.c_str());
            if (item=="Turns")      turns    = atof(value.c_str());
            if (item=="Parameter")  tune     = atof(value.c_str());
            if (item=="Resolution") res      = atof(value.c_str());
            if (item=="Axis") {
                if ( value == "GX" ) eAxis = AXIS_GX ;
                if ( value == "GY" ) eAxis = AXIS_GY ;
            }
        }
    }

    *pPulse = new GradientSpiral(duration, turns, tune, res, eAxis, name);

};

