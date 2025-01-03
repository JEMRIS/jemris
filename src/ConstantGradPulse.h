/** @file ConstantGradPulse.h
 *  @brief Implementation of JEMRIS ConstantGradPulse
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

#ifndef CONSTANTGRADPULSE_H_
#define CONSTANTGRADPULSE_H_

#include "GradPulse.h"

/**
 * @brief Gradient with constant amplitude
 */
class ConstantGradPulse : public GradPulse {


public:

    /**
     * @brief Default constructor
     */
    ConstantGradPulse() : m_amplitude(0) {};

    /**
     * @brief Default copy constructor.
     */
    ConstantGradPulse  (const ConstantGradPulse&);

    /**
     * @brief Default destructor.
     */
    ~ConstantGradPulse () {};

    /**
     * See Module::clone
     */
    inline ConstantGradPulse* Clone() const {return (new ConstantGradPulse(*this));};

    /**
     * See Module::Prepare
     */
    virtual bool        Prepare  (const PrepareMode mode);

    /**
     * See GradPulse::GetValue
     */
    virtual double GetGradient (double const time);

 protected:

     /**
     * @brief Get informations on this analytic gradient
     *
     * @return GradPulse::GetInfo plus info of the Shape
     */
    virtual string          GetInfo        ();

    double  m_amplitude;       /**< My amplitude      */

};

#endif /*PE_TRAPGRADPULSE_H_*/
