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

#define VERBOSEN

#include <H5Cpp.h>
#include <algorithm>
#include <string.h>

/**
 * @brief HDF5 IO interface
 */
class HDF5IO : public BinaryIO {

public:
	
	/**
	 * @brief Contructor
	 */
	HDF5IO     (const std::string& fname, const IO::Mode mode) {
		m_fname  = fname;
		m_mode   = mode;
		m_type   = IO::HDF5;
		this->FileAccess();
	}
	
	/**
	 * @brief Destructor
	 */
	virtual ~HDF5IO     ()           {
		m_file.close();
	}

	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	template<class T> IO::Status
	Write (const NDData<T>& data, const std::string& urn, const std::string& url = "") {

		try {

#ifndef VERBOSE
			H5::Exception::dontPrint();
#endif

			std::vector<hsize_t> dims(data.NDim());
			for (int i = 0; i < data.NDim(); i++)
				dims[i] = data.Dim(i);

			//std::reverse(dims.begin(),dims.end());

			H5::Group group;

			try {
				group = m_file.openGroup(url);
#ifdef VERBOSE
				printf ("Group %s opened for writing\n", url.c_str()) ;
#endif
			} catch (const H5::Exception& e) {
				group = CreateGroup (url);
			}

			H5::DataSpace dspace (data.NDim(), dims.data());
			H5::FloatType dtype  (H5::PredType::NATIVE_DOUBLE);
			H5::DataSet   dset = group.createDataSet(urn, dtype, dspace);

			dset.write(data.Ptr(), dtype);
			dset.close();
			dspace.close();
			group.close();

		} catch (const H5::FileIException&      e) {
			return ReportException (e, IO::HDF5_FILE_I_EXCEPTION);
		} catch (const H5::GroupIException&     e) {
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



	template<class T> IO::Status
	Read (NDData<T>& data, const std::string& urn, const std::string& url = "") {

		try {

#ifndef VERBOSE
			H5::Exception::dontPrint();
#endif
			H5::DataSet   dset   = m_file.openDataSet(URI(url,urn));
			H5::DataType  dtype  = dset.getFloatType();
			H5::DataSpace dspace = dset.getSpace();
			std::vector<hsize_t> dims (dspace.getSimpleExtentNdims());
			size_t        ndim   = dspace.getSimpleExtentDims(&dims[0], NULL);
			data                 = NDData<T> (dims);

			dset.read(data.Ptr(), dtype);
			dspace.close();
			dset.close();

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
	FileAccess    () {

		if (m_status != IO::OK)
			return m_status;

		if (m_mode == IO::IN) {
			m_file = H5::H5File (m_fname, H5F_ACC_RDONLY);
#ifdef VERBOSEN
			printf ("\nFile %s opened for RO\n", m_fname.c_str());
#endif
		} else if (m_mode == IO::OUT) {

			try {
				m_file = H5::H5File  (m_fname, H5F_ACC_TRUNC);
#ifdef VERBOSEN
				printf ("\nFile %s opened for RW\n", m_fname.c_str());
#endif
			} catch (const H5::Exception& e) {
				m_file = H5::H5File  (m_fname, H5F_ACC_RDWR);
#ifdef VERBOSEN
				printf ("\nFile %s created for RW\n", m_fname.c_str());
#endif
			}
		} else if (m_mode == IO::APPEND) {
			try  {
				m_file = H5::H5File  (m_fname, H5F_ACC_RDWR);
#ifdef VERBOSEN
				printf ("\nFile %s created for RW\n", m_fname.c_str());
#endif

			} catch (const H5::Exception& e) {
				printf ("\nOops!\n");
			}
		}

		return m_status;

	}


private:

	H5::Group CreateGroup (const std::string& url);


	/**
	 * 	@brief Handle HDF5 exceptions
	 *
	 * 	@param e     Exception
	 * 	@param ios   Status
	 */
	const IO::Status
	ReportException (const H5::Exception& e, const IO::Status ios);

	H5::H5File m_file;


};

#endif
