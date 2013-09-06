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

//#define VERBOSE

#include <H5Cpp.h>
#include <algorithm>

/**
 * @brief HDF5 IO interface
 */
class HDF5IO : public BinaryIO {

public:
	
	/**
	 * @brief Contructor
	 */
	HDF5IO     () {
		m_type = IO::HDF5;
	}
	
	/**
	 * @brief Destructor
	 */
	virtual ~HDF5IO     ()           {}

	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	virtual IO::Status
	WriteData       (const double* dc);

	/**
	 * @brief     Get information on the data in binary file
	 */
	virtual DataInfo
	GetInfo         (const std::string& dname, const std::string& dpath = "");


	template<class T> IO::Status
	ReadData (std::vector<T>& dv, const std::string& dname, const std::string& dpath) {

		m_info.dname = dname;
		m_info.dpath = dpath;

		try {

#ifndef VERBOSE
			H5::Exception::dontPrint();
#endif

//#ifdef VERBOSE
			std::cout << "Opening " << m_info << " for reading"    << std::endl;
//#endif

			H5::H5File      file (m_info.fname, H5F_ACC_RDONLY);
			H5::DataSet     dataset = file.openDataSet(m_info.URI());
			H5::DataType    type    = dataset.getFloatType();
			H5::DataSpace   space   = dataset.getSpace();
			std::vector<hsize_t> dims ((size_t)space.getSimpleExtentNdims());
			m_info.dims.resize(space.getSimpleExtentDims(&dims[0], NULL));

			for (int i = 0; i < m_info.dims.size(); i++)
				m_info.dims[i] = dims[i];

			std::reverse (m_info.dims.begin(), m_info.dims.end());

//#ifdef VERBOSE
				std::cout << "        rank: " << m_info.NDim() << ", dimensions: ";
				for (int i = 0; i < m_info.NDim(); i++) {
					std::cout << (unsigned long)(m_info.dims[i]);
					if (i == m_info.NDim() - 1)
						std::cout << " = " << m_info.GetSize() << std::endl;
					else
						std::cout << " x ";
				}
//#endif

			dv.resize(m_info.GetSize());
			dataset.read (&dv[0], type);

			space.close();
			dataset.close();
			file.close();

		} catch (const H5::FileIException&      e) {
			return ReportException (e, IO::HDF5_FILE_I_EXCEPTION);
		} catch (const H5::DataSetIException&   e) {
			return ReportException (e, IO::HDF5_DATASET_I_EXCEPTION);
		} catch (const H5::DataSpaceIException& e) {
			return ReportException (e, IO::HDF5_DATASPACE_I_EXCEPTION);
		} catch (const H5::DataTypeIException&  e) {
			return ReportException (e, IO::HDF5_DATATYPE_I_EXCEPTION);
		}

		return IO::OK;

	}


	virtual IO::Status
	Initialize    (const std::string& fname, const IO::Mode mode) {
		return BinaryIO::Initialize(fname, mode);
	}

private:
	/**
	 * 	@brief Handle HDF5 exceptions
	 *
	 * 	@param e     Exception
	 * 	@param ios   Status
	 */
	const IO::Status
	ReportException (const H5::Exception& e, const IO::Status ios);


};

#endif
