/** @file AtomicSequence.h
 *  @brief Implementation of JEMRIS AtomicSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2018  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2018  Daniel Pflugfelder
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

#ifndef ATOMICSEQUENCE_H_
#define ATOMICSEQUENCE_H_

#include "NDData.h"

#include "Sequence.h"
#include "Pulse.h"

using std::vector;

/**
 *  @brief Atomic sequence prototype
 */
class AtomicSequence : public Sequence {

 public:

    /**
     * @brief Default constructor
     */
    AtomicSequence() :m_theta(0.), m_non_lin_grad(0.), m_alpha(0.), m_phi(0.), m_eddy(false) {};

    /**
     * @brief Copy constructor.
     */
    AtomicSequence  (const AtomicSequence&);

    /**
     * @brief Default destructor.
     */
    virtual ~AtomicSequence () {};

    /**
     *  @brief See Module::clone
     */
    inline AtomicSequence* Clone() const {return (new AtomicSequence(*this));};

    /**
     * @brief Prepare the sequence.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool    Prepare        (const PrepareMode mode);

    /**
     * @brief Get the pulse given by number.
     *
     * @param number Position in pulse vetor
     * @return       Requested pulse
     */
    Pulse*          GetPulse           (int  number)  { return m_pulses.at(number); };

    /**
     * @brief  Get the number of nested pulses
     *
     * @return The number of nested pulses of this atomic sequence
     */
    int             GetNumberOfPulses ()             { return m_pulses.size(); };

    /**
     * @brief See Module::GetValue
     */
    virtual void    GetValue          (double * dAllVal, double const time) ;


    /**
     * See Module::GetDuration
     */
    double          GetDuration    ();


    /**
     * @brief See Module::GetValue
     */
    //virtual void    GetValue          (double * dAllVal, double const time, double * pos[3]) {};

    /**
     * @brief Perform a Rotation of the gradients.
     *
     * @param Grot   Rotation matrix.
     */
    void Rotation (double * Grot);

    /**
     * @brief  Check for nonlinear gradients in this atom.
     *
     * @return True, if nonlinear gradients are present
     */
    inline bool          HasNonLinGrad () {return m_non_lin_grad;};

    /**
     * @brief Marh this atom, if nonlinear gradients are present
     *
     * @param val True, if nonlinear gradients are present
     */
    inline void          SetNonLinGrad (bool val) {m_non_lin_grad=val;};

    /**
     * @brief Collect the TPOIs of child pulses
     *
     * The method calls Pulse::SetTPOIs of all pulses in the atom,
     * and adds, sorts, and purges all these TPOIs.
     * The method is automatically triggered by Module::notify
     * if a pulse inside the atom changes a private member
     * through observation.
     */
    void           CollectTPOIs       ();

    /**
     * @brief Collect sequence data (for plotting the sequence diagram)
     */
    virtual void CollectSeqData (NDData<double>& seqdata, double& t, long& offset);

    /**
     * @brief Recursively collect sequence data (for running on the scanner)
     */
    virtual void CollectSeqData	(OutputSequenceData *seqdata);

    /**
     * @brief Get Number of ADCs in this atom
     * @return number of ADCs
     */
    virtual long  GetNumOfADCs ();


    /**
     * @brief GetValue of lingering eddy currents
     */
    void    GetValueLingeringEddyCurrents (double * dAllVal, double const time);

    /**
     * @brief prepare lingering eddy currents in this atom for GetValue in next atom
     */
    void    PrepareEddyCurrents ();

    /**
     * @brief update lingering eddy currents which are still alive after this atom
     */
    void    UpdateEddyCurrents ();

    /**
     * @brief true, if this atom contains eddy currents
     * @return status of eddy currents
     */
    bool    HasEddyCurrents () {return m_eddy; };

    /**
     * @brief true, if this atom contains eddy currents
     * @return status of eddy currents
     */
    void    SetEddyCurrents (bool val) {m_eddy = val; };

protected:
    /**
     * Get informations on this AtomicSequence
     *
     * @return Information for display
     */
    virtual string          GetInfo        ();


 private:

    vector<Pulse*> m_pulses;       /**< @brief vector of pointers to child pulses */

    bool           m_non_lin_grad; /**< @brief A flag for nonlinear gradients */
    double         m_alpha;        /**< @brief Gradient Rotation matrix: Rotation angle */
    double         m_theta;        /**< @brief Gradient Rotation matrix: polar inclination from z-axis */
    double         m_phi;          /**< @brief Gradient Rotation matrix: azimutal phase measured from x-axis*/
    bool           m_eddy; 		   /**< @brief A flag for eddy currents in this atom */

};

#endif /*ATOMICSEQUENCE_H_*/
