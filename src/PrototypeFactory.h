/** @file PrototypeFactory.h
 *  @brief Implementation of JEMRIS PrototypeFactory
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

#ifndef PROTOTYPEFACTORY_H_
#define PROTOTYPEFACTORY_H_

#include "Prototype.h"
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <xercesc/dom/DOMNode.hpp>
XERCES_CPP_NAMESPACE_USE
#include <cctype>
#include <algorithm>
using namespace std;

/**
 * @brief Base class for prototype factories.
 */
class PrototypeFactory {

 public:

    /**
     * @brief Default constructor
     */
    PrototypeFactory                           () {};

    /**
     * @brief Default destructor
     */
    virtual ~PrototypeFactory                  () {};

    /**
	 * @brief Clone a prototype by node.
     *
     * @param node DOMNode containing prototype attributes.
	 * @return     Pointer to cloned prototype.
     */
    Prototype*                Clone   (DOMNode* node) ;

    /**
	 * @brief Clone a prototype by name.
     *
     * @param name Prototype name.
	 * @return     Pointer to cloned prototype .
     */
    Prototype*                Clone   (string   name) ;

    /**
     * @brief Get Pointer to list of clonable prototypes.
	 *
	 * @return Pointer to list of clonable prototypes.
     */
    inline map<string, Prototype*>*  getList () {return &m_Clonables;}

 protected:

    /**
     * @brief Get pointer to Prototype by name
     *
     * @param name Prototype name
     */
    Prototype*                get     (string name);

    /**
     * Upper case
     *
     * @param c Char to be converted.
     * @return Converted char.
     */
    int                       upper            (int c);


 protected:

    map<string, Prototype*> m_Clonables; /** < @brief Prototype list */

};

#endif /*PROTOTYPEFACTORY_H_*/
