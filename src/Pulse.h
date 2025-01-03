	/** @file Pulse.h
 *  @brief Implementation of JEMRIS Pulse
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

#ifndef PULSE_H_
#define PULSE_H_

#include "Module.h"
#include "Parameters.h"
#include "TxRxPhase.h"
#include "Event.h"

class AtomicSequence;
class AnalyticPulseShape;
class ExternalPulseData;

//! Pulse Super Class. ABC for all RF and gradient pulses

class Pulse :public Module, public TxRxPhase {

 friend class AnalyticPulseShape;
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
    Pulse                  (const Pulse&) :
    	m_adc(0), m_initial_delay(0), m_axis(AXIS_VOID), m_adc_flag(1) {};

    /**
     * See Module::GetValue
     */
    virtual void GetValue  (double * dAllVal, double const time) const {};

    /**
     *  @brief  see Module::Prepare()
     */
    virtual bool Prepare   (const PrepareMode mode) ;

    /**
     * @brief Set the duration of this pulse.
     *
     * Set the duration of this pulse to the given double value.
     * @param val double as stated above.
     */
    void SetDuration       (double val) ;

    /**
     * See Module::GetDuration
     */
    double          GetDuration    () { Notify(m_duration); return m_duration; };

    /**
     * @brief Set the Axis of propagation of this pulse.
     *
     * Set the Axis of propagation of this pulse to the given PulseAxis.
     * @param eAxis the axis as stated above.
     */
    inline void SetAxis           (const PulseAxis eAxis)  { m_axis = eAxis; };

    /**
     * @brief Get the Axis of propagation of this pulse.
     *
     * Get the Axis of propagation of this pulse to the given PulseAxis.
     * @return Axis of propagation.
     */
    inline PulseAxis GetAxis      ()   const              { return m_axis; };


    /**
     * @brief Set time points of interest.
     *
     * Must be overloaded by non-constant pulse shapes!
     * Further, the function needs to be called every
     * time the PulseShape changes its timing!!!
     */
    virtual void  SetTPOIs ();

    /**
     * @brief Generate an event to run on scanner hardware
     *
     * Must be overloaded by pulse shapes. New events are allocated on the heap
     * and pointers are stored in the events vector.
     *
     * @param events vector to store new events
     */
    virtual void  GenerateEvents (std::vector<Event*> &events) {};

    /**
     * @return Get number of ADCs
     *
     * @return Number of ADCs
     */
    inline  int  GetNADC  () { return m_adc * (m_adc_flag>0?1:0) ;  };


    /**
     * @brief Set number of ADCs
     *
     * @param  nadc
     */
    inline void  SetNADC  (int nadc) {m_adc = abs(nadc); m_adc_flag = ( nadc<0 ? 0 : m_adc_flag );  SetTPOIs();};

    /**
     * @return Get ADC flag
     *
     * @return ADC flag
     */
    inline  int  GetADCFlag  () { return m_adc_flag;  };

    /**
     * @brief Set ADC flag
     *
     * @param  nadc
     */
    inline void  SetADCFlag  (int adcflag) {m_adc_flag = adcflag; SetTPOIs();};

    /**
     * @brief Get delay in respect of the AtomicSequence holding this pulse.
     *
     * @return Delay in respect to start of the AtomicSequence.
     */
    inline double  GetInitialDelay          () {return m_initial_delay; };

 protected:

    /**
     * Get informations on this Pulse
     *
     * @return Infos
     */
    virtual string          GetInfo        ();

    PulseAxis     m_axis;              /**< The axis of this pulse*/
    int           m_adc;               /**< Number of ADCs */
    int  		   m_adc_flag;          /**< Property of ADCs (see TPOI) */
    double        m_initial_delay;     /**< Time shift at the beginning inside the atom */

};

#endif
