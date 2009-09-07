/** @file ginac_functions.h
 *  @brief Implementation of JEMRIS ginac_functions
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#ifndef GINAC_FUNCTIONS_H_
#define GINAC_FUNCTIONS_H_

#include <ginac/ginac.h>
#include <vector>

using namespace std;
/**
 * @brief Functions for numeric evaluation of expressions (evalf)
 */

/**
 * @brief floor routine.
 *
 * Round expression to integer.
 */
static GiNaC::ex floor_evalf(const GiNaC::ex &x){
	if (!GiNaC::is_a<GiNaC::numeric>(x) ) return x;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	return ((int) GiNaC::ex_to<GiNaC::numeric>(xn).to_double()  );
}
DECLARE_FUNCTION_1P(floor)
REGISTER_FUNCTION(floor, evalf_func(floor_evalf))

/**
 * @brief Mod routine.
 *
 * Modulo of two expressions.
 */
static GiNaC::ex mod_evalf(const GiNaC::ex &x, const GiNaC::ex &y){
	if (!GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y)) return 0;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	GiNaC::ex yn = GiNaC::ex_to<GiNaC::numeric>(y);
	return (xn - floor_evalf(xn/yn)*yn);
}
DECLARE_FUNCTION_2P(mod)
REGISTER_FUNCTION(mod, evalf_func(mod_evalf))

/**
 * @brief equal routine.
 *
 * Returns 1 if expressions have equal values, 0 otherwise.
 */
static GiNaC::ex equal_evalf(const GiNaC::ex &x, const GiNaC::ex &y){
	if (!GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y)) return 0;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	GiNaC::ex yn = GiNaC::ex_to<GiNaC::numeric>(y);
	int b = ((int) (	GiNaC::ex_to<GiNaC::numeric>(xn).to_double()
			==	GiNaC::ex_to<GiNaC::numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(equal)
REGISTER_FUNCTION(equal, evalf_func(equal_evalf))

/**
 * @brief greater_than routine.
 *
 * Returns 1 if expression x > y, 0 otherwise.
 */
static GiNaC::ex gt_evalf(const GiNaC::ex &x, const GiNaC::ex &y){
	if (!GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y)) return 0;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	GiNaC::ex yn = GiNaC::ex_to<GiNaC::numeric>(y);
	int b = ((int) (	GiNaC::ex_to<GiNaC::numeric>(xn).to_double()
			>	GiNaC::ex_to<GiNaC::numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(gt)
REGISTER_FUNCTION(gt, evalf_func(gt_evalf))

/**
 * @brief less_than routine.
 *
 * Returns 1 if expression x < y, 0 otherwise.
 */
static GiNaC::ex lt_evalf(const GiNaC::ex &x, const GiNaC::ex &y){
	if (!GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y)) return 0;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	GiNaC::ex yn = GiNaC::ex_to<GiNaC::numeric>(y);
	int b = ((int) (	GiNaC::ex_to<GiNaC::numeric>(xn).to_double()
			<	GiNaC::ex_to<GiNaC::numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(lt)
REGISTER_FUNCTION(lt, evalf_func(lt_evalf))

/**
 * @brief IfThenElse routine.
 *
 * if a==b returns x, otherwise y.
 */
static GiNaC::ex ite_evalf(const GiNaC::ex &a, const GiNaC::ex &b, const GiNaC::ex &x, const GiNaC::ex &y){
	if (!GiNaC::is_a<GiNaC::numeric>(x) || !GiNaC::is_a<GiNaC::numeric>(y)) return 0;
	GiNaC::ex xn = GiNaC::ex_to<GiNaC::numeric>(x);
	GiNaC::ex yn = GiNaC::ex_to<GiNaC::numeric>(y);
	if ( equal_evalf(a,b)==1 ) return xn;
	else			   return yn;
}
DECLARE_FUNCTION_4P(ite)
REGISTER_FUNCTION(ite, evalf_func(ite_evalf))


/**
 * @brief Vector element.
 *
 * Get an element of a Vector.
 */
static vector<double>* m_static_vector;
static GiNaC::ex Vector_evalf(const GiNaC::ex &i){
	if (!GiNaC::is_a<GiNaC::numeric>(i) ) return 0;
	int in = ((int) GiNaC::ex_to<GiNaC::numeric>(i).to_double() );
	if ( (*m_static_vector).size() > in )
		return (*m_static_vector).at(in);
	else
		return 0.0;
}
DECLARE_FUNCTION_1P(Vector)
REGISTER_FUNCTION(Vector, evalf_func(Vector_evalf))

#endif
