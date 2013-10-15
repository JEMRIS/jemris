/** @file BinaryContext.h
 *  @brief Implementation of JEMRIS Sample
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stoecker, Kaveh Vahedipour
 *                                  Forschungszentrum Juelich, Germany
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
#include <typeinfo>

//const std::type_info& hdf5_t = typeid(&HDF5IO);
//const std::type_info& simple_t = typeid(&SimpleIO);

class BinaryContext {
	
public:

	/**
	 * @brief        Construct populating strategies
	 */
	BinaryContext    (const std::string& fname, IO::Mode mode);
	virtual ~BinaryContext    ();

	/**
	 * @brief        Write data from container to file 
	 *
	 * @param  out   Output container
	 * @return       Status
	 */
	template<class T> IO::Status
	Write (const NDData<T>& data, const std::string& urn, const std::string& url = "") {
		if (m_strategy->IOStrategy() == IO::HDF5)
			return ((HDF5IO*)m_strategy)->Write(data, urn, url);
		else if (m_strategy->IOStrategy() == IO::SIMPLE)
			return ((SimpleIO*)m_strategy)->Write(data, urn, url);
	}
	template<class T> IO::Status
	Write (const NDData<T>& data, const char* urn, const char* url = "") {
		return Write(data, std::string(urn), std::string(url));
	}


	template<class T> IO::Status
	Read (NDData<T>& data, const std::string& urn, const std::string& url = "") {
		if (m_strategy->IOStrategy() == IO::HDF5)
			return ((HDF5IO*)m_strategy)->Read(data, urn, url);
		else if (m_strategy->IOStrategy() == IO::SIMPLE)
			return ((SimpleIO*)m_strategy)->Read(data, urn, url);
	}
	template<class T> IO::Status
	Read (NDData<T>& data, const char* urn, const char* url = "") {
		return Read (data, std::string(urn), std::string(url));
	}
	/**
	 * @brief        Get last status
	 *
	 * @return       Status
	 */
	IO::Status
	Status           () const;
	


private:
	
	std::vector<BinaryIO*> m_strategies; /**< List of available strategies */
	BinaryIO*              m_strategy;   /**< Concrete strategy            */
	IO::Status             m_status;     /**< Status                       */
	
};

#endif // BINARY_CONTEXT_H
