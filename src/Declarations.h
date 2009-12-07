/** @file Declarations.h
 *  @brief Implementation of JEMRIS Declarations
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

#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#define PI 3.14159265358979
const double TIME_ERR_TOL = 1e-6; // Avoid CVODE warnings. Doesn't affect physics.

/**
 * Spin fields
 */
enum fields {
    XC,                 /**< @brief X location                */
    YC,                 /**< @brief Y location                */
    ZC,                 /**< @brief Z location                */
    M0,                 /**< @brief Equilibrium magnetisation */
    R1,                 /**< @brief Relaxation rate 1: 1/T1   */
    R2,                 /**< @brief Relaxation rate 2: 1/T2   */
    R2S,                /**< @brief Relaxation rate 2: 1/T2S   */
    DB,                 /**< @brief Delta B (e.g. chemical-shift + susceptiblity-induced, ...) */
    NN                  /**< @brief Not-Named (unused so far) */
};

/**
 * Cylinder coords for position of coil elements
 */
enum cylinder {
	RC = 3,             /**< @brief Radius  *///!< RC
	RT,                 /**< @brief Heading *///!< RT
	RZ					/**< @brief Azimuth */             //!< RZ
};

/**
 * Polar coordinates
 * No need to define the Z component, since identical to ZC.
 */
enum polar {
	AMPL,               /**< @brief Amplitude */
	PHASE               /**< @brief Phase     */
};

/**
 * Order of pulse axis as returned by
 * the sequence.
 */
enum seqval {
	RF_AMP,             /**< @brief Amplitude */
	RF_PHS,             /**< @brief Phase     */
    GRAD_X,             /**< @brief Readout   */
    GRAD_Y,             /**< @brief Phase     */
    GRAD_Z              /**< @brief Slice     */
};


enum PulseAxis {AXIS_RF, AXIS_GX, AXIS_GY, AXIS_GZ, AXIS_VOID};

/**
 * Coil sensitivity methods
 */
enum SensitivityMethod {
	UNIFORM,			 /**< @brief Uniform idealistic sensitivity map equal 1 */
	ONE_OVER_R_SQUARE,   /**< @brief Idealistic 1/r^2 snsitivity for each coil */
	BIOT_SAVART_LOOP,    /**< @brief Biot-Savart integration for loop           */
	ONE_OVER_R
};

/**
 * Coil modes
 */
enum mode {
    RX,            /**< Receive  */
    TX,            /**< Transmit */
};

/**
 * MPI tags
 */
enum MpiTag {
	REQUEST_SPINS=66,
	SEND_NO_SPINS,
	SEND_SAMPLE,
};

#endif /*DECLARATIONS_H_*/
