/** @file CoilPrototypeFactory.cpp
 *  @brief Implementation of JEMRIS CoilPrototypeFactory
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


#include "CoilPrototypeFactory.h"
#include "IdealCoil.h"
#include "ExternalCoil.h"
#include "AnalyticCoil.h"
#include "BiotSavartLoop.h"

#include "StrX.h"

/***********************************************************/
CoilPrototypeFactory::CoilPrototypeFactory () {

	m_Clonables.insert( pair<string,Coil*>( "ANALYTICCOIL",   new AnalyticCoil() ));
	m_Clonables.insert( pair<string,Coil*>( "BIOTSAVARTLOOP", new BiotSavartLoop() ));
	m_Clonables.insert( pair<string,Coil*>( "IDEALCOIL"     , new IdealCoil()      ));
	m_Clonables.insert( pair<string,Coil*>( "EXTERNALCOIL"  , new ExternalCoil()   ));

}

/***********************************************************/
Coil* CoilPrototypeFactory::get (string name) {
    return m_Clonables.find(name)->second;
}

/***********************************************************/
Coil* CoilPrototypeFactory::Clone (DOMNode* node) {

	string  name  = XMLString::transcode    (node->getNodeName());
	transform(name.begin(), name.end(), name.begin(), (int(*)(int)) toupper);
	Coil* toClone = get(name);

	if (!toClone)
		return NULL;

	return toClone->Clone();

}


/***********************************************************/
Coil* CoilPrototypeFactory::Clone (string name) {

	Coil* toClone = get(name);

	if (!toClone)
		return NULL;

	return toClone->Clone();

}

