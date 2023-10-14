/** @file Attribute.cpp
 *  @brief Implementation of Attribute
 *
 * Author: tstoecker
 * Date  : Mar 18, 2009
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2023  Tony Stoecker
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


#include "Attribute.h"
#include "Prototype.h"
#include "AtomicSequence.h"
#include "ginac_functions.h"

/***********************************************************/
 Attribute::~Attribute (){
	if (m_datatype == typeid(   double*).name() ) delete ((   double*) m_backup);
	if (m_datatype == typeid(      int*).name() ) delete ((      int*) m_backup);
	if (m_datatype == typeid(     long*).name() ) delete ((     long*) m_backup);
	if (m_datatype == typeid( unsigned*).name() ) delete (( unsigned*) m_backup);
	if (m_datatype == typeid(     bool*).name() ) delete ((     bool*) m_backup);
	if (m_datatype == typeid(PulseAxis*).name() ) delete ((PulseAxis*) m_backup);
	if (m_datatype == typeid(  std::string*).name() ) delete ((  std::string*) m_backup);
}

/***********************************************************/
void Attribute::AttachObserver (Attribute* attrib){

	if ( !IsObservable() ) return;
	m_symbol_name = strtolower(m_prototype->GetName()+"_"+m_name);
	for (unsigned int i=0; i<m_observers.size(); i++) if ( attrib == m_observers.at(i) ) return;
	m_observers.push_back(attrib);
	attrib->AttachSubject(this);

}

/***********************************************************/
void Attribute::AttachSubject (Attribute* attrib){

	if ( !attrib->IsObservable() ) return;
	for (unsigned int i=0; i<m_subjects.size(); i++) if ( attrib == m_subjects.at(i) ) return;

	m_subjects.push_back(attrib);
	attrib->AttachObserver(this);

}


/***********************************************************/
void Attribute::UpdatePrototype (){
	Prototype* prot = GetPrototype();
	prot->Prepare(PREP_UPDATE);
	//cout << "DEBUG " << GetPrototype()->GetName() << " notified " << prot->GetName() << " : ";
	if (prot->GetType() == MOD_PULSE) ((AtomicSequence*) prot->GetParent())->CollectTPOIs();
	//Notify observers after (!) update of prototype
	if (GetTypeID()==typeid(  double*).name()) { Notify( GetMember  <double>() ); return; }
	if (GetTypeID()==typeid(     int*).name()) { Notify( GetMember     <int>() ); return; }
	if (GetTypeID()==typeid(    long*).name()) { Notify( GetMember    <long>() ); return; }
	if (GetTypeID()==typeid(unsigned*).name()) { Notify( GetMember<unsigned>() ); return; }
	if (GetTypeID()==typeid(    bool*).name()) { Notify( GetMember    <bool>() ); return; }
	//cout << " DEBUG Attribute::UpdatePrototype()  " << GetName() << endl;
}

