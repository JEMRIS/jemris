/** @file HDF5IO.h
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

#ifndef HDF5_IO_H_
#define HDF5_IO_H_

#include "BinaryIO.h"

#include <H5Cpp.h>

/**
 * @brief HDF5 IO interface
 */
class HDF5IO : public BinaryIO {

public:
	
	/**
	 * @brief Contructor
	 */
	HDF5IO     ()                      {};
	
	/**
	 * @brief Destructor
	 */
	virtual ~HDF5IO     ()                     {};
	
	/**
	 * @brief     Read data from file to container
	 *
	 * @param  dc Data container
	 */
	virtual inline const IO::Status
	ReadData        (double* dc);
	
	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	virtual inline const IO::Status
	WriteData       (const double* dc);

	/**
	 * @brief     Get information on the data in binary file
	 */
	virtual inline const DataInfo
	GetInfo         (const std::string dname);


private:

	/**
	 * @brief       Read data from file to container
	 *
	 * @param  dc   Data container
	 * @param  read Actually Read (true) or just test existence (false)?
	 */
	virtual const IO::Status
	ReadData        (double* dc, bool read);
	

	/**
	 * 	@brief Handle HDF5 exceptions
	 *
	 * 	@param e     Exception
	 * 	@param ios   Status
	 */
	const IO::Status
	ReportException (const H5::Exception e, const IO::Status ios);


};

#endif
