/** @file Pulse.h
 *  @brief Implementation of JEMRIS Pulse
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

#ifndef PULSE_H_
#define PULSE_H_

#define PI 3.14159265358979

#include "Module.h"
#include "Parameters.h"
#include "TxRxPhase.h"

class AtomicSequence;
class ExternalPulseData;

//! Pulse Super Class. ABC for all RF and gradient pulses

class Pulse :public Module {

 friend class ExternalPulseData;

 public:

    /**
     * @brief Construct and initialise some values.
     */
    Pulse ();

    /**
     * @brief Default destructor.
     */
    virtual ~Pulse         () {};

    /**
     * @brief Copy constructor.
     */
    Pulse                  (const Pulse&)     {};

    /**
     * See Module::GetValue
     */
    virtual void GetValue  (double * dAllVal, double const time) {};

    /**
     *  @brief  see Module::Prepare()
     */
    virtual bool Prepare   (PrepareMode mode) ;

    /**
     *  @brief  Does the Preparation of the "Shape" Attribute for
     *  derived analytic Pulses.
     *
     *  This is a bit ugly: Instead for the derived classes Pulse<-RfPulse<-AnalyticRfPulse and
     *  Pulse<-GradientPulse<-AnalyticGradientPulse a multiple inherited base class AnalyticPulse
     *  would be nice.
     *
     * @param mode the PrepareMode
     * @return success/failure of operation
     */
    bool PrepareAnalytic   (PrepareMode mode) ;

    /**
     * @brief Get the duration of this pulse.
     *
     * @return Duration
     */
    inline double GetDuration    ()                  { return m_duration; };

    /**
     * @brief Set the duration of this pulse.
     *
     * Set the duration of this pulse to the given double value.
     * @param val double as stated above.
     */
    void SetDuration       (double val) ;

    /**
     * @brief Set the Axis of propagation of this pulse.
     *
     * Set the Axis of propagation of this pulse to the given PulseAxis.
     * @param eAxis the axis as stated above.
     */
    inline void SetAxis           (PulseAxis eAxis)  { m_axis = eAxis; };

    /**
     * @brief Get the Axis of propagation of this pulse.
     *
     * Get the Axis of propagation of this pulse to the given PulseAxis.
     * @return Axis of propagation.
     */
    inline PulseAxis GetAxis      ()                 { return m_axis; };


    /**
     * @brief Set time points of interest.
     *
     * Must be overloaded by non-constant pulse shapes!
     * Further, the function needs to be called every
     * time the PulseShape changes its timing!!!
     */
    virtual void  SetTPOIs ();

    /**
     * @return Get number of ADCs
     *
     * @return Number of ADCs
     */
    inline unsigned int  GetNADC  () { return m_adc;  };

;

    /**
     * @brief Set number of ADCs
     *
     * @param  nadc
     * @return      The private member m_ADCs.
     */
    inline void  SetNADC          (unsigned int nadc) {m_adc = nadc; SetTPOIs();};

    /**
     * @brief Get delay in respect of the AtomicSequence holding this pulse.
     *
     * @return Delay in respect to start of the AtomicSequence.
     */
    inline double  GetInitialDelay          () {return m_initial_delay; };

	/**
	 * @brief Set phase lock.
	 *
	 * @param val Phase lock.
	 */
    inline void  SetPhaseLock     (bool val)         {m_phase_lock = val; };

	/**
	 * @brief Get phase lock.
	 *
	 * @return Phase lock.
	 */
    inline bool  GetPhaseLock     ()                 {return m_phase_lock; };


 protected:

    /**
     * Get informations on this Pulse
     *
     * @return Infos
     */
    virtual string          GetInfo        ();

    PulseAxis     m_axis;              /**< The axis of this pulse*/
    unsigned int  m_adc;               /**< Number of ADCs */
    double        m_initial_delay;     /**< Time shift at the beginning inside the atom */
    bool          m_phase_lock;        /**< Lock phase of ADCs to the phase of the last RF pulse event*/

    unsigned int  m_more_tpois;        /**< For analytic evaluation of GetValue */
    bool          m_analytic;          /**< For analytic evaluation of GetValue */
    double        m_analytic_value;    /**< For analytic evaluation of GetValue */
    double        m_analytic_time;     /**< For analytic evaluation of GetValue */
    double        m_constant[20];      /**< For analytic evaluation of GetValue */
    double        m_analytic_integral; /**< For analytic evaluation of GetValue */
};

#endif
