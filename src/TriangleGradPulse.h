/** @file TriangleGradPulse.h
 *  @brief Implementation of TriangleGradPulse
 *
 * Author: tstoecker
 * Date  : Apr 6, 2009
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


#ifndef TRIANGLEGRADPULSE_H_
#define TRIANGLEGRADPULSE_H_

#include "GradPulse.h"

enum TriangleType {TRIANGLE_UP, TRIANGLE_DN};
/**
 * @brief Gradient with constant amplitude
 */
class TriangleGradPulse : public GradPulse {


public:

    /**
     * @brief Default constructor
     */
	TriangleGradPulse() : m_amplitude(0.), m_triangle_type(TRIANGLE_UP) {};

    /**
     * @brief Default copy constructor.
     */
	TriangleGradPulse  (const TriangleGradPulse&);

    /**
     * @brief Default destructor.
     */
    ~TriangleGradPulse () {};

    /**
     * See Module::clone
     */
    inline TriangleGradPulse* Clone() const {return (new TriangleGradPulse(*this));};

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
     * @brief Get informations on this triangle gradient
     *
     * @return GradPulse::GetInfo plus info of the Shape
     */
    virtual string          GetInfo        ();

    double			m_amplitude;       /**< My amplitude      */
    TriangleType	m_triangle_type;   /**< My type (TRIANGLE_UP or TRIANGLE_DN) */

};


#endif /* TRIANGLEGRADPULSE_H_ */
