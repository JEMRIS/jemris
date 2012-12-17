/** @file SampleReorderStrategyInterface.h
 * @brief Implementation of JEMRIS of SampleReorderStrategyInterface
 *
 *  Created on: Jan 11, 2010
 *      Author: dpflug
 */


/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stöcker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
 *                                  
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

#ifndef SAMPLEREORDERSTRATEGYINTERFACE_H_
#define SAMPLEREORDERSTRATEGYINTERFACE_H_

struct Spin;

/**
 * @brief base class for different sample reorder strategies.
 */
class SampleReorderStrategyInterface {
public:
	SampleReorderStrategyInterface() {};
	virtual ~SampleReorderStrategyInterface() {};

	/**
	 * function which reorders the sample
	 */
	virtual void Execute(Spin* data) {};
};

#endif /* SAMPLEREORDERSTRATEGYINTERFACE_H_ */
