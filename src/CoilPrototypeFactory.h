/** @file CoilPrototypeFactory.h
 *  @brief Implementation of JEMRIS CoilPrototypeFactory
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#ifndef COILPROTOTYPEFACTORY_H_
#define COILPROTOTYPEFACTORY_H_

#include "Coil.h"
#include "PrototypeFactory.h"

/**
 * @brief This interface knows about coil prototypes.
 */
class CoilPrototypeFactory : public PrototypeFactory {


public:

    /**
     * @brief Constructor
     *
     * Will initialise with the available Prototypes.
     * Every available coil type needs to be registered here.
     * Also include the header of the module in the coilprototypefactory.cpp
     */
    CoilPrototypeFactory();

    /**
     * @brief Destructor
     */
    virtual ~CoilPrototypeFactory() {};

    /**
	 * @brief Clone a coil by node type
     *
     * @param node DOMNode containing coil attributes.
	 * @return The pointer to the cloned Coil
     */
    Coil* Clone  (DOMNode* node) ;

    /**
	 * @brief Clone a coil by name
     *
     * @param name of the coil
	 * @return The pointer to the cloned Coil
     */
    Coil* Clone   (string name) ;

    /**
     * @brief Get list of all coil prototypes
     *
     * @return Pointer to the coil prototype list.
     */
    map<string, Coil*>*  getList() {return &m_Clonables;}


 protected:

    /**
     * Get Coil
     *
     * @param name
     */
    Coil* get (string name) ;

    /**
     * Upper case
     *
     * @param  c Char to be converted.
     * @return   Converted char.
     */
    int upper(int c);


 private:


    map<string, Coil*> m_Clonables; /**< @brief Coil protoype list */

};

#endif /*COILPROTOTYPEFACTORY_H_*/
