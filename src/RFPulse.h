/** @file RFPulse.h
 *  @brief Implementation of JEMRIS RFPulse
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

#ifndef _RFPULSE_H_
#define _RFPULSE_H_

#include "Pulse.h"
#include "TxRxPhase.h"
#include "CoilArray.h"

//! Super class for all RF pulses

class RFPulse : public Pulse, public TxRxPhase {

 public:

    /**
     * @brief Contructor
     */
    RFPulse          ();

    /**
     * @brief Default destructor
     */
    virtual ~RFPulse () {};

    /**
     * @brief Default copy constructor.
     */
    RFPulse (const RFPulse& rfp) {
    	m_flip_angle = rfp.m_flip_angle;
    	m_bw = rfp.m_bw;
    	m_channel = rfp.m_channel;
    	m_coil_array = rfp.m_coil_array;
    	m_GetPhaseFunPtrs = rfp.m_GetPhaseFunPtrs;
    	m_refocusing = rfp.m_refocusing;
    	m_symmetry = rfp.m_symmetry;
    }

    /**
     *  @brief see Module::GetValue()
     */
    virtual void GetValue (double * dAllVal, double const time)  ;

    /**
     * @brief see Module::Prepare()
     */
    virtual bool Prepare  (const PrepareMode mode);

    /**
     * @brief Get the B1 integral of this pulse
     *
     * @param  steps The number of sampling points of the integral.
     * @return       Numeric integra.
     */
    virtual double  GetIntegralNumeric  (int steps);

    /**
     * @brief  Get the flip angle of this pulse.
     *
     * @return Flip angle.
     */
    inline double  GetFlipAngle  () const {return m_flip_angle;};

    /**
     * @brief       Set the flip angle of this RF pulse.
     *
     * @param fa Flip angle.
     */
    inline void    SetFlipAngle  (const double fa) {m_flip_angle = fa; };

    /**
     * @brief  Get the CoilArray channel of this pulse.
     *
     * @return Flip angle.
     */
    inline int  GetChannel  () const {return m_channel;};

    /**
     * @brief       Set the CoilArray channel of this RF pulse.
     *
     * @param ch    Channel
     */
    inline void    SetChannel  (int ch) {m_channel = ch; };

    /**
     * @brief Returns the Magnitidue of this pulse at a given time.
     *
     * This method needs to be implemented by every derived RF pulse.
     * It is called from RFpulse::GetValue
     *
     * The Magnitude might also be real-valued, in which case the
     * 180 degree phase jumps for negative values are set automatically.
     *
     * @param time Time-point at which the magnitude is requested.
     * @return Magnitude.
     */
    virtual double    GetMagnitude  (const double time )  { return 0.0 ; } ;

    /**
     * @brief  Get the band width of this pulse.
     *
     * @return Band width.
     */
    inline double  GetBandWidth  ()              {return m_bw;};

    /**
     * @brief Add another phase function for this RF pulse.
     *
     * Derived RF pulses should call this function in prepare
     * if they define an additional phase term through a static
     * function, similar to TxRxPhase::GetInitialPhase and
     * TxRxPhase::getLinearPhase
     *
     * @param FGetPhase Function pointer to the static member function
     *                  defining the additional phase term.
     */
    void insertGetPhaseFunction (double (*FGetPhase)(Module*, double));

    /**
     * @brief Set the CoilArray for this RF pulse.
     *
     * @param pCA Pointer to the CoilArray
     */
    inline void    SetCoilArray (CoilArray* pCA) {m_coil_array = pCA;};

    /**
     * @brief Get the CoilArray for this RF pulse.
     *
     * @return Pointer to the CoilArray
     */
    CoilArray*     GetCoilArray () {return m_coil_array;};

    virtual void SetTPOIs ();


 protected:

    /**
     * @brief  Get informations on this Pulse.
     *
     * @return Infos.
     */
    virtual string          GetInfo        ();

    double    m_flip_angle;   /**< @brief Flipangle of this pulse        */
    double    m_bw;           /**< @brief Bandwidth of this pulse        */
    int       m_channel;      /**< @brief Transmit channel of this pulse */
    bool      m_refocusing;   /**< @brief Refocussing pulse? */
    double    m_symmetry;     /**< @brief KSpace symmetry */

    // Provides sensitivies in GetValue
    CoilArray* m_coil_array;  /**< @brief Transmit array in simulation   */

    vector<double (*)(Module*, double)> m_GetPhaseFunPtrs;   /**< @brief GetPhase functions, adding additional Transmit phase terms*/

};

#endif
