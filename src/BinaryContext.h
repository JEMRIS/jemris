/** @file BinaryContext.h
 *  @brief Implementation of JEMRIS Sample
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

#ifndef BINARY_CONTEXT_H_
#define BINARY_CONTEXT_H_

#include "Declarations.h"

#include "BinaryIO.h"
#include "SimpleIO.h"
#include "HDF5IO.h"

#include <vector>

class BinaryContext {
	
public:

	/**
	 * @brief        Construct populating strategies
	 */
	BinaryContext    () {

		m_strategies.push_back ((BinaryIO*) new SimpleIO ());
		m_strategies.push_back ((BinaryIO*) new HDF5IO   ());

		m_strategy = m_strategies.at(1);

		m_status = IO::OK;
		
	};


	/**
	 * @brief        Default desctructor
	 */
	~BinaryContext   () {};
	

	/**
	 * @brief        Initialise and check access status
	 *
	 * @param  fname File name
	 * @param  mode  Access mode (i.e. read/write?)
	 * @return       Status
	 */
	inline const     IO::Status 
	Initialize       (std::string fname, IO::Mode mode) {
		
		return m_strategy->Initialize(fname, mode);
		
	};
	

	/**
	 * @brief        Read data from file to appropriately sized container
	 *
	 * @param  in    Input container
	 * @return       Status
	 */
	inline const     IO::Status
    ReadData         (double* in)  {
		
		return m_strategy->ReadData(in); 
		
	};
	

	/**
	 * @brief        Write data from container to file 
	 *
	 * @param  out   Output container
	 * @return       Status
	 */
	inline const     IO::Status
	WriteData        (double* out)  {
		
		return m_strategy->WriteData(out); 
		
	};


	/**
	 * @brief        Set DataInfo structure
	 *
	 * @param  info  Incoming DataInfo
	 */
	inline const     void
	SetInfo          (DataInfo info)  {
		
		m_strategy->SetInfo(info); 
		
	};


	/**
	 * @brief        Get information of dataset
	 *
	 * @param  dname Dataset
	 *
	 * @return       Dataset information
	 */
	inline const     DataInfo
	GetInfo          (std::string dname = "")  {
		
		return m_strategy->GetInfo(dname); 
		
	};


	/**
	 * @brief        Get last status
	 *
	 * @return       Status
	 */
	inline const     IO::Status
	Status           ()              {
		
		return m_strategy->Status();
		
	}
	


private:
	
	std::vector<BinaryIO*> m_strategies; /**< List of available strategies */
	BinaryIO*              m_strategy;   /**< Concrete strategy            */
	IO::Status             m_status;     /**< Status                       */
	
};

#endif // BINARY_CONTEXT_H
