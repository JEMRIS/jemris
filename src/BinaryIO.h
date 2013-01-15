/** @file BinaryIO.h
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

#ifndef BINARY_IO_H_
#define BINARY_IO_H_

#include "Declarations.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>


/**
 * @brief  Simple information structure.
 *         Might show to be too unflexible and be eventually replaced by a dom node?
 */
struct DataInfo {

	std::string  fname; /**< File name */
	std::string  dname; /**< Data name (i.e. sample, sensitivities, signals and sequence timing)*/
	std::string  path;  /**< Path (i.e. Group name in HDF5) not used for SimpleIO */

	int          ndim;  /**< # of dimensions */
	int          dims[256];  /**< dimensions */
	int          size;  /**< size of each cell (i.e. 8 for doubles etc) */

	IO::Mode     mode;  /**< 0: Read, 1: Write */

	long GetSize () {

		long size = 0;

		if (ndim > 0) {
			size = 1;
			for (unsigned short i = 0; i < ndim; i++)
				size *= dims[i];
		}

		return size;

	}

	int* Dims() {
		return dims;
	}

	int NDim() {
		return ndim;
	}

};


/**
 * @brief Base class for binary IO strategies
 */
class BinaryIO {
	
public:
	

	/**
	 * @brief Contructor
	 */
	BinaryIO     ()                         {

		m_info.fname = "";
		m_info.ndim  = 0;
		for (int i=0; i<256; i++)
			m_info.dims[i]  = 0;
		m_info.size  = sizeof(double);

	};
	

	/**
	 * @brief Destructor
	 */
	virtual ~BinaryIO     ()                        {};
	 

	/**
	 * @brief Get file name
	 *
	 * @return  File name
	 */
	inline const std::string 
	GetFileName   ()                       { 
		
		return m_info.fname; 
		
	};
	

	/**
	 * @brief        Set file name
	 *
	 * @param  fname File name
	 */
	inline const     
	void             SetFileName      (const std::string fname) { 
		
		m_info.fname = fname;
		
	};
	

	/**
	 * @brief        Read data from file to container
	 *
	 * @param  dc    Data container
	 */
	virtual const
	IO::Status       ReadData         (double* dc)           = 0;
	

	/**
	 * @brief        Write data from container to file
	 *
	 * @param  dc    Data container
	 */
	virtual const
	IO::Status       WriteData        (double* dc)           = 0;
	

	/**
	 * @brief        Get information on the dataset in binary file
	 *
	 * @param dname  Dataset name
	 */
	virtual inline const 
	DataInfo         GetInfo          (std::string dname = "") {
		
		m_info.dname = dname; 
		return m_info;

	};
	

	/**
	 * @brief        Get information on the data in binary file
	 */
	inline const     
	void             SetInfo          (DataInfo info)          {

		m_info = info;

	};


	/**
	 * @brief        File access?
	 */
	inline const  
	IO::Status       FileAccess    () {

		if (m_info.fname.length() > 0) {
			
					if (m_info.mode == IO::IN) {
		
						std::ifstream in (m_info.fname.c_str(), std::ios::binary);
						m_status = (in) ? IO::OK : IO::FILE_NOT_FOUND;

					} else {
						
						std::ofstream out (m_info.fname.c_str() , std::ios::binary);
						m_status = (out) ? IO::OK : IO::INSUFFICIENT_PRIVILEGES;

					}
			
		} else
			m_status = IO::EMPTY_FILE_NAME;

		return m_status;

	};


	/**
	 * @brief     Initialize file access. Look for existence if reading. Check permissions.
	 */
	inline const  
	IO::Status    Initialize    (std::string fname, IO::Mode mode) {
 
		m_info.fname = fname;
		m_info.mode  = mode;
		
		return FileAccess ();

	};


	/**
	 * @brief     Get last status
	 */
	inline const  
	IO::Status    Status        () {

		return m_status;

	};
	
protected: 
	
	DataInfo       m_info;
	IO::Status     m_status;
	
};

#endif
