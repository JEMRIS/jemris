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

    // init m_matrx_res to 1 since vaCreate_3d has problems with dim_i == 0 (write into non-allocated memory)
	m_matrx_res[ZC]=1; m_matrx_res[YC]=1; m_matrx_res[XC]=1;

	m_node    = node;

	string s = StrX(((DOMElement*) node)->getAttribute (StrX("Name").XMLchar() )).std_str() ;

	if (s.empty())
		((DOMElement*) node)->setAttribute(StrX("Name").XMLchar(),node->getNodeName());

	SetName( StrX(node->getNodeName()).std_str() );


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

    m_has_sens_map  = true;


    ofstream fout(fname.c_str() , ios::binary);

    for (int i=0; i<m_matrx_res[ZC]; i++) {
        for (int j=0; j<m_matrx_res[YC]; j++) {
            for (int k=0; k<m_matrx_res[XC]; k++) {

                double* position  = new double[3];

                position [XC] = round(((double)k) * m_space_res[XC] - m_volume[XC]/2);
                position [YC] = round(((double)j) * m_space_res[YC] - m_volume[YC]/2);
                position [ZC] = round(((double)i) * m_space_res[ZC] - m_volume[ZC]/2)+1;

                double sens   = GetSensitivity(position);


                m_sens_map[i][j][k] = sens;
                if (fname != "") fout.write((char*)(&(sens)), sizeof(sens));
                delete [] position;

            }
        }
    }

    fout.close();


}

/**********************************************************/
bool Coil::Prepare  (PrepareMode mode) {

	bool success = false;

   	ATTRIBUTE("Name"   , m_name);
	ATTRIBUTE("XPos"   , m_position [XC]);
	ATTRIBUTE("YPos"   , m_position [YC]);
	ATTRIBUTE("ZPos"   , m_position [ZC]);
	ATTRIBUTE("XDir"   , m_direction[XC]);
    ATTRIBUTE("YDir"   , m_direction[YC]);
    ATTRIBUTE("ZDir"   , m_direction[ZC]);
    ATTRIBUTE("XSize"  , m_volume   [XC]);
    ATTRIBUTE("YSize"  , m_volume   [YC]);
    ATTRIBUTE("ZSize"  , m_volume   [ZC]);
    ATTRIBUTE("XMatrix", m_matrx_res[XC]);
    ATTRIBUTE("YMatrix", m_matrx_res[YC]);
    ATTRIBUTE("ZMatrix", m_matrx_res[ZC]);
    ATTRIBUTE("Channel", m_channel);

    m_mode          = mode;
    m_world         = World::instance();
	m_signal        = NULL;
    m_has_sens_map  = false;

    success         = Prototype::Prepare(mode);

    m_sens_map      = vaCreate_3d(m_matrx_res[ZC], m_matrx_res[YC], m_matrx_res[XC], double, NULL);

    for (int i=0; i<3; i++)
        m_space_res[i] = (double)m_volume[i]/(double)m_matrx_res[i];

	return success;

}
