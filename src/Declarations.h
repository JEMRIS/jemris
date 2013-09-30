/** @file Declarations.h
 *  @brief Implementation of JEMRIS Declarations
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

#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#include <string>

#define PI 3.14159265358979
const double TIME_ERR_TOL = 1e-6; // Avoid CVODE warnings. Doesn't affect physics.

/**
 * number of spin properties:
 */
//const int NO_SPIN_PROPERTIES = 10;

/**
 * Spin fields
 */
enum fields {
    XC=0,               /**< @brief X location                */
    YC,                 /**< @brief Y location                */
    ZC,                 /**< @brief Z location                */
    M0,                 /**< @brief Equilibrium magnetisation */
    R1,                 /**< @brief Relaxation rate 1: 1/T1   */
    R2,                 /**< @brief Relaxation rate 2: 1/T2   */
    R2S,                /**< @brief Relaxation rate 2: 1/T2S   */
    DB,                 /**< @brief Delta B (e.g. chemical-shift + susceptiblity-induced, ...) */
    ID,				    /**< @brief Index of spin			  */
    NO_SPIN_PROPERTIES  /**< @brief counts number of entries to enum fields. must stay 
						            last element, other entries to 'fields' may not be assigned 
									to a value!*/
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
    TX            /**< Transmit */
};

/**
 * MPI tags
 */
enum MpiTag {
	REQUEST_SPINS=66,
	SEND_NO_SPINS,
	SEND_SAMPLE,
	SPINS_PROGRESS,
	SIG_TP,
	SIG_MX,
	SIG_MY,
	SIG_MZ
};


namespace IO {

	/**
	 * Binary io strategies
	 */
	enum Strategy {
		SIMPLE,        /**< limited standard serialised data storage */
		HDF5,          /**< Hirarchical data format 5                */
		CDF,           /**< Common data format                       */
		IOSTRATEGIES   /**< Leave this as last entry                 */
	};
	
	/**
	 * Binary io stati
	 */
	enum Status {
		OK,                          /**< OK                                          */
		EMPTY_FILE_NAME,             /**< Error: Empty file name                      */
		FILE_NOT_FOUND,              /**< Error: File not found for reading           */ 
		INSUFFICIENT_PRIVILEGES,     /**< Error: Insufficient privileges or disk full */
		HDF5_FILE_I_EXCEPTION,       /**< Error: HDF5 general                         */
		HDF5_DATASET_I_EXCEPTION,    /**< Error: HDF5 dataset                         */
		HDF5_DATASPACE_I_EXCEPTION,  /**< Error: HDF5 dataspace                       */
		HDF5_DATATYPE_I_EXCEPTION,    /**< Error: HDF5 datatype                        */
		UNMATCHED_DIMENSIONS,
		LAST_STATUS_ENTRY
	};

	static std::string StatusMessage[LAST_STATUS_ENTRY]  = {
		std::string("IO notice: OK."),
		std::string("IO error: Empty file name."),
		std::string("IO error: File not found."),
		std::string("IO error: Insufficient privileges."),
		std::string("IO error: HDF5, general file excption."),
		std::string("IO error: HDF5, dataset exception."),
		std::string("IO error: HDF5, dataspace exception."),
		std::string("IO error: HDF5, datatype exception.")
	};
		
	
	/**
	 * Binary io modes
	 */
	enum Mode {
		IN, /**< Read access */
		OUT /**< R/W  access */
	};
	
}

namespace Sim {
	
	/*enum Result {
		OK,
		NAN,
		DIVISION_BY_ZERO
		};*/
	
}

#endif /*DECLARATIONS_H_*/
