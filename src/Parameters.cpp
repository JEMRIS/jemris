/** @file Parameters.cpp
 *  @brief Implementation of JEMRIS Parameters
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

#include <iostream>
#include "Parameters.h"
#include <stdlib.h>

using namespace std;

Parameters* Parameters::m_instance = 0;

/**********************************************************/
Parameters* Parameters::instance() {

    if(m_instance == 0) {
        m_instance = new Parameters;
        m_instance->SetDefaults();
    }

    return m_instance;

}

/**********************************************************/
void Parameters::SetDefaults() {

	m_prepared = false;

	//all timing params set to zero
	m_te = m_tr = m_ti = m_td = 0.0;

    //default imaging parameters
	m_fov_x = m_fov_y = 100.0;
	m_iNx   = m_iNy = 64;
	m_fov_z = 0.0;
	m_iNz   = 1;

	//hardware parameters have some useful values:
	m_grad_slew_rate = 1;
	m_grad_max_ampl  = 1;

}

/**********************************************************/
bool    Parameters::Prepare(const PrepareMode mode) {

	m_type     = MOD_VOID;
	m_prepared = true;

	//Parameters, intended for XML declaration in Params and reference in other modules
	ATTRIBUTE("TE",           m_te);
	ATTRIBUTE("TR",           m_tr);
	ATTRIBUTE("TI",           m_ti);
	ATTRIBUTE("TD",           m_td);

	ATTRIBUTE("FOVx",         m_fov_x);
	ATTRIBUTE("FOVy",         m_fov_y);
	ATTRIBUTE("FOVz",         m_fov_z);

	ATTRIBUTE("Nx",           m_iNx);
	ATTRIBUTE("Ny",           m_iNy);
	ATTRIBUTE("Nz",           m_iNz);

	ATTRIBUTE("GradSlewRate", m_grad_slew_rate);
	ATTRIBUTE("GradMaxAmpl" , m_grad_max_ampl);

	//Parameters, intended for reference from other modules only
	m_delta_x = m_fov_x/m_iNx;
	m_delta_y = m_fov_y/m_iNy;
	m_delta_z = m_fov_z/m_iNz;
	HIDDEN_ATTRIBUTE("Dx",    m_delta_x);
	HIDDEN_ATTRIBUTE("Dy",    m_delta_y);
	HIDDEN_ATTRIBUTE("Dz",    m_delta_z);

	m_kmax_x = PI/m_delta_x;
	m_kmax_y = PI/m_delta_y;
	m_kmax_z = PI/m_delta_z;
	HIDDEN_ATTRIBUTE("KMAXx", m_kmax_x);
	HIDDEN_ATTRIBUTE("KMAXy", m_kmax_y);
	HIDDEN_ATTRIBUTE("KMAXz", m_kmax_z);

	m_delta_kx = 2*m_kmax_x/m_iNx;
	m_delta_ky = 2*m_kmax_y/m_iNy;
	m_delta_kz = 2*m_kmax_z/m_iNz;
	HIDDEN_ATTRIBUTE("DKx"    ,m_delta_kx);
	HIDDEN_ATTRIBUTE("DKy"    ,m_delta_ky);
	HIDDEN_ATTRIBUTE("DKz"    ,m_delta_kz);

    // Prepare up the chain
	Module::Prepare(mode);

	//hide XML attributes which were set by Module::Prepare()
	HideAttribute("Observe", false);
	HideAttribute("Vector",  false);
	HideAttribute("Duration",false);

	return m_prepared;

}
