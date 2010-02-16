/** @file Coil.cpp
 *  @brief Implementation of JEMRIS Coil
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

#include "Coil.h"
#include "Model.h"

/**********************************************************/
Coil::~Coil() {
	if (m_signal!=NULL) delete m_signal;
}

/**********************************************************/
void Coil::Initialize (DOMNode* node) {

	m_node    = node;

	string s = StrX(((DOMElement*) node)->getAttribute (StrX("Name").XMLchar() )).std_str() ;

	if (s.empty()) {
		((DOMElement*) node)->setAttribute(StrX("Name").XMLchar(),node->getNodeName());
		SetName( StrX(node->getNodeName()).std_str() );
	}


}

/**********************************************************/
void Coil::InitSignal(long lADCs) {

    if (m_signal!=NULL)
        delete m_signal;

    m_signal = new Signal(lADCs);

}

/**********************************************************/
void Coil::Receive (long lADC) {
//  normalization by TotalSpinnumber moved to 'Signal->DumpTo'

    m_signal->repository.tp[lADC]  = m_world->time;

    m_signal->repository.mx[lADC] +=  GetSensitivity()
        * m_world->solution[AMPL]* cos (- m_world->phase + m_world->solution[PHASE]);

    m_signal->repository.my[lADC] +=  GetSensitivity()
		* m_world->solution[AMPL]* sin (- m_world->phase + m_world->solution[PHASE]);

    m_signal->repository.mz[lADC] += GetSensitivity()
		* m_world->solution[ZC];

}

/**********************************************************/
void Coil::DumpSensMap (string fname) {

    ofstream fout(fname.c_str() , ios::binary);
    double* position  = new double[3];

    for (int k=0; k< (m_dim==3?m_points:1); k++) {

        position [ZC] = (m_dim==3?k*m_extent/m_points-m_extent/2:0.0);

        for (int j=0; j<m_points; j++) {

            position [YC] = j*m_extent/m_points-m_extent/2;

            for (int i=0; i<m_points; i++) {

                position [XC] = i*m_extent/m_points-m_extent/2;
                double sens   = GetSensitivity(position);
                m_sens_map[i][j][k] = sens;
                if (fname != "") fout.write((char*)(&(sens)), sizeof(sens));
				//cout << "i: " << i << ", j: " << j << ", k: " << k << " --- " << m_sens_map[i][j][k] << endl;
            }
        }
    }
    delete [] position;
    fout.close();

}

/**********************************************************/
double  Coil::GetSensitivity () {

	if (m_interpolate) {
		return m_scale*InterpolateSensitivity(m_world->Values);
	}
	else {
		return m_scale*GetSensitivity(m_world->Values);
	}
}

/**********************************************************/
double Coil::InterpolateSensitivity (double* position){

	// expects  -m_extent/2 <= position[j] <= m_extent/2
    double x = (position[XC]+m_extent/2)*m_points/m_extent;
    double y = (position[YC]+m_extent/2)*m_points/m_extent;
    double z = (m_dim==3?(position[ZC]+m_extent/2)*m_points/m_extent:0.0);
	int    px   = int(x),  py   = int(y), pz   = int(z);
	double normx = x-px ,	normy = y-py ,	normz = z-pz;

	//check if point is on lattice
	if (px>m_points-1 || px<0 || py>m_points-1 || py<0 || pz>m_points-1 || pz<0 ) return 0.0;

    //bilinear interpolation (2D)
	int nx = (px+1<m_points?px+1:m_points-1);
	int ny = (py+1<m_points?py+1:m_points-1);
	double i11 = m_sens_map[px][py][pz]+(m_sens_map[px][ny][pz]-m_sens_map[px][py][pz])*normy;
	double i21 = m_sens_map[nx][py][pz]+(m_sens_map[nx][ny][pz]-m_sens_map[nx][py][pz])*normy;
	double iz1 = i11+(i21-i11)*normx;

	//check 2D
	if (m_dim<3) return iz1;

    //trilinear interpolation (3D)
	int nz = (pz+1<m_points?pz+1:m_points-1);
	double i12 = m_sens_map[px][py][nz]+(m_sens_map[px][ny][nz]-m_sens_map[px][py][nz])*normy;
	double i22 = m_sens_map[nx][py][nz]+(m_sens_map[nx][ny][nz]-m_sens_map[nx][py][nz])*normy;
	double iz2 = i12+(i22-i12)*normx;

	return (iz1+(iz2-iz1)*normz);

}

/**********************************************************/
bool Coil::Prepare  (PrepareMode mode) {

	bool success = false;
	m_azimuth = 0.0;
	m_polar   = 0.0;
	m_scale   = 1.0;
	m_phase   = 0.0;
	m_dim     = 3;
	m_extent  = 0;
	m_points  = 0;

	ATTRIBUTE("XPos"   , m_position [XC]);
	ATTRIBUTE("YPos"   , m_position [YC]);
	ATTRIBUTE("ZPos"   , m_position [ZC]);
	ATTRIBUTE("Azimuth", m_azimuth      );
    ATTRIBUTE("Polar"  , m_polar        );
    ATTRIBUTE("Scale"  , m_scale        );
    ATTRIBUTE("Phase"  , m_phase        );
    ATTRIBUTE("Dim"    , m_dim          );
    ATTRIBUTE("Extent" , m_extent       );
    ATTRIBUTE("Points" , m_points       );

    m_mode          = mode;
    m_world         = World::instance();
	m_signal        = NULL;

    success         = Prototype::Prepare(mode);

    m_polar   *= PI/180.0;
    m_azimuth *= PI/180.0;
    m_interpolate = (m_points>0 && m_extent>0.0);

   	m_sens_map = vaCreate_3d(m_points, m_points, (m_dim==3?m_points:1), double, NULL);

	return success;

}
