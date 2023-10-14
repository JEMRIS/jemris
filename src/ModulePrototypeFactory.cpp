/** @file ModulePrototypeFactory.cpp
 *  @brief Implementation of JEMRIS ModulePrototypeFactory
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
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

#include "ModulePrototypeFactory.h"

#include "Parameters.h"
#include "HardRFPulse.h"
#include "ExternalRFPulse.h"
#include "EmptyPulse.h"
#include "SincRFPulse.h"
#include "GaussianRFPulse.h"
#include "SechRFPulse.h"
#include "AnalyticGradPulse.h"
#include "ConstantGradPulse.h"
#include "AnalyticRFPulse.h"
#include "TrapGradPulse.h"
#include "TriangleGradPulse.h"
#include "SpiralGradPulse.h"
#include "ExternalGradPulse.h"
#include "ConcatSequence.h"
#include "ContainerSequence.h"
#include "Container.h"
#include "AtomicSequence.h"
#include "DelayAtomicSequence.h"

#include "StrX.h"

/***********************************************************/
ModulePrototypeFactory::~ModulePrototypeFactory() {
	map<string,Module*>::iterator it;
	for( it = m_Clonables.begin(); it != m_Clonables.end(); it++ )
		if ( it->first != "PARAMETERS" ) // SeqTree deletes the Parameters singleton!
			delete it->second ;
}

/***********************************************************/
ModulePrototypeFactory::ModulePrototypeFactory () {

	m_Clonables.insert( pair<string,Module*>( "PARAMETERS",          Parameters::instance    () ));
	m_Clonables.insert( pair<string,Module*>( "EMPTYPULSE",          new EmptyPulse          () ));
	m_Clonables.insert( pair<string,Module*>( "HARDRFPULSE",         new HardRFPulse         () ));
	m_Clonables.insert( pair<string,Module*>( "EXTERNALRFPULSE",     new ExternalRFPulse     () ));
	m_Clonables.insert( pair<string,Module*>( "GAUSSIANRFPULSE",     new GaussianRFPulse     () ));
	m_Clonables.insert( pair<string,Module*>( "SECHRFPULSE",         new SechRFPulse         () ));
	m_Clonables.insert( pair<string,Module*>( "SINCRFPULSE",         new SincRFPulse         () ));
	m_Clonables.insert( pair<string,Module*>( "TRAPGRADPULSE",       new TrapGradPulse       () ));
	m_Clonables.insert( pair<string,Module*>( "TRIANGLEGRADPULSE",   new TriangleGradPulse   () ));
	m_Clonables.insert( pair<string,Module*>( "CONSTANTGRADPULSE",   new ConstantGradPulse   () ));
	m_Clonables.insert( pair<string,Module*>( "ANALYTICGRADPULSE",   new AnalyticGradPulse   () ));
	m_Clonables.insert( pair<string,Module*>( "ANALYTICRFPULSE",     new AnalyticRFPulse     () ));
	m_Clonables.insert( pair<string,Module*>( "SPIRALGRADPULSE",     new SpiralGradPulse     () ));
	m_Clonables.insert( pair<string,Module*>( "EXTERNALGRADPULSE",   new ExternalGradPulse   () ));
	m_Clonables.insert( pair<string,Module*>( "CONCATSEQUENCE",      new ConcatSequence      () ));
	m_Clonables.insert( pair<string,Module*>( "CONTAINERSEQUENCE",   new ContainerSequence   () ));
	m_Clonables.insert( pair<string,Module*>( "CONTAINER",           new Container           () ));
	m_Clonables.insert( pair<string,Module*>( "ATOMICSEQUENCE",      new AtomicSequence      () ));
	m_Clonables.insert( pair<string,Module*>( "DELAYATOMICSEQUENCE", new DelayAtomicSequence () ));

}

/***********************************************************/
Module* ModulePrototypeFactory::GetModule     (string name)                    {
    return m_Clonables.find(name)->second;
}

/***********************************************************/
Module* ModulePrototypeFactory::CloneModule   (DOMNode* node)        {

	string  name    = StrX(node->getNodeName()).std_str() ;
	transform(name.begin(), name.end(), name.begin(), (int(*)(int)) toupper);
	Module* toClone = GetModule(name);
	if (!toClone)
		return NULL;
	return toClone->Clone();

}


/***********************************************************/
Module* ModulePrototypeFactory::CloneModule   (string name)        {

	Module* toClone = GetModule(name);
	if (!toClone)
		return NULL;
	return toClone->Clone();
}

