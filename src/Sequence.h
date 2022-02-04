/** @file Sequence.h
 *  @brief Implementation of JEMRIS Sequence
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

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Module.h"
#include "Parameters.h"
#include "NDData.h"
#include "OutputSequenceData.h"

#include "ismrmrd/ismrmrd.h"
#include "ismrmrd/xml.h"
#include "ismrmrd/dataset.h"
#include "ismrmrd/version.h"


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
    Sequence (const Sequence&) {m_aux1=0;m_aux2=0;m_aux3=0;};

    /**
     * See Module::GetValue
     */
    virtual void    GetValue   (double * dAllVal, double const time) {};

    /**
     * Prepare the sequence.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return Success
     */
    virtual bool Prepare       (const PrepareMode mode) ;

    /**
     * Sequence Diag
     *
     * @param fname File name
     */
    void  SeqDiag  (const string& fname = "seq.h5");

   /**
     * New Sequence Diag via ISMRMRD
     *
     * @param fname File name
     */
    bool  SeqISMRMRD  (const string& fname = "seq.h5");


    /**
     * @brief Recursively collect sequence data (for plotting the sequence diagram)
     */
    virtual void CollectSeqData          (NDData<double>& seqdata, double& t, long& offset) = 0;

    /**
     * Sequence output
     *
     * @param fname output filename
     */
    void OutputSeqData (map<string,string> &scanDefinitions, const string& outputDir="", const string& outFile="external.seq");

    /**
     * @brief Recursively collect sequence data (for running on the scanner)
     */
    virtual void CollectSeqData	(OutputSequenceData *seqdata) = 0;

    /**
     * Get the number of ADCs for this sequence.
     *
     * @return The number of ADCs.
     */
    virtual long GetNumOfADCs () = 0;


 protected:

    vector<string>   m_prep_errors; /**< @brief Preparation Error Messages of Modules */


 private:

    double m_aux1 ;  /**< @brief auxiliary general purpose variable for convinience */
    double m_aux2 ;  /**< @brief auxiliary general purpose variable for convinience */
    double m_aux3 ;  /**< @brief auxiliary general purpose variable for convinience */

};

#endif /*SEQUENCE_H_*/