/***********************************************************/
bool Attribute::SetMember (std::string expr, const vector<Attribute*>& obs_attribs, const vector<string>& obs_attrib_keyword, bool verbose){

	//set my own symbol
	m_symbol_name = strtolower(m_prototype->GetName()+"_"+m_name);
	
	//attribute represents a std::string
	if (GetTypeID()==typeid(std::string*).name()) {
	  m_formula = expr;
	  EvalExpression ();
	  return true;
	}

	//attribute represents a PulseAxis
	if (GetTypeID()==typeid(PulseAxis*).name()) {
		if (expr=="RF") { WriteMember(AXIS_RF);   return true; }
		if (expr=="GX") { WriteMember(AXIS_GX);   return true; }
		if (expr=="GY") { WriteMember(AXIS_GY);   return true; }
		if (expr=="GZ") { WriteMember(AXIS_GZ);   return true; }
		else			{ WriteMember(AXIS_VOID); return true; }
	}

	//GiNaC expressions
	Prototype::ReplaceSymbolString(expr,"step","csgn");
	if (expr.find("I", 0)!=std::string::npos) m_complex = true;

	m_subjects.clear();
	m_symlist.remove_all();
	//GiNaC::symbol d(m_sym_diff);
    //loop over all possibly observed subjects
	for (unsigned int i=0; i<obs_attribs.size() ; i++) {
		//convert user-defined keywords to the matching symbol name
		Attribute* subject_attrib = obs_attribs.at(i);
		std::string  SymbolName = strtolower( subject_attrib->GetPrototype()->GetName() + "_" + subject_attrib->GetName() );
        if (!Prototype::ReplaceSymbolString(expr,obs_attrib_keyword.at(i),SymbolName)) continue;
        //still here? the attribute was in the expression, so it is an observed subject
        AttachSubject( subject_attrib );
        m_symlist.append( get_symbol(SymbolName) );
	}


    m_formula = expr;

	//build GiNaC expression (maybe not successful at first call, if subjects still missing)
	try {
		m_expression = GiNaC::ex(m_formula,m_symlist);
		//differentiation of expression?
		if (m_diff>0) {
			m_expression = m_expression.diff(get_symbol(m_sym_diff),m_diff);
			stringstream se; se << m_expression;
			m_formula = se.str();
		}
		//test the expression evaluation once
		EvalExpression ();

	} catch (exception &p) {

        if ( verbose ) {

                    cout   << "Warning in " << m_prototype->GetName() << ": attribute " << GetName()
                           << " can not evaluate its GiNaC expression E = " << expr
                           << ". Reason: " << p.what() << endl;

            }
        return false;
	}

    return true;
}

/***********************************************************/
void Attribute::EvalExpression () {
  
	if (m_formula.empty())  return;
	
	//strings: xml value is stored in m_formula
	if (GetTypeID()==typeid(std::string*).name()) {
		WriteMember(m_formula);
		return;	  
	}

	//collect symbols and corresponding member-values from observed attributes
	GiNaC::lst numlist;
	for (unsigned int i=0; i<m_subjects.size() ; i++) {
		Attribute* a = m_subjects.at(i);
		if (a->GetTypeID()==typeid(  double*).name()) { numlist.append(a->GetMember  <double>() ); continue; }
		if (a->GetTypeID()==typeid(     int*).name()) { numlist.append(a->GetMember     <int>() ); continue; }
		if (a->GetTypeID()==typeid(    long*).name()) { numlist.append(a->GetMember    <long>() ); continue; }
		if (a->GetTypeID()==typeid(unsigned*).name()) { numlist.append(a->GetMember<unsigned>() ); continue; }
		if (a->GetTypeID()==typeid(    bool*).name()) { numlist.append(a->GetMember    <bool>() ); continue; }
	}

	
	//numeric evaluation of GiNaC expression
	GiNaC::ex e = m_expression.subs(m_symlist,numlist);	
	m_static_vector = m_prototype->GetVector(); // static pointer to evaluate the Vector function
	e = GiNaC::evalf(e);
	double d = 0.0;
	if (GiNaC::is_a<GiNaC::numeric>(e)) {
		if ( m_complex ) m_imaginary = -0.5 * GiNaC::ex_to<GiNaC::numeric>( (e-e.conjugate())*GiNaC::I ).to_double();
		d = GiNaC::ex_to<GiNaC::numeric>( e ).to_double();//default is real-part
	}


	//overwrite private member
	if (m_datatype==typeid(  double*).name() ) WriteMember((double)   d );
	if (m_datatype==typeid(     int*).name() ) WriteMember((int)      d );
	if (m_datatype==typeid(    long*).name() ) WriteMember((long)     d );
	if (m_datatype==typeid(unsigned*).name() ) WriteMember((unsigned) d );
	if (m_datatype==typeid(    bool*).name() ) WriteMember((bool)     d );

	//  for dynamic change of runtime compiled attributes after notification!
	if (m_ginac_excomp && m_cur_fp==m_num_fp && m_num_fp>0) {
		//cout << "!!! " << GetName() << " : NFP =" << GetNumberFunctionPointers() << endl;
		//cout << "!!! " << GetName() << " : CFP =" << GetCurrentFunctionPointer() << endl;
		//cout << "!!! " << GetName() << " : size=" << m_compiled.size() << endl;

			if (m_compiled.size() == m_cur_fp ) m_compiled.push_back(false);
		}
}

