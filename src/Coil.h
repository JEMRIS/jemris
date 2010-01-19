/** @file Coil.h
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

#ifndef COIL_H_
#define COIL_H_

#include "Prototype.h"
#include "Signal.h"
#include "Declarations.h"
#include "var_array++.h"

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
     * @brief Get the sensitivity at point (x,y,z)
     *
	 * @return          Sensitivity with respect to spin in World
     */
    double  GetSensitivity () {return this->GetSensitivity(m_world->Values);}

    /**
     * @brief Get the sensitivity at point (x,y,z)
     *
     * @param position  At position.
     * @return          Sensitivity with respect to spin in World.
     */
    virtual double  GetSensitivity (double* position) = 0;

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
     * @brief Get the recieved signal of this coil
     *
     * @return          The recieved signal
     */
    Signal* GetSignal       () { return m_signal; }

    /**
     *  See Prototype::Clone
     */
    Coil* Clone() const = 0;

    /**
     * @brief Dump sensitivity map
     *
     * @param  fname    Filename for sensitivity map dump.
     */
    void DumpSensMap     (string fname) ;

    /**
     * @brief Prepare coil with given attributes.
     *
     * @param mode Sets the preparation mode, one of enum PrepareMode {PREP_INIT,PREP_VERBOSE,PREP_UPDATE}.
     */
    virtual bool Prepare  (PrepareMode mode);

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

 protected:

    /**
     * Constructor
     */
    Coil() {};

    vaArray_3d(double) m_sens_map;
    double             m_position[3];   /**< My location          */
    double             m_direction[3];  /**< My location          */
    Signal*            m_signal;        /**< My signal repository */
    int                m_channel;       /**< My channel no        */
    unsigned short     m_mode;          /**< My mode (RX/TX)      */
    int                m_matrx_res[3];  /**< My sensitivity map matrix  resolution */
    double             m_space_res[3];  /**< My sensitivity map spatial resolution */
    double             m_volume[3];     /**< My volume (X,Y,Z)                     */
    bool               m_has_sens_map;  /**< compute sensitivity map               */

};

#endif /*COIL_H_*/
