/** @file Parameters.h
 *  @brief Implementation of JEMRIS Parameters
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

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "Module.h"
#include <xercesc/dom/DOMNode.hpp>
#include <cstdlib>

#define PARAM_PI    3.14159265358979

/**
 * @brief World parameters provide the parametric data of the sequence.
 */
class Parameters : public Module {

public:

    /**
     * @brief Default destructor
     */
    ~Parameters() { _instance = 0; };

    /**
     * @brief Get the single instance of the World parameters
     */
    static Parameters* instance ();

    /**
     * @brief Clone
     *
     * Clone the Parameters returns a pointer to the single instance.
     */
    inline Parameters* Clone() const { return instance(); };

;

    /**
     * @see Prototype::Prepare()
     *
     * @param  mode Prepare mode (Here only PREP_INIT)
     * @return      Success
     */
    bool Prepare(PrepareMode mode = PREP_INIT);

    /**
     * @brief Set default parameters.
     *
     * the method is called once during creation of the
     * single instance.
     */
    void SetDefaults();


    static Parameters* _instance;

    // Sequence parameters
    double m_te;             /**< @brief Sequence TE */
    double m_tr;             /**< @brief Sequence TR */
    double m_ti;             /**< @brief Sequence TI */
    double m_td;             /**< @brief Sequence TD */

    double m_fov_x;          /**< @brief Sequence FOV in x direction */
    double m_fov_y;          /**< @brief Sequence FOV in y direction */
    double m_fov_z;          /**< @brief Sequence FOV in z direction */

    unsigned int m_iNx;      /**< @brief Sequence image size in x direction */
    unsigned int m_iNy;      /**< @brief Sequence image size in y direction */
    unsigned int m_iNz;      /**< @brief Sequence image size in z direction */

    double m_delta_x;        /**< @brief Sequence image resolution in x direction */
    double m_delta_y;        /**< @brief Sequence image resolution in y direction */
    double m_delta_z;        /**< @brief Sequence image resolution in z direction */

    double m_kmax_x;         /**< @brief Sequence maximum k-value in x direction */
    double m_kmax_y;         /**< @brief Sequence maximum k-value in y direction */
    double m_kmax_z;         /**< @brief Sequence maximum k-value in z direction */

    double m_delta_kx;       /**< @brief Sequence k-space resolution in x direction */
    double m_delta_ky;       /**< @brief Sequence k-space resolution in y direction */
    double m_delta_kz;       /**< @brief Sequence k-space resolution in z direction */

    //Scanner hardware parameters
    double m_grad_slew_rate; /**< @brief Overall maximum gradient slewrate */
    double m_grad_max_ampl;  /**< @brief Overall maximum gradient amplitude */


 private:

    Parameters() {};	/**< private default constructor */

};

#endif /*PARAMETERS_H_*/
