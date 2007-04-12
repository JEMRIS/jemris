/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai, K. Vahedipour
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef 	SEQUENCE_H
#define 	SEQUENCE_H

#include 	<stdexcept>
#include 	<string>
using 	std::string;
#include 	<fstream>
using 	std::ofstream;
#include 	<cstdlib>

#include 	"Parameter.h"

class 	PulseShape; //declaration is needed for the virtual FindPulse method

class  	Sequence {

	public:
	Sequence() 
	{ 
		m_bNewParam=false; m_dFactor=1.0; m_iTreeSteps=0; 
	};
	~Sequence() {};
	
	//virtual functions to be implemented by derived classes
  	virtual double 		getDuration			()=0;
  	
  	virtual bool  		getValue			
  					(const double time , double* const d_AllValuesPtr)=0;
  						
  	virtual void 		Destroy			()=0;
  	
  	virtual void 		writeADCs			(ofstream* pfout)=0;
  	
  	virtual void 		writeSeqVal			
  						(double& dTimeShift,ofstream* pfout)=0;
  	
  	virtual int 		getNumberOfChildren	() {return 0;};
  	
  	virtual Sequence* 	getChild			(int ChildID) {return NULL;};
  	
  	virtual bool 		Prepare			(bool verbose) {return true;};
  	
  	virtual void 		setListOfTimepoints	(bool force) {};
  	
  	virtual PulseShape* 	FindPulse			(string sPulseName)=0;
  	
  	virtual void 		getInfo			(int ident)=0;
  	
  	//local function implementations
  	Sequence* 			getParent 			() {return m_cParentPtr;}; 
  	
  	Sequence* 			getRoot 			() 
  	{
  		Sequence* S=this; 
  		for(;;) {S=S->getParent(); if (S==S->getParent()) return S;} 
  	}; 
  	
  	string 			getName 			() {return m_sName;};
  	void 				setName 			(string value)
  		{m_sName=value;};

  	void 				setParent			(Sequence* value)
  		{m_cParentPtr = value;};

  	void 				writeBinaryFile		
  					(string filename, double dT, string adcfile);

	Parameter* 			getParameter		() {return &m_Parameter;};

	int 				getTreeSteps 		() {return m_iTreeSteps;};
	 
	void 				setTreeSteps 		(int value )
		{m_iTreeSteps = value;};


	/* these functions might be used during Prepare() 
	*  to check whether parameters have changed */
	
	void 				NewParam			(bool val) 
									{m_bNewParam = val; };
	bool 				NewParam 			() {return m_bNewParam; };

	void 				setFactor			(double val) {m_dFactor=val;};
	double 			getFactor			() {return m_dFactor;};

	private:
	Sequence*			m_cParentPtr;
	string			m_sName;
	Parameter			m_Parameter;
	bool				m_bNewParam;
	double			m_dFactor;
	int				m_iTreeSteps;

};

#endif
