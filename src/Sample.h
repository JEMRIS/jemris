/** @file Sample.h
 *  @brief Implementation of JEMRIS Sample
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
#include "sys/time.h"

class SampleReorderStrategyInterface;
class CoilArray;

using std::string;
using std::ofstream;

using namespace std;
XERCES_CPP_NAMESPACE_USE


/**
 * Defined for memory allocation testing.
 */
static int ens_alloc = 0;


/**
 * @brief Spin ensemble
 */
//template <class T = double>
struct Ensemble {

	std::vector<long> m_dims; /**< Specific dimensions (arbitrary maximum to way too high limit of 256) */
    std::vector<double> m_data;      /**< Actual serialized data (Data is stored column-major)                 */
	long     m_nspins;    /**< Live spins (i.e. Spins with M0 > 0)                                  */

	/**
	 * @brief      Construct
	 */
	Ensemble () : m_nspins(0) {}


	/**
	 * @brief      Destruct
	 */
	~Ensemble () {
		Clear();
	}


	/**
	 * @brief       Number of raw data elements
	 * 
	 * @return      Number of elements in data store
	 */
	inline long Size () const {
		return m_data.size();
	}


	/**
	 * @brief       Number of spins in data store
	 *
	 * @return      Number of spins in data store
	 */
	inline long NSpins () const {
		return m_nspins;
	}


	/**
	 * @brief       Innermost data dimension containing physical parameters of every isochromat
	 *
	 * @return      Number of physical and spatial properties of every spin
	 */
	inline long	NProps () const {
		return m_dims[0];
	}
	

	/**
	 * @brief        Reference to innermost data dimensions
	 *
	 * @return       Reference to number of physical and spatial properties of every spin. 
	 */
	inline long& NProps () {
		return m_dims[0];
	}
	

	/**
	 * @brief       Clear data store and dimensions.
	 */
	inline void Clear () {
		m_nspins = 0;
		m_dims.clear();
		m_data.clear();
	};
	

	/**
	 * @brief       Clear data store and dimensions.
	 */
	inline void ClearData () {
		m_nspins = 0;
		m_dims.clear();
		m_data.clear();
	};
	

	/**
	 * @brief  Initialize data store with dimensions and number of non-zero spins
	 *
	 * @param  ndim Number of dimensions
	 * @param  dims Actual dimensions
	 * @param  live Live (non-zero) spins
	 */ 
	inline void Init (const long ndim, const long* dims, const long live) {

		m_dims.resize(ndim);
		std::copy (dims, dims+ndim, m_dims.begin());

		// Add spatial dimensions 
		m_dims[0] += 4;

		// Set number of live spins (Generally less spins may have M0 > 0)
		m_nspins   = live;

		Allocate();
		Zero();

	}

	
	/**
	 * @brief      Allocate RAM
	 */ 
	inline void	Allocate () {
		m_data.resize(NSpins() * NProps());
	}


	/**
	 * @brief     Zero data 
	 */
	inline void Zero () {
		m_data.assign(m_data.size(), 0.);
	}


	/**
	 * @brief  Initialize data store number of non-zero spins only (MPI slaves)
	 *
	 * @param  live Live (non-zero) spins
	 */ 
	inline void	Init (const long live) {
		m_nspins   = live;
		Allocate();
		Zero();
	}

	/**
	 * @brief  Initialize data store number of non-zero spins only (MPI slaves)
	 *
	 * @param  live Live (non-zero) spins
	 */
	inline void	Init (const long nprops, const long live) {
		m_dims.push_back(nprops);
		m_nspins   = live;
		Allocate();
		Zero();
	}

	/**
	 * @brief       Dimensions
	 *
	 * @return      Dimensions of the ensemble
	 */
	inline const long* Dims () const {
		return m_dims.data();
	}


	/**
	 * @brief       Access to position in store
	 * @param  pos  Desired position 
	 * @return      Reference to pos-th value in the store
	 */
	inline double& operator[] (const long pos) {
		assert(pos >= 0);
		assert(pos <  m_data.size());
		return m_data[pos];
	}

	/**
	 * @brief       Access to position in store
	 * @param  pos  Desired position
	 * @return      Reference to pos-th value in the store
	 */
	inline double operator[] (long pos) const {
		assert(pos >= 0);
		assert(pos <  m_data.size());
		return m_data[pos];
	}

	/**
	 * @brief       Access to data
	 *
	 * @return      Reference to data
	 */
	inline double* Data () {
		return m_data.data();
	}

};


/**
 * @brief a container of spins
 */
struct Spin {
    long    size; 		/**< Data size    */
    Ensemble *data;	/**< array of spins */
};


