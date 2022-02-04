/** @file ContainerSequence.h
 *  @brief Implementation of JEMRIS ContainerSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#ifndef CONTAINERSEQUENCE_H_
#define CONTAINERSEQUENCE_H_

#include "ConcatSequence.h"

//forward declaration of Container class
class Container;

/**
 * @brief Prototype of a SequenceContainer
 */
class ContainerSequence : public ConcatSequence {

 friend class Container;

 public:

    /**
     * @brief Default constructor
     */
    ContainerSequence() : m_container(0) {};

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
    //virtual void                 GetValue          (double * dAllVal, double const time) {};

    /**
     * Get the Container for this ContainerSequence.
     *
     * @return Pointer to the Container.
     */
    Container*  GetContainer() {return m_container; };


    /**
     * Set the Container for this ContaineSequencer.
     *
     * @param pC pointer to the Container.
     */
    void  SetContainer(Container* pC) { m_container=pC; };


 protected:
    /**
     * @brief Get information on this module.
     *
     * @return Information for display.
     */
    virtual string GetInfo () ;

 private:

    Container*		m_container;		/**< @brief pointer to the Container */
    double			m_import [10];		/**< @brief attributes to import */
    double			m_export [10];		/**< @brief attributes to export */
    vector<string>	m_impexpdoc;		/**< @brief attributes to export */

};

#endif /*CONTAINERSEQUENCE_H_*/
