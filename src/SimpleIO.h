/** @file SimpleIO.h
 *  @brief Implementation of JEMRIS Sample
 */

/*
 *  JEMRIS Copyright (C) 2007-2010  Tony Stöcker, Kaveh Vahedipour
 *                                  Forschungszentrum Jülich, Germany
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

#ifndef SIMPLE_IO_H_
#define SIMPLE_IO_H_

#include "BinaryIO.h"

class SimpleIO : public BinaryIO {
	
public:

	/**
	 * @brief Contructor
	 */
	SimpleIO     ()                      {};
	
	/**
	 * @brief Destructor
	 */
	~SimpleIO     ()                     {};
	
	/**
	 * @brief     Read data from file to container
	 *
	 * @param  dc Data container
	 */
	virtual const IO::Status
	ReadData      (double* dc)           { return IO::OK; };
	
	/**
	 * @brief     Write data from container to file
	 *
	 * @param  dc Data container
	 */
	virtual const IO::Status
	WriteData     (double* dc)           {return IO::OK; };

};

#endif
