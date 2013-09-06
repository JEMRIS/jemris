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
template<class T>
struct Data {

	std::string  dname; /**< Data name (i.e. sample, sensitivities, signals and sequence timing)*/
	std::string  dpath;  /**< Path (i.e. Group name in HDF5) not used for SimpleIO */

	std::vector<size_t> dims;  /**< dimensions */
	std::vector<T> data;

	int          size;  /**< size of each cell (i.e. 8 for doubles etc) */

	IO::Mode     mode;  /**< 0: Read, 1: Write */

	void Allocate () {
		data.resize(GetSize());
	}

	size_t GetSize () {
		return prod(dims);
	}

	size_t* Dims() {
		return dims.data();
	}

	size_t NDim() const {
		return dims.size();
	}

	std::string DataPath () const {
		return dpath;
	}

	std::string DataName () const {
		return dname;
	}

	std::ostream& Print (std::ostream& os) {
		os << this->URI().c_str();
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

	T& operator[] (const size_t p) {return data[p];}
	T  operator[] (const size_t p) const {return data[p];}

};


template <class T>
inline std::ostream& operator<< (std::ostream& os, Data<T>& di) {
	return di.Print(os);
}


/**
 * @brief Base class for binary IO strategies
 */
class BinaryIO {
	
public:
	

	BinaryIO () : m_status(IO::OK), m_type(IO::NONE), m_fname (""), m_mode (IO::IN) {}

	/**
	 * @brief Contructor
	 */
	BinaryIO     (const std::string& fname, const IO::Mode mode) :
		m_status(IO::OK), m_type(IO::NONE), m_fname (fname), m_mode (mode) {

		FileAccess ();

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
		return m_fname;
	}
	

	IO::Strategy
	IOStrategy () const {
		return m_type;
	}


	/**
	 * @brief        Read data from file to container
	 *
	 * @param  dc    Data container
	 */
	template<class T> IO::Status
	ReadData (Data<T>& data) {
		std::cout << "Oh Oh: You are wrong here!" << std::endl;
	}
	
	/**
	 * @brief        Write data from container to file
	 *
	 * @param  dc    Data container
	 */
	template<class T> IO::Status
	WriteData (const Data<T>& dc);
	

	/**
	 * @brief        File access?
	 */
	virtual IO::Status
	FileAccess    () {

		if (m_fname.length() > 0) {
			
			if (m_data.mode == IO::IN) {
				std::ifstream in (m_fname.c_str(), std::ios::binary);
				m_status = (in) ? IO::OK : IO::FILE_NOT_FOUND;
			} else {
				std::ofstream out (m_fname.c_str() , std::ios::binary);
				m_status = (out) ? IO::OK : IO::INSUFFICIENT_PRIVILEGES;
			}
			
		} else
			m_status = IO::EMPTY_FILE_NAME;

		return m_status;

	}


	/**
	 * @brief     Get last status
	 */
	inline const  IO::Status
	Status        () {
		return m_status;
	}
	




protected: 

	std::string    m_fname;
	Data<double>   m_data;
	IO::Mode       m_mode;
	IO::Status     m_status;
	IO::Strategy   m_type;
	
};

#endif
