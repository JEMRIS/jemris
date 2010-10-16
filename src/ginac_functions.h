/** @file ginac_functions.h
 *  @brief Implementation of JEMRIS ginac_functions
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

#ifndef GINAC_FUNCTIONS_H_
#define GINAC_FUNCTIONS_H_

#include <ginac/ginac.h>
#include <vector>
#include <dlfcn.h>
#include <fstream>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>
#include "config.h"

#ifdef HAVE_MKSTEMPS
extern "C" int mkstemps (char *path, int len);
#endif

using namespace std;

using namespace GiNaC ;



/**
 * @brief Get a unique GiNaC symbol
 *
 * A symbol factory, which either creates a new symbol or
 * takes previously defined symbol from a global map.
 */
const symbol & get_symbol(const string & sym_name)
{
    static map<string, symbol> symbol_list;
    map<string, symbol>::iterator it = symbol_list.find(sym_name);
    if (it != symbol_list.end())
        return it->second;
    else
        return symbol_list.insert(pair<string, symbol>(sym_name, symbol(sym_name))).first->second;
}


/**
 * @brief sinc-function.
 *
 * return sin(x)/x for nonzero x, else return 1
 */
static ex sinc_eval(const ex &x){
	if ( x.is_zero() )
		return 1;
	else
		return (sin(x)/x);
}

static ex sinc_evalf(const ex & x){
     if (is_exactly_a<numeric>(x)) {
    	 double d = (ex_to<numeric>(x)).to_double();
         return ( d==0.0 ? 1 : sin(d)/d );
     }
     else
         return (sin(x)/x);
 }

static ex sinc_deriv(const ex & x, unsigned diff_param) {
    return cos(x)/x - sin(x)/(x*x);
}

//real(sinc(a+I*b)) = (a*cosh(b)*sin(a) + b*cos(a)*sinh(b))/(a^2 + b^2)
static ex sinc_real_part(const ex & x)
{
	return ( ( ( real_part(x)*cosh(imag_part(x))*sin(real_part(x)) ) +
			   ( imag_part(x)*sinh(imag_part(x))*cos(real_part(x)) ) ) /
			   ( pow(real_part(x),2) + pow(imag_part(x),2) ) );
}

//imag(sinc(a+I*b)) = (a*cos(a)*sinh(b) - b*cosh(b)*sin(a))/(a^2 + b^2)
static ex sinc_imag_part(const ex & x)
{
	return ( ( ( real_part(x)*sinh(imag_part(x))*cos(real_part(x)) ) -
			   ( imag_part(x)*cosh(imag_part(x))*sin(real_part(x)) ) ) /
			   ( pow(real_part(x),2) + pow(imag_part(x),2) ) );
}

static ex sinc_conjugate(const ex & x) {
	return sin(x.conjugate())/x.conjugate();
}

DECLARE_FUNCTION_1P(sinc)
REGISTER_FUNCTION(sinc, eval_func      (sinc_eval     ).
                        evalf_func     (sinc_evalf    ).
                        derivative_func(sinc_deriv    ).
                        real_part_func (sinc_real_part).
                        imag_part_func (sinc_imag_part).
                        conjugate_func (sinc_conjugate))


/**
 * @brief floor routine.
 *
 * Round expression to integer.
 */
static ex floor_evalf(const ex &x){
	if (!is_a<numeric>(x) ) return x;
	ex xn = ex_to<numeric>(x);
	return ((int) ex_to<numeric>(xn).to_double()  );
}
DECLARE_FUNCTION_1P(floor)
REGISTER_FUNCTION(floor, evalf_func(floor_evalf))

/**
 * @brief Mod routine.
 *
 * Modulo of two expressions.
 */
static ex mod_evalf(const ex &x, const ex &y){
	if (!is_a<numeric>(x) || !is_a<numeric>(y)) return 0;
	ex xn = ex_to<numeric>(x);
	ex yn = ex_to<numeric>(y);
	return (xn - floor_evalf(xn/yn)*yn);
}
DECLARE_FUNCTION_2P(mod)
REGISTER_FUNCTION(mod, evalf_func(mod_evalf))

/**
 * @brief equal routine.
 *
 * Returns 1 if expressions have equal values, 0 otherwise.
 */
