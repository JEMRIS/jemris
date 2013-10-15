/** @file ExternalRFPulse.h
 *  @brief Implementation of JEMRIS ExternalRFPulse
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
    ExternalRFPulse() :m_scale(1.), m_dname(""), m_dpath("") {};

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
    virtual bool Prepare  (const PrepareMode mode);


    /**
     * @see Pulse::SetTPOIs
     */
    virtual void     SetTPOIs();

    /**
     * @brief Returns a constant Magnitidue for all times.
     *
     * @param time The flip angle as double.
     * @return the Magnitude.
     */
    virtual double   GetMagnitude  (const double time );
    virtual double GetPhase (const double time);
    static double  GetExtPhase (Module* mod, double time ) { return (((ExternalRFPulse*) mod)->GetPhase(time)  ); };


 private:

    /**
     * @brief Get informations on this external gradient
     *
     * @return RFPulse::GetInfo plus the name of the binary file
     */
    string           GetInfo      ();

    string m_fname;                /**< @brief Filename containing the RF amps and phases  */
    string m_dname;
    string m_dpath;

    vector<double> m_times;       /**< @brief My sample time points                       */
    vector<double> m_magnitudes;  /**< @brief My corresponding amplitudes                 */
    vector<double> m_phases;      /**< @brief Vector of phases                            */

    double m_scale;                /**< @brief Scaling factor for the amps                 */
};

#endif /*HARDRFPULSE_H_*/
