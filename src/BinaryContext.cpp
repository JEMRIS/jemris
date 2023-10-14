#include "BinaryContext.h" 

BinaryContext::BinaryContext    (const std::string& fname, IO::Mode mode) {
	m_strategy = (BinaryIO*) new HDF5IO   (fname, mode);
	m_status = IO::OK;
}

BinaryContext::~BinaryContext () {
	delete m_strategy;
};

IO::Status BinaryContext::Status () const {
	return m_strategy->Status();
}
