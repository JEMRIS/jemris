/** @file AnalyticGradPulse.h
 *  @brief Implementation of JEMRIS AnalyticGradPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#ifndef ANALYTICGRADPULSE_H_
#define ANALYTICGRADPULSE_H_

#include "GradPulse.h"
#include "AnalyticPulseShape.h"

/**
 * @brief Gradient with analytic shape
 */
class AnalyticGradPulse : public GradPulse {


public:

    /**
     * @brief Default constructor
     */
    AnalyticGradPulse() {};

    /**
     * @brief Default copy constructor.
     */
    AnalyticGradPulse  (const AnalyticGradPulse&);

    /**
     * @brief Default destructor.
     */
    ~AnalyticGradPulse () {};

    /**
     * See Module::clone
     */
    inline AnalyticGradPulse* Clone() const {return (new AnalyticGradPulse(*this));};

    /**
     * See Module::Prepare
     */
    virtual bool        Prepare  (const PrepareMode mode);

    /**
     * See GradPulse::GetValue
     */
    virtual double GetGradient (double const time) {return m_pulse_shape.GetShape(time); };

    /**
     * @see Pulse::SetTPOIs
     */
    //virtual void     SetTPOIs() { m_pulse_shape.SetTPOIs(); } ;


 protected:

     /**
     * @brief Get informations on this analytic gradient
     *
     * @return GradPulse::GetInfo plus info of the Shape
     */
    virtual string          GetInfo        ();

    AnalyticPulseShape	m_pulse_shape; /**<computes the pulse shape*/

};

#endif /*PE_TRAPGRADPULSE_H_*/
