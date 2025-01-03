/** @file AnalyticCoil.h
 *  @brief Implementation of AnalyticCoil
 *
 * Author: tstoecker
 * Date  : Jan 20, 2010
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


#ifndef ANALYTICCOIL_H_
#define ANALYTICCOIL_H_


#include "Coil.h"

/**
 * @brief Prototype of a Biot-Savart loop
 */
class AnalyticCoil : public Coil {

 public:


        /**
         * @brief constructor
         */
        AnalyticCoil  () {
        	m_use_grid      = true;
        	m_analytic		= false;
        	m_sensitivity	= 0.0;
        	m_analytic_phase= 0.0;
        	m_px			= 0.0;
        	m_py			= 0.0;
        	m_pz			= 0.0;
        };

        /**
         * @brief Default destructor
         */
        ~AnalyticCoil () {};

        /**
         * @brief  Request sensitivity at given position
         *
         * @param  position Position.
         * @return          Sensitivity at requested position.
         */
        virtual double GetSensitivity (const double* position);

        virtual double GetPhase (const double* position){ return m_analytic_phase;};

        /**
         * @brief Clone.
         *
         * @return A clone.
         */
        inline virtual AnalyticCoil* Clone () const {return (new AnalyticCoil(*this));};

    /**
     * @brief Prepare.
     *
     * @param  mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     * @return      Success
     */
    virtual bool Prepare (const PrepareMode mode);


 private:

	bool   m_use_grid;    	/**< @brief compute sensitivities on a grid for later use */
    bool   m_analytic;    	/**< @brief Check, whether analytic evaluation is already prepared */
    double m_sensitivity;	/**< @brief Sensitivity of the AnalyticCoil as calculated by GiNaC */
    double m_analytic_phase;/**< @brief Phase of the AnalyticCoil as calculated by GiNaC */
    double m_px;          	/**< @brief x-position to compute sensitivity*/
    double m_py;          	/**< @brief y-position to compute sensitivity*/
    double m_pz;          	/**< @brief z-position to compute sensitivity*/

};



#endif /* ANALYTICCOIL_H_ */
