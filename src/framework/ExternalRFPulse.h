/** @file ExternalRFPulse.h
 *  @brief Implementation of JEMRIS ExternalRFPulse
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

#ifndef EXTERNALRFPULSE_H_
#define EXTERNALRFPULSE_H_

#include "RFPulse.h"

/**
 * @brief RF pulse with external pulse shape
 */
class ExternalRFPulse : public RFPulse {

 public:

    /**
     * @brief Default constructor
     */
    ExternalRFPulse() {};

    /**
     * @brief Copy constructor.
     */
    ExternalRFPulse (const ExternalRFPulse&);

    /**
     * @brief Default destructor.
     */
    ~ExternalRFPulse () {};

    /**
     *  See Module::clone
     */
    inline ExternalRFPulse* Clone() const {return (new ExternalRFPulse(*this));};

    /**
     * @brief Prepare the hard RF pulse.
     *
     * @param  mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return      Success.
     */
    virtual bool Prepare  (PrepareMode mode);

    /**
     * @brief Returns a constant Magnitidue for all times.
     *
     * @param time The flip angle as double.
     * @return the Magnitude.
     */
    virtual double   GetMagnitude  (double time );

    /**
     * @brief Set my time points of interest
     */
    virtual void     SetTPOIs () ;

 protected:

    /**
     * @brief Get informations on this external gradient
     *
     * @return RFPulse::GetInfo plus the name of the binary file
     */
    string           GetInfo      ();

    string m_fname;                /**< @brief Filename containing the RF amps and phases  */
    double m_scale;                /**< @brief Scaling factor for the amps                 */
    double m_external_phase;       /**< @brief External phase                              */
    vector<double> m_times;        /**< @brief Vector of Times correlated to the RF events */
    vector<double> m_magnitudes;   /**< @brief Vector of magnitudes                        */
    vector<double> m_phases;       /**< @brief Vector of phases                            */
};

#endif /*HARDRFPULSE_H_*/
