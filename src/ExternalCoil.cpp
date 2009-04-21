/** @file ExternalCoil.cpp
 *  @brief Implementation of JEMRIS ExternalCoil
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

#include "ExternalCoil.h"

/***********************************************************/
ExternalCoil::~ExternalCoil () {
    delete [] m_smap;
}

/***********************************************************/
ExternalCoil* ExternalCoil::Clone() const {
	return (new ExternalCoil(*this));
}

/***********************************************************/
double ExternalCoil::GetSensitivity(double* position) {

    return m_smap [(int)round(position[ZC]/m_space_res[ZC])+m_matrx_res[ZC]/2-1]
                  [(int)round(position[YC]/m_space_res[YC])+m_matrx_res[YC]/2]
                  [(int)round(position[XC]/m_space_res[XC])+m_matrx_res[XC]/2];

}

/***********************************************************/
bool ExternalCoil::Prepare(PrepareMode mode) {

    bool success = true;
    double temp  = 0.0;

    ATTRIBUTE("URI"       , m_uri);

    Coil::Prepare(mode);

    // dynamically allocate 3-dim array for
    m_smap = (double ***) malloc(m_matrx_res[XC] * m_matrx_res[YC] * m_matrx_res[ZC] * sizeof(double));

    for (int i=0; i<m_matrx_res[ZC]; i++) {
        m_smap[i] = (double **) malloc (m_matrx_res[YC]*m_matrx_res[XC]*sizeof(double));
        for (int j=0; j<m_matrx_res[YC]; j++){
            m_smap[i][j] = (double *) malloc (m_matrx_res[XC]*sizeof(double));
		}
    }

    ifstream fin(m_uri.c_str(), ios::binary);

    for (int i=0; i<m_matrx_res[ZC]; ++i) {
		for (int j=0; j<m_matrx_res[YC]; j++){
            for (int k=0; k<m_matrx_res[XC]; k++) {
                fin.read((char *)(&temp), sizeof(double));
                m_smap[i][j][k] = temp;
				//cout << "i: " << i << ", j: " << j << ", k: " << k << " --- " << m_smap[i][j][k] << endl;
            }
		}
	}

    fin.close();

    return success;

}

