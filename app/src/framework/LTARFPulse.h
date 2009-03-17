/** @file LTARFPulse.h
 *  @brief Implementation of JEMRIS LTARFPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#ifndef LTARFPULSE_H_
#define LTARFPULSE_H_

#include "RFPulse.h"

//! Prototype for hard radio frequency pulses.

class LTARFPulse : public RFPulse {


 public:

    /**
     * @brief Default constructor
     */
    LTARFPulse() {};

    /**
     * @brief Default copy constructor.
     */
    LTARFPulse  (const LTARFPulse&) {};

    /**
     * @brief Default destructor.
     */
    ~LTARFPulse () {};

    /**
     *  @See Prototype::Clone
     */
    inline LTARFPulse* Clone() const { return (new LTARFPulse(*this)); };

     /**
     * @brief Prepare.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return     Success.
     */
    virtual bool Prepare  (PrepareMode mode);

    /**
     * Returns a constant Magnitude for all times.
     *
     * @param time The flip angle as double.
     * @return     Magnitude.
     */
    inline virtual double  GetMagnitude (double time );

 protected:
	virtual string GetInfo();


 private:

    double m_Gamma;
    double m_Alpha;
	double m_A;
	double m_Beta;
	int    m_N;



};

#endif /*HARDRFPULSE_H_*/
