/** @file EddyPulse.h
 *  @brief Implementation of JEMRIS EddyPulse
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

#ifndef EDDYPULSE_H_
#define EDDYPULSE_H_

#include "GradPulse.h"
#include "AtomicSequence.h"

//! Prototype of an Eddy pulse

class EddyPulse : public GradPulse {

 public:

    /**
     * @brief Default constructor
     */
    EddyPulse           () {Init();};

    /**
     * @brief Default destructor.
     */
    ~EddyPulse          () {};

    /**
     * @brief Default copy constructor.
     */
    EddyPulse           (const EddyPulse&){Init();};

    /**
     * @brief initialize .
     */
    void Init();

    /**
     *  See Prototype::Clone.
     */
    inline EddyPulse*         Clone() const { return (new EddyPulse(*this)); };

   /**
     * @brief Prepare the hard RF pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return     Success.
     */
    virtual bool Prepare (PrepareMode mode);

    /**
     * @brief Insert EddyCurrent pulse in sequence tree
     */
    bool  Insert (PrepareMode mode);

    /**
     * @brief compute the convolution kernel
     */
    bool  Convolve ();

    /**
     * @brief compute the area of the eddy currents
     */
    double GetAreaNumeric (int steps);

    /**
     * @brief Get linger time of the eddy current (eddy current duration outside its parent atom)
     * @return linger time
     */
    double GetLingerTime() {return m_linger_time;};

    /**
     * @brief Get duration of the parent atom of this eddy current
     * @return duration
     */
    double GetParentDuration() {if (m_parent!=NULL) return m_parent->GetDuration(); else return 0.0; };

    /**
     * See GradPulse::GetValue
     */
    virtual void GetValue  (double * dAllVal, double const time);

    /**
     * See GradPulse::GetGradient
     */
    virtual double GetGradient (double const time);

    /**
     * @brief Allows non-equdistant setting of ADCs via Shape attribute (GiNaC formula)
     * @see Pulse::SetTPOIs()
     */
    virtual void    SetTPOIs  ();

    /**
     * @brief Set the pulse which generates the eddy currents
     */
    void SetGenPulse  (GradPulse* pulse) {m_gen_pulse=pulse;};

    /**
     * @brief Get the pulse which generates the eddy currents
     */
    GradPulse* GetGenPulse() {return m_gen_pulse;};

    /**
     * @brief Get the AtomicSequence in which eddy currents were generated
     */
    AtomicSequence* GetParentAtom() {return m_parent;}

 protected:

    /**
     * Get informations
     *
     * @return Infos for display.
     */
    virtual string         GetInfo        ();

 private:

    double 			m_dt;		      /**< convolution smapling interval*/
    double			m_linger_time;    /**< time of the EC outside the parent atom*/
    double			m_area_gen_pulse; /**< area of the generating gradient pulse */
    int				m_length;         /**< length of the convolution kernel*/
    vector<double>  m_kernel;         /**< @brief Convolution kernel for EC calculation */
    vector<double>  m_eddy;           /**< @brief the eddy current */
    bool			m_prepared;       /**< @brief status whether eddy currents were succesfully prepared */
    GradPulse*		m_gen_pulse;      /**< @brief The pulse which generates the eddy currents */
    AtomicSequence* m_parent;         /**< @brief The parent atom */

};

#endif /*HARDRFPULSE_H_*/
