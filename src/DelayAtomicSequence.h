/** @file DelayAtomicSequence.h
 *  @brief Implementation of JEMRIS DelayAtomicSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2025  Tony Stoecker
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

#ifndef DELAYATOMICSEQUENCE_H_
#define DELAYATOMICSEQUENCE_H_

#include "AtomicSequence.h"


/**
 * @brief Delay types
 */
enum DelayType {
    DELAY_B2E, /**< The delay starts with beginning of the given start sequence and ends with the end of the given end sequence.*/
    DELAY_C2C, /**< The delay starts from the center of the given start sequence and ends with the center of the given end sequence.*/
    DELAY_B2C, /**< The delay starts with beginning of the given start sequence and ends with the center of the given end sequence.*/
    DELAY_C2E  /**< The delay starts from the center of the given start sequence and ends with the end of the given end sequence.*/
};

/**
 * @brief Prototype of a delay
 */
class DelayAtomicSequence : public AtomicSequence {

 public:

    /**
     * @brief Default constructor
     */
    DelayAtomicSequence() :
    	m_adc(0), m_delay_time(0),
    	m_phase_lock(0), m_dt(DELAY_B2E), m_await_time(0),
    	m_iMYpos(0), m_iS1pos(10000), m_iS2pos(-1) {};

    /**
     * @brief Copy constructor
     */
    DelayAtomicSequence  (const DelayAtomicSequence&);

    /**
     * @brief Default destructor.
     */
    ~DelayAtomicSequence () {};

    /**
     *  See Module::clone
     */
    inline DelayAtomicSequence* Clone() const {return (new DelayAtomicSequence(*this));};

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

    /**
     * @brief  Prepare the delay atomic sequence.
     * - Search start and stop sequences and their positions on this level of the tree
     * - Observe timing of modules between start and stop
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return Success
     */
    bool  PrepareDelay   (const PrepareMode mode);

    /**
     * @brief Collect sequence data (for running on the scanner)
     */
    virtual void CollectSeqData	(OutputSequenceData *seqdata);

 protected:
    /**
     * @brief Get information on this module.
     *
     * @return Information for display.
     */
    virtual string GetInfo () ;

    /**
     * @brief Get delay length
     *
     * @return  delay in ms
     */
    double  GetDelay();

 private:

    string    m_start;         /**< @brief Name of the sequence after which the delay shall starts.             */
    string    m_stop;          /**< @brief Name of the sequence from after which the delay shall starts.        */
    string    m_delay_type;    /**< @brief Type of this delay.                                                  */

    double    m_await_time;    /**< @brief Rest time. Real length of Delay in ms. Is calculated during runtime. */
    double    m_delay_time;    /**< @brief Declared length of Delay.                                            */

    int		  m_iMYpos;
    int		  m_iS1pos;
    int		  m_iS2pos;

    vector<Sequence*> m_seqs;  /**< @brief Durations of other modules which have to be observed                  */

    DelayType m_dt;            /**< @brief Type of this delay.                                                  */

    int       m_adc;           /**< @brief Number of ADCs                                                       */
    int       m_adc_flag;      /**< @brief ADC flag                                                             */
    bool      m_phase_lock;    /**< @brief Lock phase of ADCs to the phase of the last RF pulse event           */

};

#endif /*DELAYATOMICSEQUENCE_H_*/
