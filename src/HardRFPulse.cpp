/** @file HardRFPulse.cpp
 *  @brief Implementation of JEMRIS HardRFPulse
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2020  Tony Stoecker
 *                        2007-2018  Kaveh Vahedipour
 *                        2009-2019  Daniel Pflugfelder
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

#include "HardRFPulse.h"

/***********************************************************/
bool HardRFPulse::Prepare  (const PrepareMode mode) {

	if (mode != PREP_UPDATE)
		m_bw  = 1e16;

	// Prepare up the chain
	bool b = RFPulse::Prepare(mode);

	// Remove Bandwidth from XML attributes
	if (mode != PREP_UPDATE)
		HideAttribute ("Bandwidth",false);

	return b;

}