/***********************************************************/
double Attribute::EvalCompiledExpression (double const val, std::string const attrib ) {

//cout << GetPrototype()->GetName() << " ??  at pointer num " << m_cur_fp << " -> compiled = " << m_compiled.at(m_cur_fp) << endl;
 	if (!m_compiled.at(m_cur_fp)) {
 		//substitute all attributes with numbers in GiNaC expression, except the attribute
 		//which serves as the free parameter for runtime compilation
 		GiNaC::lst symlist;
 		GiNaC::lst numlist;
 		for (unsigned int i=0; i<m_subjects.size() ; i++) {
 			Attribute* a = m_subjects.at(i);
 			if (a->GetName() == attrib) continue;
 	        symlist.append( get_symbol(a->GetSymbol()) );
 			if (a->GetTypeID()==typeid(  double*).name()) { numlist.append(a->GetMember  <double>() ); continue; }
 			if (a->GetTypeID()==typeid(     int*).name()) { numlist.append(a->GetMember     <int>() ); continue; }
 			if (a->GetTypeID()==typeid(    long*).name()) { numlist.append(a->GetMember    <long>() ); continue; }
 			if (a->GetTypeID()==typeid(unsigned*).name()) { numlist.append(a->GetMember<unsigned>() ); continue; }
 			if (a->GetTypeID()==typeid(    bool*).name()) { numlist.append(a->GetMember    <bool>() ); continue; }
 		}

		GiNaC::ex e = GiNaC::evalf((symlist.nops()==0)?m_expression:m_expression.subs(symlist,numlist));

		//add function pointers
		m_fp.push_back(NULL);
		m_fpi.push_back(NULL);
		//compile the GiNaC expression
		try {
			if (!m_complex) {
				compile_ex(e, get_symbol(GetPrototype()->GetAttribute(attrib)->GetSymbol()), m_fp.at(m_num_fp));
			}
			else {
				compile_ex(e.real_part(), get_symbol(GetPrototype()->GetAttribute(attrib)->GetSymbol()), m_fp.at(m_num_fp));
				compile_ex(e.imag_part(), get_symbol(GetPrototype()->GetAttribute(attrib)->GetSymbol()), m_fpi.at(m_num_fp));
			}
 			//cout << " compiling expression " << e << " of attribute " << GetName() << " in module " << GetPrototype()->GetName() << endl;
 		 	m_num_fp++;
		}
	 	catch (exception &p) {
 			cout << " Warning: attribute " << GetName() << " of module " << GetPrototype()->GetName() << endl << endl
 				 << " function Attribute::EvalCompiledExpression" << endl
 				 << " No external runtime compiler available: " << p.what() << endl
				 << " Falling back to (slow) analytic evaluation!" << endl << endl
				 << " Hint: if you have a shell and gcc on your system, create the one-liner " << endl << endl
				 << "    #!/bin/sh" << endl
				 << "    gcc -x c -fPIC -shared -o $1.so $1" << endl << endl
 		         << " name it \"ginac-excompiler\", and put it somewhere in your search path." << endl << endl;
	 		m_ginac_excomp = false;
	 	}
		m_compiled.at(m_cur_fp) = true; //even if compilation failed, as we don't have to try a second time!
 	}

	//if compilation failed, invoke slow analytic evaluation
	if (!m_ginac_excomp ) {
		*((double*) GetPrototype()->GetAttribute(attrib)-> GetAddress()) = val;
 		EvalExpression();
 		return *((double*) GetAddress());
 	}

	//invoke fast runtime compiled routines
 	if (m_fpi.at(m_cur_fp) != NULL ) m_imaginary = m_fpi.at(m_cur_fp)(val);
	if ( m_fp.at(m_cur_fp) != NULL ) return m_fp.at(m_cur_fp)(val);

	return 0.0;

}

