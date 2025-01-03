/** @file SimpleIO.h
 *  @brief Implementation of JEMRIS Sample
 */

/*
 * JEMRIS Copyright (C)
 *                        2006-2025  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#ifndef SIMPLE_IO_H_
#define SIMPLE_IO_H_

#include "BinaryIO.h"

class SimpleIO : public BinaryIO {
	
public:

	/**
	 * @brief Contructor
	 */
	SimpleIO     (const std::string& fname, const IO::Mode mode) :
		BinaryIO::BinaryIO (fname, mode) {
		m_type = IO::SIMPLE;
	}
	
	/**
	 * @brief Destructor
	 */
	~SimpleIO     ()                     {}
	
	template<class T> IO::Status
	Read (NDData<T>& data, const std::string& urn,
			const std::string& url = "") {return IO::OK; };

	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	template<class T> IO::Status
	Write (const NDData<T>& data, const std::string& urn,
			const std::string& url = "") { return IO::OK; }


};

#endif
