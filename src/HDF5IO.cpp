#include "HDF5IO.h"

const IO::Status
HDF5IO::ReportException (const H5::Exception& e, const IO::Status ios) {
	e.printError();
	m_status = ios;
	std::cout << IO::StatusMessage[m_status] << std::endl;
	return m_status;
}


H5::Group
HDF5IO::CreateGroup (const std::string& url) {

	int    depth   = 0;
	char*   path   = new char[url.length()];
	std::copy (url.begin(), url.end(), path);
	char*  subpath = strtok (path, "/");

	H5::Group* tmp, group;

	while (subpath != NULL) {

		try {
			group = depth ? tmp->openGroup(subpath)   : m_file.openGroup(subpath);
		} catch (const H5::Exception& e) {
			group = depth ? tmp->createGroup(subpath) : m_file.createGroup(subpath);
		}

		subpath = strtok (NULL, "/");
		tmp = &group;
		depth++;

	}

	delete[] path;
	return *tmp;

}
