/** @file Coil.h
 *  @brief Implementation of JEMRIS Coil
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

#ifndef COIL_H_
#define COIL_H_

#include "Prototype.h"
#include "Signal.h"
#include "Declarations.h"
#include "NDData.h"

/**
 * @brief Base class of coil objects
 */

class Coil : public Prototype {

 public:

    /**
     * @brief Destructor
     */
    virtual ~Coil          ();

    /**
     * @brief Get the B1+ magnitude at point (x,y,z) of the current spin
     *
	 * @return          Sensitivity with respect to spin in World
     */
    double  GetSensitivity (const double time=0) ;

    /**
     * @brief Get the B1+ phase at point (x,y,z) of the current spin
     *
	 * @return          Phase with respect to spin in World
     */
    double  GetPhase (const double time=0) ;

    /**
     * @brief Interpolate the sensitivity at point (x,y,z)
     *
	 * @return          Interpolated Sensitivity
     */
    double InterpolateSensitivity (const double* position, bool magnitude=true);

    /**
     * @brief Get the B1+ magnitude at point (x,y,z)
     *
     * This method must be implemented by every derived coil.
     *
     * @param position  At position.
     * @return          Sensitivity with respect to spin in World.
     */
    virtual double  GetSensitivity (const double* position) = 0;

    /**
     * @brief Get the B1+ phase at point (x,y,z)
     *
     * This method may be implemented by every derived coil. Otherwise phase is zero.
     *
     * Important: the phase of Coils needs to be implemented with unit radians!
     * (In contrast to the phase of RF pulses which has units degrees.)
     *
     * @param position  At position.
     * @return          B1+ phase with respect to spin in World.
     */
    virtual double  GetPhase (const double* position) {return 0.0;};

    /**
     * @brief Initialize my signal repository
     *
     * @param lADCs     Number of ADCs
     */
    void    InitSignal     (long lADCs);

    /**
     * @brief Receive signal from World
     *
     * @param lADC      Receive the signal for this particular ADC
     */
    void    Receive        (long lADC);

    /**
     * @brief Transmit signal.
     */
    void    Transmit        ();

    /**
     * @brief Get the received signal of this coil
     *
     * @return          The received signal
     */
    Signal* GetSignal       () { return m_signal; }

    /**
     *  See Prototype::Clone
     */
    Coil* Clone() const = 0;

    /**
     * @brief Dump sensitivity map on the XML defined grid
     */
	void GridMap     ();

    /**
     * @brief Map magnitudes
     *
     * @return Magnitudes
     */
	double* MagnitudeMap     ();

    /**
     * @brief Map phases
     *
     * @return phases
     */
	double* PhaseMap     ();

    /**
     * @brief Prepare coil with given attributes.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool Prepare  (const PrepareMode mode);

    /**
     * @brief Initialize this prototype.
     *
     * The first step after cloning!
     * The method sets the Name of the Prototype, and pointers to
     * the referring node and the (unique) SequenceTree.
     *
     * @param node The DOMNode referring to this prototype.
     */
    void Initialize  (DOMNode* node );

	int GetPoints ();

	int GetExtent ();

	unsigned GetNDim ();

	double GetNorm (){return m_norm;};

 protected:

    /**
     * Constructor
     */
    Coil() {};

    double		m_position[3];	/**< Center location   */
    Signal*		m_signal;    	/**< Signal repository */
    unsigned		m_mode;      	/**< My mode (RX/TX)      */
    double			m_azimuth; 		/**< Change of coordinate system: azimuth angle*/
    double			m_polar;   		/**< Change of coordinate system: polar angle*/
    double			m_scale;   		/**< Scaling factor for sensitivities */
    double			m_norm;   		/**< Normalization factor for sensitivities */
    double			m_phase;   		/**< Constant phase shift */
    bool            m_interpolate;  /**< Whether to precompute sensitivities in an array */
    bool			m_complex;		/**< True, if sensitivity map is complex (non-zero phase entries).*/
    bool		    m_conjugate;	/**< Complex conjugate the sensitivites, if true.*/
    unsigned		m_dim;     		/**< Dimensions (2D or 3D) of the array*/
    double			m_extent;  		/**< Array extend of support region [mm] */
    int				m_points;  		/**< Sampling points of the array */

    NDData<double>  m_sensmag;
    NDData<double>  m_senspha;

    double Unwrap(double diff,bool magnitude); /**< helper function to check for phase wraps in interpolation of phase maps. */

};

#endif /*COIL_H_*/
