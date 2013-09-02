#include "HDF5IO.h"

#include <typeinfo>

using namespace H5;

//#define VERBOSE

const IO::Status
HDF5IO::WriteData (const double* data) {
	
	try {
		
#ifndef VERBOSE
		Exception::dontPrint();
#endif
		
		hsize_t* dims = new hsize_t [m_info.ndim];
		
		for (int i = 0; i < m_info.ndim; i++)
			dims[i] = m_info.dims[i];
		
		H5File        file; 
		
		try {
			file = H5File  (m_info.fname, H5F_ACC_RDWR);
#ifdef VERBOSE
			printf ("File %s opened for RW\n", m_info.fname.c_str());
#endif
		} catch (Exception e) {
			file = H5File  (m_info.fname, H5F_ACC_TRUNC);
#ifdef VERBOSE
			printf ("File %s created for RW\n", m_info.fname.c_str());
#endif
		}
			
		Group group;

		try {

			group = file.openGroup(m_info.path);
#ifdef VERBOSE
			printf ("Group %s opened for writing\n", m_info.path.c_str()) ;
#endif

		} catch (Exception e) {

			int    depth   = 0;
			char*   path = new char[m_info.path.length()];
			strcpy (path, m_info.path.c_str());
			char*  subpath = strtok (path, "/");
			Group* tmp;

			while (subpath != NULL) {
				
				try {
					if (depth)
						group = (*tmp).openGroup(subpath);
					else
						group = file.openGroup(subpath);
				} catch (Exception e) {
					if (depth)
						group = (*tmp).createGroup(subpath);
					else
						group = file.createGroup(subpath);
				}

				subpath = strtok (NULL, "/");

				tmp = &group;
				depth++;

			}

			group = (*tmp);
			
			delete[] path;

		}

		DataSpace     space (m_info.ndim, dims);
		FloatType     type  (PredType::NATIVE_DOUBLE);
		DataSet       set = group.createDataSet(m_info.dname, type, space);
		
		// Write data
		set.write  (data, type);

		delete[] dims;

		// Clean up.
		set.close  ();
		space.close();
		group.close();
		file.close();		

	} catch (const FileIException&      e) {
		return ReportException (e, IO::HDF5_FILE_I_EXCEPTION);
	} catch (GroupIException&    e) {
		return ReportException (e, IO::HDF5_FILE_I_EXCEPTION);
	} catch (const DataSetIException&   e) {
		return ReportException (e, IO::HDF5_DATASET_I_EXCEPTION);
	} catch (const DataSpaceIException& e) {
		return ReportException (e, IO::HDF5_DATASPACE_I_EXCEPTION);
	} catch (const DataTypeIException&  e) {
		return ReportException (e, IO::HDF5_DATATYPE_I_EXCEPTION);
	}

	return IO::OK;

}

const IO::Status
HDF5IO::ReadData (double* data, bool read) {

	try {

#ifndef VERBOSE
		Exception::dontPrint();
#endif
		
#ifdef VERBOSE
			printf ("Opening %s opened for RO access\n", m_info.fname.c_str());
#endif

		H5File      file(m_info.fname, H5F_ACC_RDONLY);

		DataSet     dataset = file.openDataSet(m_info.dname);
		DataType    type    = dataset.getFloatType();
		DataSpace   space   = dataset.getSpace();

		hsize_t*    dims    = new hsize_t[space.getSimpleExtentNdims()];
		m_info.ndim         = space.getSimpleExtentDims(dims, NULL);

		for (int i = 0; i < m_info.ndim; i++)
			m_info.dims[i] = dims[i];
		
#ifdef VERBOSE
		if (!read) {
			std::cout << "rank: " << m_info.ndim << ", dimensions: ";
			for (int i = 0; i < m_info.ndim; i++) {
				std::cout << (unsigned long)(m_info.dims[i]);
				if (i == m_info.ndim - 1)
					std::cout << std::endl;
				else
					std::cout << " x ";
			}
		}
#endif

		if (read)
			dataset.read (data, type);

		delete[] dims;

		space.close();
		dataset.close();
		file.close();

	} catch (const FileIException&      e) {
		return ReportException (e, IO::HDF5_FILE_I_EXCEPTION);
	} catch (const DataSetIException&   e) {
		return ReportException (e, IO::HDF5_DATASET_I_EXCEPTION);
	} catch (const DataSpaceIException& e) {
		return ReportException (e, IO::HDF5_DATASPACE_I_EXCEPTION);
	} catch (const DataTypeIException&  e) {
		return ReportException (e, IO::HDF5_DATATYPE_I_EXCEPTION);
	}

	return IO::OK;

}



const IO::Status
HDF5IO::ReadData (double* data) {

	return ReadData (data, true);

}



const DataInfo
HDF5IO::GetInfo (const std::string& dname) {

	m_info.dname = std::string(dname);
	ReadData (NULL, false);
	return m_info;
	
}



const IO::Status
HDF5IO::ReportException (const H5::Exception& e, const IO::Status ios) {
	
	e.printError();
	m_status = ios;
	std::cout << IO::StatusMessage[m_status] << std::endl;

	return m_status;
	
}