static ex equal_evalf(const ex &x, const ex &y){
	if (!is_a<numeric>(x) || !is_a<numeric>(y)) return 0;
	ex xn = ex_to<numeric>(x);
	ex yn = ex_to<numeric>(y);
	int b = ((int) (	ex_to<numeric>(xn).to_double()
			==	ex_to<numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(equal)
REGISTER_FUNCTION(equal, evalf_func(equal_evalf))

/**
 * @brief greater_than routine.
 *
 * Returns 1 if expression x > y, 0 otherwise.
 */
static ex gt_evalf(const ex &x, const ex &y){
	if (!is_a<numeric>(x) || !is_a<numeric>(y)) return 0;
	ex xn = ex_to<numeric>(x);
	ex yn = ex_to<numeric>(y);
	int b = ((int) (	ex_to<numeric>(xn).to_double()
			>	ex_to<numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(gt)
REGISTER_FUNCTION(gt, evalf_func(gt_evalf))

/**
 * @brief less_than routine.
 *
 * Returns 1 if expression x < y, 0 otherwise.
 */
static ex lt_evalf(const ex &x, const ex &y){
	if (!is_a<numeric>(x) || !is_a<numeric>(y)) return 0;
	ex xn = ex_to<numeric>(x);
	ex yn = ex_to<numeric>(y);
	int b = ((int) (	ex_to<numeric>(xn).to_double()
			<	ex_to<numeric>(yn).to_double() ) );
	return b;
}
DECLARE_FUNCTION_2P(lt)
REGISTER_FUNCTION(lt, evalf_func(lt_evalf))

/**
 * @brief IfThenElse routine.
 *
 * if a==b returns x, otherwise y.
 */
static ex ite_evalf(const ex &a, const ex &b, const ex &x, const ex &y){
	if (!is_a<numeric>(x) || !is_a<numeric>(y)) return 0;
	ex xn = ex_to<numeric>(x);
	ex yn = ex_to<numeric>(y);
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
static ex Vector_evalf(const ex &i){
	if (!is_a<numeric>(i) ) return 0;
	unsigned int in = ((int) ex_to<numeric>(i).to_double() );
	if ( (*m_static_vector).size() > in )
		return (*m_static_vector).at(in);
	else
		return 0.0;
}
DECLARE_FUNCTION_1P(Vector)
REGISTER_FUNCTION(Vector, evalf_func(Vector_evalf))


/**
 * All what follows is for GiNaC external compiler with four function parameters
 * (for nonlinear gradient fields)
 */

/* copy-paste of the GiNaC excompiler helper class (Removed all comments; see GiNaC sources for more info) */
class excompiler
{
	struct filedesc
	{
		void* module;
		std::string name;
		bool clean_up;
	};
	std::vector<filedesc> filelist;
public:
	~excompiler()
	{
		for (std::vector<filedesc>::const_iterator it = filelist.begin(); it != filelist.end(); ++it) {
			clean_up(it);
		}
	}
	void add_opened_module(void* module, const std::string& name, bool clean_up)
	{
		filedesc fd;
		fd.module = module;
		fd.name = name;
		fd.clean_up = clean_up;
		filelist.push_back(fd);
	}
	void clean_up(const std::vector<filedesc>::const_iterator it)
	{
		dlclose(it->module);
		if (it->clean_up) {
			remove(it->name.c_str());
		}
	}
	void create_src_file(std::string& filename, std::ofstream& ofs)
	{
		if (filename.empty()) {
			const char* filename_pattern = "./GiNaCXXXXXX";
			char* new_filename = new char[strlen(filename_pattern)+1];
			strcpy(new_filename, filename_pattern);
			#ifndef HAVE_MKSTEMPS
			if (!mkstemp(new_filename)) {
				delete[] new_filename;
				throw std::runtime_error("mktemp failed");
			}
			#else
			if (!mkstemps(new_filename, 0)) {
				delete[] new_filename;
				throw std::runtime_error("mktemps failed");
			}
			#endif
			filename = std::string(new_filename);
			ofs.open(new_filename, std::ios::out);
			delete[] new_filename;
		} else {
			ofs.open(filename.c_str(), std::ios::out);
		}

		if (!ofs) {
			throw std::runtime_error("could not create source code file for compilation");
		}

		ofs << "#include <stddef.h> " << std::endl;
		ofs << "#include <stdlib.h> " << std::endl;
		ofs << "#include <math.h> " << std::endl;
		ofs << std::endl;
	}
	void compile_src_file(const std::string filename, bool clean_up)
	{
		std::string strcompile = "ginac-excompiler " + filename;
		if (system(strcompile.c_str())) {
			throw std::runtime_error("excompiler::compile_src_file: error compiling source file!");
		}
		if (clean_up) {
			remove(filename.c_str());
		}
	}
	void* link_so_file(const std::string filename, bool clean_up)
	{
		void* module = NULL;
		module = dlopen(filename.c_str(), RTLD_NOW);
		if (module == NULL)	{
			throw std::runtime_error("excompiler::link_so_file: could not open compiled module!");
		}

		add_opened_module(module, filename, clean_up);

		return dlsym(module, "compiled_ex");
	}
	void unlink(const std::string filename)
	{
		for (std::vector<filedesc>::iterator it = filelist.begin(); it != filelist.end();) {
			if (it->name == filename) {
				clean_up(it);
				it = filelist.erase(it);
			} else {
				++it;
			}
		}
	}
};

typedef double (*FUNCP_4P) (double, double, double, double);
static excompiler global_excompiler;
void compile_ex(const ex& expr, const symbol& sym1, const symbol& sym2, const symbol& sym3, const symbol& sym4,
		        FUNCP_4P& fp, const std::string filename = "") {

	symbol x("x"), y("y"), z("z"), g("g");
	ex expr_with_xyzg = expr.subs(lst(sym1==x, sym2==y, sym3==z, sym4==g));

	std::ofstream ofs;
	std::string unique_filename = filename;
	global_excompiler.create_src_file(unique_filename, ofs);

	ofs << "double compiled_ex(double x, double y, double z, double g)" << std::endl;
	ofs << "{" << std::endl;
	ofs << "double res = ";
	expr_with_xyzg.print(GiNaC::print_csrc_double(ofs));
	ofs << ";" << std::endl;
	ofs << "return(res); " << std::endl;
	ofs << "}" << std::endl;

	ofs.close();

	global_excompiler.compile_src_file(unique_filename, filename.empty());
	fp = (FUNCP_4P) global_excompiler.link_so_file(unique_filename+".so", filename.empty());
}


#endif
