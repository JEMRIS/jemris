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

	size_t depth = 0,
	       snip  = url.find_first_of(SLASH, 0) ? 0 : 1,
	       snap  = url.find_first_of(SLASH, snip);

	std::string subpath = url.substr(snip,snap);

	H5::Group* tmp, group;

	while (!subpath.empty()) {

		try {
			group = depth ? tmp->openGroup(subpath)   : m_file.openGroup(subpath);
		} catch (const H5::Exception& e) {
			group = depth ? tmp->createGroup(subpath) : m_file.createGroup(subpath);
		}

		snip = ++snap;
		snap = url.find_first_of(SLASH,snip);
		subpath = url.substr(snip,snap);

		tmp = &group;
		depth++;

	}

	return *tmp;

}
