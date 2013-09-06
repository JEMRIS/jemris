#include "HDF5IO.h"

#include <typeinfo>
#include <string.h>

using namespace H5;

//#define VERBOSE

IO::Status
HDF5IO::WriteData (const double* data) {
	
	try {
		
#ifndef VERBOSE
		Exception::dontPrint();
#endif
		
		hsize_t* dims = new hsize_t [m_info.NDim()];
		
		for (int i = 0; i < m_info.NDim(); i++)
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

			group = file.openGroup(m_info.dpath);
#ifdef VERBOSE
			printf ("Group %s opened for writing\n", m_info.dpath.c_str()) ;
#endif

		} catch (Exception e) {

			int    depth   = 0;
			char*   path = new char[m_info.dpath.length()];
			strcpy (path, m_info.dpath.c_str());
			char*  subpath = strtok (path, "/");
			Group* tmp;

			while (subpath != NULL) {
				
				try {
					group = depth ? tmp->openGroup(subpath)   : file.openGroup(subpath);
				} catch (Exception e) {
					group = depth ? tmp->createGroup(subpath) : file.createGroup(subpath);
				}

				subpath = strtok (NULL, "/");
				tmp = &group;
				depth++;

			}

			group = (*tmp);
			
			delete[] path;

		}

		DataSpace     space (m_info.NDim(), dims);
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

DataInfo
HDF5IO::GetInfo (const std::string& dname, const std::string& dpath) {

	return m_info;
	
}



const IO::Status
HDF5IO::ReportException (const H5::Exception& e, const IO::Status ios) {
	
	e.printError();
	m_status = ios;
	std::cout << IO::StatusMessage[m_status] << std::endl;

	return m_status;
	
}
