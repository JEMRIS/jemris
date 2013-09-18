/** @file GradPulse.h
 *  @brief Implementation of JEMRIS GradPulse
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

#ifndef _GRADPULSE_H_
#define _GRADPULSE_H_

#include "Pulse.h"

//! Base class and prototype for all gradient pulses

class GradPulse : public Pulse {

 public:

    /**
     * Public Contructor
     */
    GradPulse          ();

    /**
     * Public Destructor
     */
    virtual ~GradPulse ();

    /**
     * Copy constructor.
     */
    GradPulse (const GradPulse&);

    // @brief see Module::GetValue()
    virtual void GetValue (double * dAllVal, double const time);

    /**
     * @brief Get the value of the gradient pulse at time t.
     *
     * This function has to be implemented for every gradient pulse.
     *
     * @param time  The time at which the gradient value is requested.
     * @return      The gradient value.
     */
    virtual double GetGradient (double const time){return 0.0; };

    // @brief see Module::Prepare
    virtual bool Prepare  (const PrepareMode mode);

    /**
     * @brief Preparation of Nonlinear Gradient fields
     *
     * @param mode for preparation
     */
    virtual bool PrepareNLGfield  (const PrepareMode mode);

    /**
     * @brief get the area of this gradient pulse by numerical integration.
     *
     * @param steps The number of steps for integration.
     * @return The gradient area.
     */
    double GetAreaNumeric (int steps);

    /**
     * @brief Set the area of this gradient pulse.
     *
     * @param val Gradient area.
     */
    void SetArea (double val);

    /**
     * @brief Set the nonlinear gradient field term World::NonLinGradField
     *
     * The method gets the current spin-position from the World, calculates
     * the nonlinear gradient field, and writes it to the World.
     *
     * @param time  The time at which the gradient value is requested.
     */
    void SetNonLinGradField(double const time);

    /**
     * @brief Check, whether this gradient is nonlinear
     *
     * @return True, if a nonlinear gradient
     */
    bool          HasNonLinGrad(){return m_non_lin_grad; };

 protected:
    /**
     * Get informations on this Gradient
     *
     * @return Pulse::GetInfo plus The area of this gradient
     */
    virtual string          GetInfo        ();

   double m_slew_rate;       /**< @brief The slewrate of this gradient pulse */
   double m_max_ampl;        /**< @brief The maximum amplitude of this gradient pulse */
   double m_area;            /**< @brief The area of the gradient pulse*/

   bool   m_non_lin_grad;    /**< @brief A flag for nonlinear gradients */
   double m_nlg_field;       /**< @brief Nonlinear gradient field (NLG)*/
   double m_nlg_px;          /**< @brief x-position to compute the NLG field*/
   double m_nlg_py;          /**< @brief y-position to compute the NLG field*/
   double m_nlg_pz;          /**< @brief z-position to compute the NLG field*/
   double m_nlg_val;         /**< @brief gradient value to compute the NLG field*/

};

#endif
