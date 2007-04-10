/*
	This file is part of the MR simulation project
	Date: 03/2006
	Author:  T. Stoecker, J. Dai
	MR group, Institute of Medicine, Research Centre Juelich, Germany
*/




#ifndef _SCANNER_H_
#define _SCANNER_H_

#define MAXSLEWRATE 	2
#define GMAX  		1


class   Scanner
{
public:

 //constructor
 Scanner(){m_dMaxSlewrate = MAXSLEWRATE;m_dMaxGradientPower = GMAX;};
 //destructor
  ~Scanner(){};

  double getMaxSlewRate (){return m_dMaxSlewrate;};
  double getMaxGradientPower(){return m_dMaxGradientPower;};

void setMaxSlewrate (double value ){m_dMaxSlewrate = value;};

void setMaxGradientPower (double value ){m_dMaxGradientPower = value;};

private:
  double m_dMaxSlewrate;
  double m_dMaxGradientPower;	

};//end Class Sequence


#endif
