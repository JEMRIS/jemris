/** @file TxRxPhase.cpp
 *  @brief Implementation of JEMRIS TxRxPhase
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

#include "TxRxPhase.h"
#include "RFPulse.h"

/*****************************************************************/
TxRxPhase::TxRxPhase() {

   SetInitialPhase(0.0);
   SetFrequency(0.0) ;

}


/*****************************************************************/
double    TxRxPhase::getLinearPhase  (Module* mod, double time ) {
	// set zero phase to center of Pulse Object. (True for e.g. sinc pulses, where main excitation is at center.)
    return ( (((RFPulse*) mod)->GetFrequency() * (time-((RFPulse*) mod)->GetDuration()/2))*180.0/PI ) ;
};
