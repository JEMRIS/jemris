/** @file TrajectoryRespiration.cpp
 * @brief Implementation of TrajectoryRespiration
 *
 *  Created on: Aug 7, 2020
 *      Author: hmhanson, in collaboration with bennyrowland (adapted from dpflug's TrajectoryMotion class)
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2019  Tony Stoecker
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

#include "TrajectoryRespiration.h"

#include "BinaryContext.h"
#include <cmath>

TrajectoryRespiration:: TrajectoryRespiration() {
}

TrajectoryRespiration::~TrajectoryRespiration() {
}

void TrajectoryRespiration::LoadFile(string filename) {

    // file is in HDF5 format
    BinaryContext bc(filename, IO::IN);
    if (bc.Status() != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }

    if (bc.Read(m_field, "resolution", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }
    m_res = m_field.Data();

    if (bc.Read(m_field, "offset", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }
    m_offset = m_field.Data();

    if (bc.Read(m_ap, "ap", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }



    if (bc.Read(m_si, "si", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }


    if (bc.Read(m_model_offset, "model_offset", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }

    if (bc.Read(m_breathing_trace, "breathing_trace", "/model") != IO::OK) {
        cout << bc.Status();
        exit(-1);
    }


    // Samples automatically have their positions offset by half their size to
    // centre them around (0, 0, 0), so we do the same to the field
    for (size_t i = 0; i < 3; i++) {
        m_offset[i] -= 0.5 * (m_ap.Dim(i + 1) - 1) * m_res[i + 1];
    }

    // this is necessary to fool the TrajectoryInterface into calling
    // GetValueDerived() as it requires at least one time in the list
    m_time.push_back(100);

}



void TrajectoryRespiration::GetValueDerived(double time, double *value) {

    bool same_spin = true;
    if (store_x != value[0]) {same_spin = false;}
    if (store_y != value[1]) {same_spin = false;}
    if (store_z != value[2]) {same_spin = false;}


    if (same_spin == false) {


        // calculate motion model indices
        dx = modf((value[0] - m_offset[0]) / m_res[1], &int_part);
        xi = min(max(size_t(int_part), size_t(0)), m_ap.Dim(1));
        dy = modf((value[1] - m_offset[1]) / m_res[2], &int_part);
        yi = min(max(size_t(int_part), size_t(0)), m_ap.Dim(2));
        dz = modf((value[2] - m_offset[2]) / m_res[3], &int_part);
        zi = min(max(size_t(int_part), size_t(0)), m_ap.Dim(3));


        // also calculate indices plus one for the interpolation
        xip = min(xi + 1, m_ap.Dim(1) - 1);
        yip = min(yi + 1, m_ap.Dim(2) - 1);
        zip = min(zi + 1, m_ap.Dim(3) - 1);


        // interpolate motion model AP, SI and offset fields around the spin
        for (size_t i=0; i < 3; i++) {
            m_ap_interpolated[i] = (1 - dx) * (1 - dy) * (1 - dz) * m_ap(i, xi, yi, zi) +
                                   (dx) * (1 - dy) * (1 - dz) * m_ap(i, xip, yi, zi) +
                                   (1 - dx) * (dy) * (1 - dz) * m_ap(i, xi, yip, zi) +
                                   (1 - dx) * (1 - dy) * (dz) * m_ap(i, xi, yi, zip) +
                                   (dx) * (dy) * (1 - dz) * m_ap(i, xip, yip, zi) +
                                   (dx) * (1 - dy) * (dz) * m_ap(i, xip, yi, zip) +
                                   (1 - dx) * (dy) * (dz) * m_ap(i, xi, yip, zip) +
                                   (dx) * (dy) * (dz) * m_ap(i, xip, yip, zip);

        }



        for (size_t i=0; i < 3; i++) {
            m_si_interpolated[i] = (1 - dx) * (1 - dy) * (1 - dz) * m_si(i, xi, yi, zi) +
                                   (dx) * (1 - dy) * (1 - dz) * m_si(i, xip, yi, zi) +
                                   (1 - dx) * (dy) * (1 - dz) * m_si(i, xi, yip, zi) +
                                   (1 - dx) * (1 - dy) * (dz) * m_si(i, xi, yi, zip) +
                                   (dx) * (dy) * (1 - dz) * m_si(i, xip, yip, zi) +
                                   (dx) * (1 - dy) * (dz) * m_si(i, xip, yi, zip) +
                                   (1 - dx) * (dy) * (dz) * m_si(i, xi, yip, zip) +
                                   (dx) * (dy) * (dz) * m_si(i, xip, yip, zip) ;
        }



        for (size_t i=0; i < 3; i++) {
            m_model_offset_interpolated[i] = (1 - dx) * (1 - dy) * (1 - dz) * m_model_offset(i, xi, yi, zi) +
                                             (dx) * (1 - dy) * (1 - dz) * m_model_offset(i, xip, yi, zi) +
                                             (1 - dx) * (dy) * (1 - dz) * m_model_offset(i, xi, yip, zi) +
                                             (1 - dx) * (1 - dy) * (dz) * m_model_offset(i, xi, yi, zip) +
                                             (dx) * (dy) * (1 - dz) * m_model_offset(i, xip, yip, zi) +
                                             (dx) * (1 - dy) * (dz) * m_model_offset(i, xip, yi, zip) +
                                             (1 - dx) * (dy) * (dz) * m_model_offset(i, xi, yip, zip) +
                                             (dx) * (dy) * (dz) * m_model_offset(i, xip, yip, zip);

        }


    }


    // time is cyclical so we calculate mod total duration
    double dt = modf(time / m_res[0], &int_part);
    size_t ti = int(int_part) % m_breathing_trace.Dim(1);
    size_t tip = (ti + 1) % m_breathing_trace.Dim(1);

    // interpolate surrogate signal AP and SI for current time
    double ap = m_breathing_trace(0,ti) * (1-dt) + m_breathing_trace(0,tip) * (dt);
    double si = m_breathing_trace(1,ti) * (1-dt) + m_breathing_trace(1,tip) * (dt);

    // store spin co-ordinates for next iteration
    store_x = value[0];
    store_y = value[1];
    store_z = value[2];

    // calculate spin displacement
    for (size_t i=0; i < 3; i++) {
        value[i] += ap * m_ap_interpolated[i] + si * m_si_interpolated[i] + m_model_offset_interpolated[i];
    }

}