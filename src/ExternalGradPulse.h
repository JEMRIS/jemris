/** @file ExternalGradPulse.h
 *  @brief Implementation of JEMRIS ExternalGradPulse
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#ifndef _EXTERNALGRADPULSE_H
#define _EXTERNALGRADPULSE_H

#include <cmath>
#include "GradPulse.h"

/**
 * @brief External gradient prototype.
 */

class ExternalGradPulse : public GradPulse {

 public:

    /**
     * @brief Default constructor
     */
    ExternalGradPulse               () {};

    /**
     * @brief Copy constructor.
     */
    ExternalGradPulse               (const ExternalGradPulse&)  {  m_gain=1.0; };

    /**
     * @brief Default destructor.
     */
    ~ExternalGradPulse              () {};

    /**
     * @see Prototype::Clone
     */
    inline ExternalGradPulse* Clone () const { return (new ExternalGradPulse(*this)); };

    /**
     * @brief Prepare the external gradient pulse.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool     Prepare      (PrepareMode mode);

    /**
     * @see GradPulse::GetGradient
     */
    virtual double   GetGradient  (double const time);

    /**
     * @see Pulse::SetTPOIs
     */
    virtual void     SetTPOIs      ();


 protected:

    /**
     * @brief Get information on this external gradient.
     *
     * @return Information for display.
     */
    string           GetInfo      ();

    vector<double> m_times;         /**< @brief My sample time points                          */
    vector<double> m_magnitudes;    /**< @brief My corresponding amplitudes                    */
    double  m_gain;                 /**< @brief Amplification gain                             */
    long    m_samples;              /**< @brief Number of samples of this pulse                */
    string  m_fname;                /**< @brief URI of the external gradient file              */

};

#endif /*EXTERNALGRADPULSE_*/
