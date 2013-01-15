/** @file ConcatSequence.h
 *  @brief Implementation of JEMRIS ConcatSequence
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

#ifndef CONCATSEQUENCE_H_
#define CONCATSEQUENCE_H_

#include "Sequence.h"
#include "RepIter.h"

/**
 * @brief Concat sequence prototype
 */

class ConcatSequence : public Sequence {

 public:

    /**
     * @brief Default constructor
     */
    ConcatSequence() {};

    /**
     * Copy constructor.
     */
    ConcatSequence                 (const ConcatSequence& );

    /**
     * @brief Default destructor.
     */
    ~ConcatSequence                () {};

    /**
     *  See Module::clone
     */
    inline ConcatSequence* Clone          () const { return (new ConcatSequence(*this)); };

    /**
     * @brief Prepare the sequence.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return Success
     */
    virtual bool    Prepare        (PrepareMode mode) ;

    /**
     * See Module::GetValue
     */
    virtual void            GetValue       (double * dAllVal, double const time) ;

    /**
     * See Module::GetValue
     */
    virtual void            GetValue       (double * dAllVal, double const time, double * pos[3]) {};


    /**
     * See Module::GetDuration
     */
    double          GetDuration    ();

    /**
     * @brief Default destructor.
     */
    virtual int     GetNumOfTPOIs ();

    /**
     * @brief Get the number of repetitions
     *
     * @return The number of repetitions.
     */
    inline virtual unsigned int    GetMyRepetitions ()                  { return m_repetitions; };

    /**
     * @brief Set the number of repetitions
     *
     * @param val The number of repetitions. val>0 required.
     */
    void            SetRepetitions (unsigned int val);

    /**
     * @brief Set the current repetition counter of this ConcatSequence.
     *
     * @param val The current repetition counter.
     * @param record Record (default false)
     */
    void            SetRepCounter  (unsigned int val, bool record=false);

    /**
     * @brief Get the current repetition counter of this ConcatSequence.
     *
     * @return The current repetition counter.
     */
    inline unsigned int    GetMyRepCounter  () {return m_counter;};

    /**
     * Get the initial iterator for counting repetitions.
     *
     * @return The first repetition iterator.
     */
    inline RepIter begin() {m_counter=-1; NewState(m_counter); SetRepCounter(0); return RepIter(this,0);}

    /**
     * Get the final iterator for counting repetitions.
     *
     * @return The final repetition iterator.
     */
    RepIter end()  { return RepIter(this,m_repetitions); }

    /*
     * @brief Create a DOM tree where all expressions are evaluated (for IDEA)
     *
     * @param doc  Document
     * @param node Node
     *
     * @return success or failure
     */
    //virtual bool StaticDOM(DOMDocument* doc, DOMNode* node);

 protected:
    /**
     * Get informations on this ConcatSequence
     *
     * @return The number of the repetitions
     */
    virtual string          GetInfo        ();

    unsigned int m_repetitions; /**< @brief The number of repetitions for this container */
    unsigned int m_counter;     /**< @brief Current value of the repetition counter for this container */
};

#endif /*CONCATSEQUENCE_H_*/
