/** @file Container.h
 *  @brief Implementation of JEMRIS Container
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "ConcatSequence.h"

//declare ContainerSequence
class ContainerSequence;

/**
 * @brief Prototype of a Container
 */

class Container : public ConcatSequence {

 public:

    /**
     * @brief Default constructor
     */
	Container() {};

    /**
     * @brief Copy constructor
     */
	Container  (const Container&);

    /**
     * @brief Default destructor.
     */
    ~Container () {};

    /**
     *  See Module::clone
     */
    inline Container* Clone() const {return (new Container(*this));};

    /**
     * @brief Prepare the delay atomic sequence.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
	 * @return     Success.
     */
    virtual bool                 Prepare           (const PrepareMode mode);

    /**
     * See Module::GetValue()
     */
    virtual void                 GetValue          (double * dAllVal, double const time) {};


 protected:
    /**
     * @brief Get information on this module.
     *
     * @return Information for display.
     */
    virtual string GetInfo () ;

 private:

    string				m_container_name; /**< @brief name of the ContainerSequence */
    ContainerSequence*	m_container_seq;  /**< @brief pointer to the ContainerSequence */
};


#endif /*CONTAINER_H_*/
