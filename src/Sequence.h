/** @file Sequence.h
 *  @brief Implementation of JEMRIS Sequence
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

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Module.h"
#include "Parameters.h"

//! Super class for all sequence (non-pulse) modules

class Sequence : public Module {

public:

    /**
     * @brief Default constructor.
     */
    Sequence () {m_aux1=0;m_aux2=0;m_aux3=0;};

    /**
     * @brief Default destructor.
     */
    virtual ~Sequence () {};

    /**
     * @brief Default copy constructor.
     */
    Sequence (const Sequence&) {};

    /**
     * See Module::GetValue
     */
    virtual void    GetValue   (double * dAllVal, double const time) {};

    /**
     * Get the duration of this module.
     *
     * @return Duration in ms
     */
    virtual double GetDuration () = 0;

    /**
     * Prepare the sequence.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return Success
     */
    virtual bool Prepare       (PrepareMode mode) ;

    /**
     * Sequence Diag
     *
     * @param fname File name
     */
    void  SeqDiag  (string fname = "seq.bin");

    /**
     * Write sequence file
     *
     * @param pfout File output stream
     * @param time  Time: More elaborate description, please.
     */
    void CollectSeqData          (vector <double*> seqdata, double& time, long& offset);

    /**
     * Get the number of ADCs for this sequence.
     *
     * @return The nmuber of ADCs.
     */
    long GetNumOfADCs ();


 protected:

    vector<string>   m_prep_errors; /**< @brief Preparation Error Messages of Modules */


 private:

    double m_aux1 ;  /**< @brief auxiliary general purpose variable for convinience */
    double m_aux2 ;  /**< @brief auxiliary general purpose variable for convinience */
    double m_aux3 ;  /**< @brief auxiliary general purpose variable for convinience */

};

#endif /*SEQUENCE_H_*/
