/** @file ExternalCoil.cpp
 *  @brief Implementation of JEMRIS ExternalCoil
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
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

#include "ExternalCoil.h"

/***********************************************************/
ExternalCoil::~ExternalCoil () {
}

/***********************************************************/
ExternalCoil* ExternalCoil::Clone() const {
	return (new ExternalCoil(*this));
}

/***********************************************************/
double ExternalCoil::GetSensitivity(double* position) {

    return InterpolateSensitivity(position);

}

/***********************************************************/
bool ExternalCoil::Prepare(PrepareMode mode) {

    bool success = true;
    double temp  = 0.0;

    ATTRIBUTE("Filename" , m_fname);

    Coil::Prepare(mode);


    ifstream fin(m_fname.c_str(), ios::binary);

    for (int k=0; k< (m_dim==3?m_points:1); k++) {
		for (int j=0; j<m_points; j++){
            for (int i=0; i<m_points; i++) {
                fin.read((char *)(&temp), sizeof(double));
                m_sens_map[i][j][k] = temp;
				//cout << "i: " << i << ", j: " << j << ", k: " << k << " --- " << m_sens_map[i][j][k] << endl;
            }
		}
	}

    fin.close();

    return success;

}

