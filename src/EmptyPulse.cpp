/** @file EmptyPulse.cpp
 *  @brief Implementation of JEMRIS EmptyPulse
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

#include "EmptyPulse.h"

/***********************************************************/
bool EmptyPulse::Prepare  (PrepareMode mode) {

	ATTRIBUTE("Shape"    , m_analytic_value );
	
	m_axis = AXIS_VOID;

	return Pulse::Prepare(mode);

};

/*****************************************************************/
inline void  EmptyPulse::SetTPOIs () {

	if (!m_analytic) {
	  //standard way: equidistant ADC sampling
	  Pulse::SetTPOIs();
	}
	else {
	  //non-equdistant sampling accordig to Shape-attribute (GiNaC formula)
	  m_tpoi.Reset();
	  double D=GetDuration();
	  m_tpoi + TPOI::set(D, -1.0);
	  unsigned int N = GetNADC();
	  double first = GetAttribute("Shape")->EvalCompiledExpression(0.0,"AnalyticTime");
	  double last  = GetAttribute("Shape")->EvalCompiledExpression(D,"AnalyticTime");
	  for (unsigned i = 0; i < N; i++) {
	    double t = (i+1)*D/(GetNADC()+1);
	    double shape = GetAttribute("Shape")->EvalCompiledExpression(t,"AnalyticTime");
	    double adc   = D*(shape-first)/(last-first); //scale adc event into livetime of this emptypulse
	    m_tpoi + TPOI::set(adc, (m_phase_lock?World::instance()->PhaseLock:0.0) );
	  }
	}

}
