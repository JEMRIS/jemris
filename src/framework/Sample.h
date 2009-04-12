/** @file Sample.h
 *  @brief Implementation of JEMRIS Sample
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <stdexcept>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

#include <xercesc/dom/DOM.hpp>

#include "rng.h"

#include "Declarations.h"

using std::string;
using std::ofstream;

using namespace std;
XERCES_CPP_NAMESPACE_USE

/**
 * Binary file type
 */
enum fType  {
    EXPLICIT, /**< Old type (x,y,z) coords as last of 8 values per spin */
    IMPLICIT  /**< New type (x,y,z) ordered double values w/o spatial information in bin file */
};

/**
 * @brief Spin structure
 */

struct Spin_data {
    double x;    /**< x-position of the spin                    */
    double y;    /**< x-position of the spin                    */
    double z;    /**< x-position of the spin                    */
    double m0;   /**< equilibrium magnetisation                 */
    double r1;   /**< longitudinal relaxation rate              */
    double r2;   /**< transverse relaxation rate                */
    double db;   /**< 'Delta B' - any off-resoance at this spin */
    double nn;   /**< not-known (free purpose parameter)        */

};

struct Spin {
    long    size; 		/**< Data size    */
    Spin_data *data;	/**< array of spins */
};



/*
 *  @brief  Persistent MR sample cointaining the spins
 */

class Sample {


 public:

    /**
     * Constructor
     */
    Sample                              ();

    /**
     * Constructor
     *
     * Create a container from binary file
     *
     * @param file Sample binary file
     */
    Sample                              (string file);

    /**
     * Constructor
     *
     * Create a container of requested size initialized with zeros
     *
     * @param size Size of the sample
     */
    Sample                              (long size);

    /**
     * Destructor
     */
    virtual ~Sample                     ();

    /**
     * @brief delete the spin structure
     */
    void ClearSpins ();

    /**
     * @brief create the spin structure
     *
     * @param size Size of the spin structure to create
     */
    void CreateSpins (long size);

    /**
     * Get size of the sample
     *
     * @return Size of the sample
     */
    long    GetSize                     ();

    /**
     * Get a subset of this sample
     *
     * @param n    N-th subset
     * @param size Size of the subset
     */
    Sample* GetSubSample                (int n, long size);

    /**
     * Get values
     *
     * More elaborate description here please
     *
     * @param l More elaborate description here please
     */
    double* GetValues                   (long l) ;

    /**
     * Get resolution
     *
     * More elaborate description here please
     *
     * @param l More elaborate description here please
     */
    double* GetResolution()  { return m_res; };

    /**
     * Initialize the randome number generator
     *
     * @param val any integer
     */
    void    InitRandGenerator   (int val=1) { m_rng = RNG( val * (long ) clock() ); };

    /**
     * Set the random local field fluctuations.
     *
     * @param val R2Prime (Shaping parameter of the Lorentz distribution in kHz).
     */
    void    SetR2Prime   (double val) {m_r2prime = val;};

    /**
     * Set the position randomness in per cent of the cartesian resolution.
     *
     * @param val Percentage width of the Gaussian distribution.
     */
    void    SetPositionRandomness   (double val) {m_pos_rand_perc = val;};

    /**
     * @Brief Get off-resonance of a specific spin.
     *
     * The off-resonance is given by the deterministic value, read by Populate from the binary
     * file, plus the random field fluctuation specified by SetR2Prime.
     *
     * @param pos    Position of the spin. If negative, last acquired spin by GetValues is used.
     * @return       The off-resonance in unit [Khz]
     */
    double  GetDeltaB                   (long pos = -1);

    /**
     *
     * Create a container from binary file
     *
     * @param file Sample binary file
     */
    void Populate (string file);


    Spin_data* GetSpinsData() {return spins.data;};

 private:
    Spin         spins;      /** < My private spins structure          */

    /**
     * Populate sample with given type and binary source
     *
     * @param fin  File input stream to the binary file containing the sample.
     * @param type Type of binary file
     */
    void    Populate                    (ifstream* fin) ;

    double       m_val   [8];  /** < Copy of the spin properties asked for by GetValues */
    long         m_index [3];  /** < Sample dimension       */
    double       m_res   [3];  /** < Sample resolution [mm] */
    double       m_offset[3];  /** < Sample offeset to {0,0,0} origin */

    RNG          m_rng;          /** < random number generator*/
    double       m_r2prime;      /** < R2-Prime == shaping parameter of the Lorentzian distribution */
    double       m_pos_rand_perc;/** < Percantage (of cartesian resolution) randomness in spin position .*/



};

#endif /*SAMPLE_H_*/
