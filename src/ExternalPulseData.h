/** @file ExternalPulseData.h
 *  @brief Implementation of JEMRIS ExternalPulseData
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

#ifndef _EXTERNALPULSEDATA_H
#define _EXTERNALPULSEDATA_H

#include <Pulse.h>

/**
 * @brief Read and retrieve data of external pulse shapes
 */

class ExternalPulseData {

 friend class ExternalGradPulse;
 friend class ExternalRFPulse;

 private:
    /**
     * @brief Default destructor.
     */
    ~ExternalPulseData              () {};

    /**
     * @brief constructor
     */
    ExternalPulseData               ();
  
    /**
     * @brief set the pulse owning this 
     */
    void     SetPulse(Pulse* p){m_pulse=p;};

    /**
     * @brief get the data
     *
     * @param time
     * @return data (Gradient value or RF magnitude)
     */
    double   GetData (double const time)  ;

    /**
     * @brief Read the pulse shape data
     *
     * @param fname name of binary file
     */
    bool     ReadPulseShape(string fname, bool verbose);

    /**
     * @see Pulse::SetTPOIs
     */
    void     SetTPOIs();

    static double GetPhase (Module* mod, double time );

    Pulse* m_pulse;               /**<@brief My pulse*/
    string m_fname;               /**< @brief Filename containing the RF amps and phases  */
    vector<double> m_times;       /**< @brief My sample time points                       */
    vector<double> m_magnitudes;  /**< @brief My corresponding amplitudes                 */
    vector<double> m_phases;      /**< @brief Vector of phases                            */
    double	m_phase;			  /**< @brief current phase                            */
};

#endif /*_EXTERNALPULSEDATA_H*/
