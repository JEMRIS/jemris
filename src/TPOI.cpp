/** @file TPOI.cpp
 *  @brief Implementation of JEMRIS TPOI
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

#include "TPOI.h"
#include <cmath>

/***********************************************************/
void TPOI::operator += (const TPOI& tpoi) {

    for (int i = 0; i < tpoi.GetSize(); i++) 
        *(this) + TPOI::set (tpoi.m_time[i], tpoi.m_phase[i]);

}


/***********************************************************/
TPOI TPOI::operator + (const TPOI& tpoi) {
    *this += tpoi; 
    return *this;
}


/***********************************************************/
void TPOI::operator + (const TPOI::set& data) {

    m_time.push_back(  data.dtime );
    m_phase.push_back( data.dphase );
  
};


/***********************************************************/
int TPOI::GetSize () const {
    return m_time.size(); 
}


/***********************************************************/
void TPOI::Sort ()        { 

    int i, j;
    double       keyT, keyP;
    
    // An insert Sort algorithm implementation
    for (j = 1; j < GetSize(); j++) {
        keyT = m_time[j];
        keyP = m_phase[j];
        for(i = j - 1; (i >= 0) && (m_time[i] > keyT); i--) {
            m_time[i+1]    = m_time[i];
            m_phase[i+1]   = m_phase[i];
        }
        m_time[i+1] = keyT;
        m_phase[i+1] = keyP;
    }

}


/***********************************************************/
void TPOI::Purge ()        { 

 // will only purge, if more than one data points 
 if (GetSize() > 1) {

   int i=0, j=0, k=0;
   for (i = 0; i < GetSize(); ++i) {

        if ( i+1 < GetSize() )
        if ( fabs(m_time[i+1]-m_time[i])< TIME_ERR_TOL )   //(i+1,i) same => keep maximum phase
        {
            if ( m_phase[i]> m_phase[i+1] )	m_phase[i+1] = m_phase[i] ;
	    else				m_phase[i] = m_phase[i+1] ;
        }

        if ( fabs(m_time[i]-m_time[j])> TIME_ERR_TOL) //(j,i) different => replace j+1 with i 
	{
            m_phase[++j]  = m_phase[i];
            m_time[j]     = m_time[i];
	}
   }
   m_time.erase( m_time.begin()+j+1, m_time.end() );
   m_phase.erase( m_phase.begin()+j+1, m_phase.end() );

 }
}

