/** @file BinaryIO.h
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

#ifndef BINARY_IO_H_
#define BINARY_IO_H_

#include "Declarations.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

const static std::string SLASH ("/");
const static std::string DSLASH ("//");

template<class T> inline static T _prod (const T a, const T b) {return (a*b);}
template<class T> inline static T prod (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)1, _prod<T>);
}
template<class T> inline static T sum (const std::vector<T>& v) {
	return std::accumulate(v.begin(), v.end(), (T)0);
}

/**
 * @brief  Simple information structure.
 *         Might show to be too unflexible and be eventually replaced by a dom node?
 */
struct DataInfo {

	std::string  fname; /**< File name */
	std::string  dname; /**< Data name (i.e. sample, sensitivities, signals and sequence timing)*/
	std::string  dpath;  /**< Path (i.e. Group name in HDF5) not used for SimpleIO */

	std::vector<size_t> dims;  /**< dimensions */
	int          size;  /**< size of each cell (i.e. 8 for doubles etc) */

	IO::Mode     mode;  /**< 0: Read, 1: Write */

	size_t GetSize () {
		return prod(dims);
	}

	size_t* Dims() {
		return dims.data();
	}

	int NDim() {
		return dims.size();
	}

	std::string DataPath () const {
		return dpath;
	}

	std::string DataName () const {
		return dname;
	}

	std::string FileName () const {
		return fname;
	}

	std::ostream& Print (std::ostream& os) {
		os << fname << " " << this->URI().c_str();
		return os;
	}

	std::string URI () {
		std::string uri (dpath + "/" + dname);
		size_t pos = uri.find(DSLASH);
		while (pos != std::string::npos) {
			uri.replace (pos, 2, SLASH);
			pos = uri.find(DSLASH);
		}
		return uri;
	}

};

inline std::ostream& operator<< (std::ostream& os, DataInfo& di) {
	return di.Print(os);
}


/**
 * @brief Base class for binary IO strategies
 */
class BinaryIO {
	
public:
	

	/**
	 * @brief Contructor
	 */
	BinaryIO     ()  : m_status(IO::OK), m_type(IO::NONE) {

		m_info.fname = "";
		m_info.size  = sizeof(double);

	}
	

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
	}
	

	IO::Strategy IOStrategy () const {
		return m_type;
	}

	/**
	 * @brief        Set file name
	 *
	 * @param  fname File name
	 */
	inline const     
	void             SetFileName      (const std::string fname) {
		m_info.fname = fname;
	}
	

	/**
	 * @brief        Read data from file to container
	 *
	 * @param  dc    Data container
	 */
	template<class T>
	IO::Status       ReadData (std::vector<T>& dv, const std::string& dname, const std::string& dpath) {
		std::cout << "Oh Oh: You are wrong here!" << std::endl;
	}
	
	/**
	 * @brief        Write data from container to file
	 *
	 * @param  dc    Data container
	 */

	virtual IO::Status       WriteData        (const double* dc)           = 0;
	

	/**
	 * @brief        Get information on the dataset in binary file
	 *
	 * @param dname  Dataset name
	 */
	virtual	DataInfo         GetInfo          (const std::string&, const std::string&) = 0;
	

	/**
	 * @brief        Get information on the data in binary file
	 */
	void             SetInfo          (DataInfo info)          {
		m_info = info;
	}


	/**
	 * @brief        File access?
	 */
	inline
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

	}


	/**
	 * @brief     Initialize file access. Look for existence if reading. Check permissions.
	 */
	virtual inline
	IO::Status    Initialize    (const std::string& fname, const IO::Mode mode) {
 
		m_info.fname = fname;
		m_info.mode  = mode;
		
		return FileAccess ();

	}


	/**
	 * @brief     Get last status
	 */
	inline const  
	IO::Status    Status        () {

		return m_status;

	}
	




protected: 
	
	DataInfo       m_info;
	IO::Status     m_status;
	IO::Strategy   m_type;
	
};

#endif
