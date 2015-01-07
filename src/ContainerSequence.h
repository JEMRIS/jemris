/** @file ContainerSequence.h
 *  @brief Implementation of JEMRIS ContainerSequence
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

#ifndef CONTAINERSEQUENCE_H_
#define CONTAINERSEQUENCE_H_

#include "ConcatSequence.h"


/**
 * @brief Prototype of a delay
 */
class ContainerSequence : public ConcatSequence {

 public:

    /**
     * @brief Default constructor
     */
    ContainerSequence() {};

    /**
     * @brief Copy constructor
     */
    ContainerSequence  (const ContainerSequence&);

    /**
     * @brief Default destructor.
     */
    ~ContainerSequence () {};

    /**
     *  See Module::clone
     */
    inline ContainerSequence* Clone() const {return (new ContainerSequence(*this));};

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

};

#endif /*CONTAINERSEQUENCE_H_*/
