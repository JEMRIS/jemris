/** @file TxRxPhase.h
 *  @brief Implementation of JEMRIS TxRxase.h
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#ifndef TXRXPHASE_H_
#define TXRXPHASE_H_


//declare modules
class Module;

//! Phase physics super class

class TxRxPhase {

public:

    /**
     * @brief Constructor
     */
    TxRxPhase();

    /**
     * @brief Default destructor
     */
    virtual ~TxRxPhase() {};

    /**
     * @brief Get phase of this pulse.
     */
    //virtual double  GetPhase    (double time)  { return 0.0; };

    /**
     * @brief Set phase of this pulse.
     *
     * @param val The phase as double.
     */
    inline void    SetInitialPhase     (double val ) {m_initial_phase = val;};

    /**
     *@brief Get phase lock of this pulse.
     */
    inline bool    GetPhaseLock        ()              {return m_phase_lock;};

    /**
     * @brief Set phase lock of this pulse.
     *
     * @param bval Set phase lock {true|false}.
     */
    inline void    SetPhaseLock        (bool bval)     {m_phase_lock = bval;};


    /**
     * @brief Get initial phase of this pulse.
     */
    inline double  GetInitialPhase      ()              {return m_initial_phase;};

    /**
     * @brief Get constant frequency for linear phase terms.
     *
     * @return  The frequency of the puls.
     */
    inline double  GetFrequency         ()              {return m_frequency;};

    /**
     * @brief Set constant frequency for linear phase terms.
     *
     * @param val  The new frequency of the puls.
     */
    inline void   SetFrequency          (double val)   {  m_frequency = val;};
;

    /**
     * @brief A general purpose linear additive phase term.
     *
     * Rx case: A pointer to this function can be added to the
     * list RFPulse::m_GetPhaseFunPtrs .
     *
     * @param time  The time at which the phase is requested.
     * @param mod   A pointer to a Module.
     * @return      The linear phase term.
     */
    static double getLinearPhase (Module* mod, double time );


 protected:

    double             m_initial_phase;      /**< @brief Initial Phase of this phase event*/
    bool               m_phase_lock;         /**< @brief Phase lock of this phase event*/
    double             m_frequency;          /**< @brief A constant frequency to set linear phase terms*/

};

#endif /*TXRXPHASE_H_*/
