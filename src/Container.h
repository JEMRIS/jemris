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

#include "Sequence.h"

//declare ContainerSequence
class ContainerSequence;

/**
 * @brief Prototype of a Container
 */

class Container : public Sequence {

 friend class ContainerSequence;

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
    ~Container ();

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
     * See Module::GetDuration
     */
    double          GetDuration    ();

    /**
     * See Module::GetValue()
     */
    virtual void                 GetValue          (double * dAllVal, double const time);

    /**
     * @brief Get Number of TPOIs for this sequence.
     * @return Number of TPOIs
     */
    virtual int     GetNumOfTPOIs ();

    /**
     * Get the number of ADCs for this sequence.
     *
     * @return The number of ADCs.
     */
    virtual long GetNumOfADCs ();

    /**
     * @brief Recursively collect sequence data (for plotting the sequence diagram)
     */
    virtual void CollectSeqData (NDData<double>& seqdata, double& t, long& offset);


    /**
     * Get the ContainerSequence for this Container.
     *
     * @return Pointer to the ContainerSequence.
     */
    ContainerSequence*	GetContainerSequence() {return m_container_seq; };


 protected:
    /**
     * @brief Get information on this module.
     *
     * @return Information for display.
     */
    virtual string GetInfo () ;

 private:

    string				m_container_seq_name; 	/**< @brief name of the ContainerSequence */
    ContainerSequence*	m_container_seq;      	/**< @brief pointer to the ContainerSequence */
    SequenceTree*	    m_container_seqtree;  	/**< @brief pointer to the SequenceTree of the ContainerSequence */
    double				m_import [10];			/**< @brief attributes to import */
    double				m_export [10];			/**< @brief attributes to export */

};


#endif /*CONTAINER_H_*/