//! The Sample is the object to simulate. It contains the spins.
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
    Sample                              (const string& file, const int multiple = 1);

    /**
     * Constructor
     *
     * Create a container of requested size initialized with zeros
     *
     * @param size Size of the sample
     */
    Sample                              (const long size);

    /**
     * Destructor
     */
    virtual ~Sample                     ();

    /**
     * init variables which are same for all constructors
     *
     */
    virtual void Prepare (const std::string& fname = "");

    /**
     * @brief delete the spin structure
     */
    void ClearSpins ();

    /**
     * @brief create the spin structure
     *
     * @param size Size of the spin structure to create
     */
    void CreateSpins (const long size);

    /**
     * @brief create the spin structure
     *
     * @param size Size of the spin structure to create
     */
    void CreateSpins (const long nprops, const long size);

    /**
     * Get size of the sample
     *
     * @return Size of the sample
     */
    long    GetSize                     () const;

    /**
     * Get number of spin properties including spatial
     *
     * @return Numnber of spin properties
     */
    inline long  GetNProps () {return m_ensemble.NProps();};

    /**
     * Get number of spin properties including spatial
     *
     * @return Numnber of spin properties
     */
    inline const long* GetSampleDims () const {return m_ensemble.Dims();};

    /**
     * Get a subset of this sample
     *
     * @param n    N-th subset
     * @param size Size of the subset
     */
    Sample* GetSubSample                (const int n, const long size);

    /**
     * @brief Get values for spin l and deliver them in val.
     *
     * @param l   Spin number
     * @param val Output container
     */
    void GetValues                   (const long l, double* val) ;

    /**
     * @brief Get grid resolution
     *
     * @return Grid resolution
     */
    double* GetResolution()  { return m_res; };

    /**
     * @brief Initialize the randome number generator
     *
     * @param val any integer
     */
    void    InitRandGenerator   (int val=1) { m_rng = RNG( val * (long ) clock() ); };

    /**
     * @brief Set the random local field fluctuations.
     *
     * @param val R2Prime (Shaping parameter of the Lorentz distribution in kHz).
     */
    void    SetR2Prime   (double val) {m_r2prime = val;};

    /**
     * @brief Set the position randomness in per cent of the cartesian resolution.
     *
     * @param val Percentage width of the Gaussian distribution.
     */
    void    SetPositionRandomness   (double val) {m_pos_rand_perc = val;};

    /**
     * @brief Get off-resonance of a specific spin.
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
	virtual IO::Status Populate (const string& file);

    /**
     * returns pointer to sample data (needed for MPI send/receive)
     */
    double* GetSpinsData() {return m_ensemble.Data();};

    /**
     * can set a method to reorder the sample (do nothing, shuffle sample,... )
     */
    void SetReorderStrategy(string strat);

    /**
     * executes sample reordering
     */
    void ReorderSample();

    /**
     * @brief Utility function for restart:
     * dump information to restart jemris after crash
     */
    void DumpRestartInfo(CoilArray* RxCA);

    /**
     * @brief Utility function for restart:
     * mark spins which have been calculated.
     */
    void ReportSpin(int beg,int end,int value) {for (int i=beg; i<=end; i++) m_spin_state[i] = value;}

    /**
     * @brief Utility function for restart:
     * Read restart file after crash
     */
    int ReadSpinsState();

    /**
     * @brief Utility function for restart:
     * mark all spins as not simulated.
     */
    void ClearSpinsState();

    /**
     * @brief utility function to send sample in parallel mode:
     * initial samples are send via mpi_scatterv; get needed vectors to scatter data.
     */
    void GetScatterVectors(int *sendcount, int *displ,int size);

    /**
     * @brief utility function to send sample in parallel mode:
     * get next spin packet to be sent. (beginning index + no_spins to send)
     */
    void GetNextPacket(int &noSpins, int &NextSpinToSend, int SlaveId);

    /**
     * Returns No spins which still needs to be calculated.
     */
    int SpinsLeft();

    /**
     * Set Time interval in seconds after which new spins are sent (approx. value.)
     */
    void    SetTimeInterval(double val) {m_sent_interval=val;};

    bool    IsRestart() {return m_is_restart;};

	void    GetHelper (double* target);

	double* GetHelper ();

	long    GetHelperSize ();
	
	// HACK
	int     GetNoSpinCompartments ();
	
	void    SetNoSpinCompartments (int n);
	
	void    CreateHelper (const long l);
	
	void    CopyHelper (double* out);

	virtual void CropEnumerate ();
	

 protected:
	void 	MultiplySample(int multiple);  /** clones sample 'multiple'-times, e.g. for diffusion simulation */

	Ensemble     m_ensemble;


    long         m_index [3];  /** < Sample dimensions      */
    double       m_res   [3];  /** < Sample resolution [mm] */
    double       m_offset[3];  /** < Sample offeset to {0,0,0} origin */

    RNG          m_rng;          /** < random number generator*/
    double       m_r2prime;      /** < R2-Prime == shaping parameter of the Lorentzian distribution */
    double       m_pos_rand_perc;/** < Percantage (of cartesian resolution) randomness in spin position .*/

    SampleReorderStrategyInterface *m_reorder_strategy;

// data for sending sample in parallel mode:
    int m_max_paket_size;		/** maximal no of spins to send */
    int m_min_paket_size;		/** minimal no of spins to send */
    int m_next_spin_to_send;

// bookkeeping for restart:
    vector<char>  	m_spin_state;	/** keeps track whether spin is not touched (==0), sent (==1) or calculated (==2) */
    bool 			m_is_restart;	/** true if simulation run is from a restart */
    vector<int>		m_spins_sent;	/** no of spins last sent to each slave */
    vector<int>		m_last_offset_sent;/** offset to the last spins sent */
    vector<timeval> m_last_time;	/** last timepoint at which spins were sent */
    double			m_sent_interval;/** approx. time in seconds after which new spins are sent */

    double			m_total_cpu_time;
    double			m_no_spins_done;

	double*         m_helper;
	int             m_helper_size;
	int             m_no_spin_compartments;

};

#endif /*SAMPLE_H_*/
