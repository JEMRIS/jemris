/** @file World.cpp
 *  @brief Implementation of JEMRIS World
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

World* World::m_instance = 0;

/***********************************************************/
World* World::instance() {

    if (m_instance == 0) {

        m_instance = new World();


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


	m_instance->pAtom		= NULL;
	m_instance->pStaticAtom		= NULL;
		m_instance->pAtom		= NULL;

        for (int i=0; i<3; i++)
            m_instance->solution[i]   =  0.0;

        for (int i=0; i<8; i++)
            m_instance->InitValues[i] =  0.0;

        m_instance->Values            = m_instance->InitValues;

        m_instance->m_myRank		  = -1;
        m_instance->m_useLoadBalancing= false;


    }

    return m_instance;

}

/***********************************************************/
double World::ConcomitantField (double* G) {

        if (GMAXoverB0==0.0) return 0.0;
        return ((0.5*GMAXoverB0)*(pow(G[0]*Values[ZC]-0.5*G[2]*Values[XC],2) + pow(G[1]*Values[ZC]-0.5*G[2]*Values[YC],2))) ;

};
