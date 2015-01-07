/** @file ContainerSequence.cpp
 *  @brief Implementation of JEMRIS ContainerSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#include "ContainerSequence.h"

/***********************************************************/
ContainerSequence::ContainerSequence  (const ContainerSequence& as) {


}

/***********************************************************/
bool ContainerSequence::Prepare (const PrepareMode mode) {

    bool b=true;

    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of ContainerSequence '" << GetName() << "' not succesful. " << endl;

    return b;

}

/***********************************************************/
string          ContainerSequence::GetInfo () {

	string ret;

	return ret;

}

