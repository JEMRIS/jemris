#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>

#include <H5Cpp.h>

/**
 * Binary file IO template class
 */
class BinaryIO {
	
public:
	
	/**
	 * @brief Contructor
	 */
	BinaryIO () {};
	
	/**
	 * @brief Destructor
	 */
	~BinaryIO () {};
	
	/**
	 * @brief Get information on the binary data in our file
	 */
	const void 
	GetInfo () {};
	
	/**
	 * @brief Get file name
	 *
	 * @return  File name
	 */
	const std::string 
	GetFileName () { return m_file_name; };

	/**
	 * @brief Set file name
	 *
	 * @param fn  File name
	 */
	const void
	SetFileName (const std::string fn) { m_file_name = fn; }

	/**
	 * @brief Get Data
	 */
	virtual void GetData (double* ) {};
	
	
private: 
	
	std::string   m_file_name;
	
};
