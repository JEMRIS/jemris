#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>

/**
 * @brief Information structure on binary data
 */
struct FileInfo {

	std::string file_name;
	std::string data_name;
	std::string data_type;
	std::string uuid;
	double      ndim;
	double*     dims; 
	
};

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
	 * @brief Get file name
	 *
	 * @return  File name
	 */
	inline const std::string 
	GetFileName   ()                     { 
		return m_file_info.file_name; 
	};

	/**
	 * @brief Set file name
	 *
	 * @param fn  File name
	 */
	inline const void
	SetFileName   (const std::string fn) { 
		m_file_info.file_name = fn; 
	}

	/**
	 * @brief     Read data from file to container
	 *
	 * @param  dc Data container
	 */
	virtual void 
	ReadData      (double* dc)           = 0;
	
	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	virtual void 
	GetData       (double* dc)           = 0;
	
	/**
	 * @brief     Get information on the data in binary file
	 */
	inline const FileInfo
	GetInfo       ()                     {
		return m_file_info;
	}
	
	/**
	 * @brief     Get information on the data in binary file
	 */
	inline const void
	SetInfo       (FileInfo fi)          {
		m_file_info = fi;
	}
	
	
private: 
	
	FileInfo m_file_info;
	
};
