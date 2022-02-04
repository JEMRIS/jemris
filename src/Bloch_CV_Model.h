/** @file Bloch_CV_Model.h
 *  @brief Implementation of JEMRIS Blo_CV_Model.h
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2022  Tony Stoecker
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

#ifndef BLOCH_CV_MODEL_H_
#define BLOCH_CV_MODEL_H_

#include "Model.h"
#include "config.h"

//CVODE2.5 includes:
#ifdef HAVE_CVODE_CVODE_H
    #include "cvode/cvode.h"
#endif
#ifdef HAVE_CVODE_H
    #include "cvode.h"
#endif
#ifdef HAVE_NVECTOR_NVECTOR_SERIAL_H
    #include "nvector/nvector_serial.h"
#endif
#ifdef HAVE_NVECTOR_SERIAL_H
    #include "nvector_serial.h"
#endif




#define NEQ   3                   // number of equations
#define RTOL  1e-6                // scalar relative tolerance
#define ATOL1 1e-8                // vector absolute tolerance components
#define ATOL2 1e-8
#define ATOL3 1e-8
#define BEPS  1e-10

//! Structure keeping the vectors for cvode
struct nvec {
    N_Vector y;      /**< CVODE vector */
    N_Vector abstol; /**< CVODE vector */
};

/**
 * @brief Numerical solving of Bloch equations
 * As an application of the CVODE solver
 * by Lawrence Livermore National Laboratory - Livermore, CA
 * http://www.llnl.gov/CASC/sundials
 */

//! MR model solver using CVODE
class Bloch_CV_Model : public Model {

 public:

    /**
     * @brief Default destructor
     */
    virtual ~Bloch_CV_Model      () {
    	CVodeFree(&m_cvode_mem);
    };

    /**
     * @brief Constructor
     */
    Bloch_CV_Model               ();


 protected:

    /**
     * @brief Initialise solver
     *
     * Inistalise N_Vector and attach it to my world
     */
    virtual void InitSolver      ();

    /**
     * @brief Free solver
     *
     * Release the N_Vector
     */
    virtual void FreeSolver      ();

    /**
     * @brief Summery output
     *
     * More elaborate description here please
     */
    void         PrintFinalStats ();


    /**
     *  see Model::Calculate()
     */
    virtual bool Calculate       (double next_tStop);

 private:

    // CVODE related
    void*  m_cvode_mem;	 /**< @brief pointer to cvode malloc */
    double m_tpoint;	 /**< @brief current time point */
    double m_reltol;	 /**< @brief relative error tolerance for CVODE */

};

#endif /*BLOCH_CV_MODEL_H_*/
