#include "BinaryContext.h" 

BinaryContext::BinaryContext    () {
	m_strategies.push_back ((BinaryIO*) new SimpleIO ());
	m_strategies.push_back ((BinaryIO*) new HDF5IO   ());
	m_strategy = m_strategies.at(1);
	m_status = IO::OK;
}

BinaryContext::~BinaryContext () {};

IO::Status BinaryContext::Initialize (const std::string& fname, IO::Mode mode) {
	return m_strategy->Initialize(fname, mode);
}



IO::Status BinaryContext::WriteData (double* out) {
	return m_strategy->WriteData(out);
}

void       BinaryContext::SetInfo (const DataInfo& info) {
	m_strategy->SetInfo(info);
}

DataInfo   BinaryContext::GetInfo (const std::string& dname, const std::string& dpath) {
	return m_strategy->GetInfo(dname, dpath);
}

IO::Status BinaryContext::Status () const {
	return m_strategy->Status();
}