/***********************************************************/
double Attribute::EvalCompiledNLGExpression (double const x, double const y ,double const z, double const g ) {

//cout << GetPrototype()->GetName() << " ??  at pointer num " << m_cur_fp << " -> compiled = " << m_compiled.at(m_cur_fp) << endl;
 	if (m_nlgfp == NULL && m_ginac_excomp) {
 		//substitute all attributes with numbers in GiNaC expression, except the attribute
 		//which serves as the free parameter for runtime compilation
 		GiNaC::lst symlist;
 		GiNaC::lst numlist;
 		for (unsigned int i=0; i<m_subjects.size() ; i++) {
 			Attribute* a = m_subjects.at(i);
 			if (a->GetName() == "NLG_posX") continue;
 			if (a->GetName() == "NLG_posY") continue;
 			if (a->GetName() == "NLG_posZ") continue;
 			if (a->GetName() == "NLG_value") continue;
 	        symlist.append( get_symbol(a->GetSymbol()) );
 			if (a->GetTypeID()==typeid(  double*).name()) { numlist.append(a->GetMember  <double>() ); continue; }
 			if (a->GetTypeID()==typeid(     int*).name()) { numlist.append(a->GetMember     <int>() ); continue; }
 			if (a->GetTypeID()==typeid(    long*).name()) { numlist.append(a->GetMember    <long>() ); continue; }
 			if (a->GetTypeID()==typeid(unsigned*).name()) { numlist.append(a->GetMember<unsigned>() ); continue; }
 			if (a->GetTypeID()==typeid(    bool*).name()) { numlist.append(a->GetMember    <bool>() ); continue; }
 		}

		GiNaC::ex e = GiNaC::evalf((symlist.nops()==0)?m_expression:m_expression.subs(symlist,numlist));

		try {
			compile_ex (e,
						get_symbol(GetPrototype()->GetAttribute("NLG_posX")->GetSymbol()),
						get_symbol(GetPrototype()->GetAttribute("NLG_posY")->GetSymbol()),
						get_symbol(GetPrototype()->GetAttribute("NLG_posZ")->GetSymbol()),
						get_symbol(GetPrototype()->GetAttribute("NLG_value")->GetSymbol()),
						m_nlgfp);
 			//cout << " compiling attribute " << GetName() << " of module " << GetPrototype()->GetName() << endl;
		}
	 	catch (exception &p) {
 			cout << " Warning: attribute " << GetName() << " of module " << GetPrototype()->GetName() << endl << endl
 				 << " function Attribute::EvalCompiledNLGExpression" << endl
 				 << " No external runtime compiler available: " << p.what() << endl
				 << " Falling back to (slow) analytic evaluation!" << endl << endl
				 << " Hint: if you have a shell and gcc on your system, create the one-liner " << endl << endl
				 << "    #!/bin/sh" << endl
				 << "    gcc -x c -fPIC -shared -o $1.so $1" << endl << endl
 		         << " name it \"ginac-excompiler\", and put it somewhere in your search path." << endl << endl;
	 		m_ginac_excomp = false;
	 	}
 	}

	//if compilation failed, invoke slow analytic evaluation
	if (!m_ginac_excomp ) {
		*((double*) GetPrototype()->GetAttribute("NLG_posX")-> GetAddress()) = x;
		*((double*) GetPrototype()->GetAttribute("NLG_posY")-> GetAddress()) = y;
		*((double*) GetPrototype()->GetAttribute("NLG_posZ")-> GetAddress()) = z;
		*((double*) GetPrototype()->GetAttribute("NLG_value")-> GetAddress()) = g;
 		EvalExpression();
 		return *((double*) GetAddress());
 	}

	//invoke fast runtime compiled routines
	if ( m_nlgfp != NULL ) return m_nlgfp(x,y,z,g);

	return 0.0;

}


