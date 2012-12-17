/** @file ModulePrototypeFactory.h
 *  @brief Implementation of JEMRIS ModulePrototypeFactory
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
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

#ifndef MODULEPROTOTYPEFACTORY_H_
#define MODULEPROTOTYPEFACTORY_H_

#include "PrototypeFactory.h"
#include "Module.h"
/**
 * @brief Module factory 
 */
class ModulePrototypeFactory : public PrototypeFactory {

 public:

    /**
     * Constructor
     * 
     * Will initialise with the available Prototypes. Every available module needs to be registered here!
     * Also include the header of the module in the ModulePrototypeFactory.cpp
     */    
    ModulePrototypeFactory();

    /**
     * Destructor
     */
    virtual ~ModulePrototypeFactory();

    /**
	 * @brief Clone a module by node type
     *
     * @param node DOMNode containing module attributes. 
	 * @return The pointer to the cloned Module 
     */
    Module* CloneModule   (DOMNode* node) ;

    /**
	 * @brief Clone a module by name
     *
     * @param name of the module
	 * @return The pointer to the cloned Module 
     */
    Module* CloneModule   (string name) ;

    /**
     *  Return a pointer to the clonable Module list.
     */
    map<string, Module*>*  GetModuleList() {return &m_Clonables;}

 protected:
    
    /**
     * Get Module
     *
     * @param name
     */
    Module* GetModule (string name) ;
    
    /**
     * Upper case
     * 
     * @param c Char to be converted.
     * @return Converted char.
     */
    int upper(int c);
    
private:
	//Modules
	map<string, Module*> m_Clonables;

};

#endif /*MODULEPROTOTYPEFACTORY_H_*/
