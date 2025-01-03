/** @file TrajectoryRespiration.h
 * @brief Implementation of TrajectoryRespiration
 *
 *  Created on: Aug 7, 2020
 *      Author: hmhanson, in collaboration with bennyrowland (adapted from dpflug's TrajectoryMotion class)
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

#ifndef JEMRIS_TRAJECTORYRESPIRATION_H
#define JEMRIS_TRAJECTORYRESPIRATION_H

#include "TrajectoryInterface.h"
#include "NDData.h"

class TrajectoryRespiration : public TrajectoryInterface {
public:
    TrajectoryRespiration();
    virtual ~TrajectoryRespiration();

    virtual void GetValueDerived(double time, double *value);

    virtual void LoadFile(string filename);

    double dx;
    size_t xi;
    double dy;
    size_t yi;
    double dz;
    size_t zi;
    size_t xip;
    size_t yip;
    size_t zip;

    double int_part;
    double store_x;
    double store_y;
    double store_z;
    double m_ap_interpolated[3];
    double m_si_interpolated[3];
    double m_model_offset_interpolated[3];

protected:
    vector<double> m_res;
    vector<double> m_offset;
    NDData<double> m_field;
    NDData<double> m_ap;
    NDData<double> m_si;
    NDData<double> m_model_offset;
    NDData<double> m_breathing_trace;
};

#endif //JEMRIS_TRAJECTORYRESPIRATION_H