/*
        This file is part of the MR simulation project
        Date: 03/2006
        Authors:  T. Stoecker, J. Dai
        MR group, Institute of Medicine, Research Centre Juelich, Germany
*/

#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#define PARAM_PI    3.14159265358979

class Parameter {
 
public:
 Parameter(){
	m_iNx = 64;
	m_iNy = 64;
	m_iNz = 1;
	m_dTE = 1.0;
	m_dTR = 10.0;
	m_dTI = 5.0;
	m_dTD = 0.0;
	m_dReadBW = 0.0; //zero default bandwidth! (=> do not overwrite the BW set with RO_TGPS pulses)
	m_dFOVx = 256;
	m_dFOVy = 256;
	m_dFOVz = 1;
	m_dSlewRate = -1.0; //default: no scanner limits settings
	m_dMaxAmpl  = -1.0; //         through the parameter class
  };
 ~Parameter(){ };

//setter
void setNx(int val){m_iNx=val;}; 
void setNy(int val){m_iNy=val;}; 
void setNz(int val){m_iNz=val;}; 
void setTR(double val){m_dTR=val;}; 
void setTE(double val){m_dTE=val;}; 
void setTI(double val){m_dTI=val;}; 
void setTD(double val){m_dTD=val;}; 
void setFOVx(double val){m_dFOVx=val;}; 
void setFOVy(double val){m_dFOVy=val;}; 
void setFOVz(double val){m_dFOVz=val;}; 
void setDz(double val){m_dFOVz=val*m_iNz;}; //slice thickness is not used internally
void setReadBW(double val){m_dReadBW=val;}; 
void setSlewRate ( double val ){m_dSlewRate=val; }; 
void setMaxAmpl  ( double val ){m_dMaxAmpl =val; }; 

//getter
int getNx(){return m_iNx;}; 
int getNy(){return m_iNy;}; 
int getNz(){return m_iNz;}; 
double getTR(){return m_dTR;}; 
double getTE(){return m_dTE;}; 
double getTI(){return m_dTI;}; 
double getTD(){return m_dTD;}; 
double getReadBW(){return m_dReadBW;}; 
double getFOVx(){return m_dFOVx;}; 
double getFOVy(){return m_dFOVy;}; 
double getFOVz(){return m_dFOVz;}; 
double getDx(){return m_dFOVx/m_iNx;}; 
double getDy(){return m_dFOVy/m_iNy;}; 
double getDz(){return m_dFOVz/m_iNz;}; 
double getKMAXx(){return PARAM_PI/getDx();}; 
double getKMAXy(){return PARAM_PI/getDy();}; 
double getKMAXz(){return PARAM_PI/getDz();}; 
double getDKx(){return 2*getKMAXx()/m_iNx;}; 
double getDKy(){return 2*getKMAXy()/m_iNy;}; 
double getDKz(){return 2*getKMAXz()/m_iNz;}; 
double getSlewRate ( ){return m_dSlewRate;}; 
double getMaxAmpl ( ){return m_dMaxAmpl;}; 

double getArea(int method){
	switch (method)
	{
		case 1 : return  getKMAXx();
		case 2 : return  getKMAXy();
		case 3 : return  getKMAXz();
		case 4 : return  getDKx();
		case 5 : return  getDKy();
		case 6 : return  getDKz();
		default : return 0.0;
	}
};

private:
	int m_iNx;
	int m_iNy;
	int m_iNz;
	double m_dTE;
	double m_dTR;
	double m_dTI;
	double m_dTD;
	double m_dReadBW;
	double m_dFOVx;
	double m_dFOVy;
	double m_dFOVz;
	double 	m_dSlewRate;
	double 	m_dMaxAmpl;
};


#endif
