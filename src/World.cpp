/** @file World.cpp
 *  @brief Implementation of JEMRIS World
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "World.h"
#include "Model.h"
#include "SequenceTree.h"

World* World::m_instance = 0;

/***********************************************************/
World* World::instance() {

    if (m_instance == 0) {

        m_instance = new World();

	//MODIF
        fstream logAct("logActivation.txt",ios::in);
        if(logAct.is_open())	{
		logAct>>m_instance->logFile;
		logAct>>m_instance->logTrajectories;
	}
	else
	{
		m_instance->logFile 	    = 0;
		m_instance->logTrajectories = 0;
	}
        logAct.close();
        m_instance->m_trajBegin         =  0;
        m_instance->m_trajSize          =  1;
        //MODIF***
        m_instance->time                =  0.0;
        m_instance->total_time          =  0.0;
        m_instance->phase               = -1.0;
        m_instance->deltaB              =  0.0;
        m_instance->GMAXoverB0          =  0.0;
        m_instance->NonLinGradField     =  0.0;
        m_instance->RandNoise           =  0.0;
        m_instance->saveEvolStepSize    =  0;
        m_instance->saveEvolFileName    =  "";
        m_instance->saveEvolOfstream    = NULL;
        m_instance->saveEvolFunPtr      = &Model::saveEvolution;
        m_instance->solverSuccess       = true;
        m_instance->m_noofspinprops     = 9;

        m_instance->pSeqTree            = NULL;
        m_instance->pAtom               = NULL;
        m_instance->pStaticAtom         = NULL;

        m_instance->m_myRank            = -1;
        m_instance->m_useLoadBalancing  = true;
        m_instance->m_no_processes      = 1;  /* default: serial jemris */
        m_instance->m_startSpin         = 0;

        m_instance->m_slice             = 0;
        m_instance->m_lastScanInSlice   = false;

    }

    XMLPlatformUtils::Initialize ();
    
    return m_instance;

}

/***********************************************************/
double World::ConcomitantField (double* G) {

	if (GMAXoverB0==0.0) 
		return 0.0;

	return ((0.5*GMAXoverB0)*(pow(G[0]*Values[ZC]-0.5*G[2]*Values[XC],2) + pow(G[1]*Values[ZC]-0.5*G[2]*Values[YC],2))) ;

}

/***********************************************************/
void World::SetNoOfSpinProps (int n) { 

	// valid also for multi pool sample
	if ( m_noofspincompartments > 1 ){
		int m_ncoprops =  (n - 4) / m_noofspincompartments;
		m_noofspinprops = n;
		Values = new double [n*m_ncoprops];
		for ( int i = 0; i < n; i++ )
			for ( int j = 0; j<m_ncoprops; j++ )
				Values [m_ncoprops*j+i] = 0.0;
	}else{
		m_noofspinprops = n;
		Values = new double [n];
		for ( int i = 0; i < n; i++ )
			Values [i] = 0.0;
	}
}

void World::InitHelper (long size)  {
  if (size > 0)
    helper.resize(size);

}

int World::GetNoOfCompartments () {
	return m_noofspincompartments;
}

void World::SetNoOfCompartments (int n) {

	m_noofspincompartments = n;

    if (solution.size() < 3*n) {
    	solution.clear();
        solution.resize(m_noofspincompartments * 3);
    }

}
   

World::~World () { 
	
	m_instance=0; 
	
	if (Values)
		delete Values; 
	
}
