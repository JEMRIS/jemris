#include "HDF5IO.h"

const IO::Status
HDF5IO::ReportException (const H5::Exception& e, const IO::Status ios) {
	e.printError();
	m_status = ios;
	std::cout << IO::StatusMessage[m_status] << std::endl;
	return m_status;
}
