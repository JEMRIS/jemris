/** @file Prototype.cpp
 *  @brief Implementation of JEMRIS Prototype
 */

/*
 *  JEMRIS Copyright (C) 2007-2008  Tony Stöcker, Kaveh Vahedipour
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

#include "Prototype.h"
#include "AtomicSequence.h"

#include "ginac_functions.h"

/***********************************************************/
bool Prototype::Newstate (void* key) {

	map<void*,void*>::iterator itmem = m_subject_state.find(key);
	if (itmem == m_subject_state.end()) return false; //key is not in the state map

	map<void*,string>::iterator itype = m_subject_type.find(key);
	if (itype->second == typeid(double*).name()) {
		if (*((double*) itmem->second) == *((double*) key))             return false;
		*((double*) itmem->second) = *((double*) key);                  return  true;
	}

	if (itype->second == typeid(unsigned int*).name()) {
		if (*((unsigned int*) itmem->second) == *((unsigned int*) key)) return false;
		*((unsigned int*) itmem->second) = *((unsigned int*) key);      return  true;
	}

	if (itype->second == typeid(bool*).name()) {
		if (*((bool*) itmem->second) == *((bool*) key))                 return false;
		*((bool*) itmem->second) = *((bool*) key);                      return  true;
	}

	if (itype->second == typeid(float*).name()) {
		if (*((float*) itmem->second) == *((float*) key))               return false;
		*((float*) itmem->second) = *((float*) key);                    return  true;
	}

	if (itype->second == typeid(int*).name()) {
		if (*((int*) itmem->second) == *((int*) key))                   return false;
		*((int*) itmem->second) = *((int*) key);                        return true ;
	}

	if (itype->second == typeid(long*).name()) {
		if (*((long*) itmem->second) == *((long*) key))                 return false;
		*((long*) itmem->second) = *((long*) key);                      return true ;
	}

	if (itype->second == typeid(string*).name() ) {
		if (*((string*) itmem->second) == *((string*) key))             return false;
		*((string*) itmem->second) = *((string*) key);                  return  true;
	}

	if (itype->second == typeid(PulseAxis*).name()) {
		if (*((PulseAxis*) itmem->second) == *((PulseAxis*) key))       return false;
		*((PulseAxis*) itmem->second) = *((PulseAxis*) key);            return  true;
	}

	return false;

}

/***********************************************************/
bool Prototype::Notify (void* ppriv){

    if (!Newstate(ppriv)) return false;

	map<string,void*>::iterator iaddr;

	for(iaddr = m_attrib_addr.begin(); iaddr != m_attrib_addr.end(); iaddr++)
		if (iaddr->second == ppriv) Notify(iaddr->first);

	return true;
};

/***********************************************************/
void Prototype::Notify (string attrib) {

	//iterators to find all keys==attrib in the AttribObserver multimap
	pair<multimap< string , ObsModAttr > ::iterator, multimap< string , ObsModAttr > ::iterator> itp;
	itp = m_attrib_observer.equal_range(attrib);
	multimap< string , ObsModAttr >::iterator it;

	//update member variables in observers
	for (it=itp.first; it!=itp.second; ++it) {

		ObsModAttr obs = it->second;

   		//debug output
		DEBUG_PRINT( " Attribute " <<  attrib << " triggered a Notify(). Module " << GetName() <<
                             " updates attribute " << obs.second << " of module "
			     << (obs.first)->GetName() << endl; )

		//Calculate linked-attributes of the observer
		(obs.first)->EvalExpressions(obs.second,PREP_UPDATE);

		//call Prepare(2)
		(obs.first)->Prepare(PREP_UPDATE);

		//collect possibly changed TPOIs for parent Atoms
		if ((obs.first)->GetType() == MOD_PULSE)
		    ((AtomicSequence*) (obs.first)->GetParent())->CollectTPOIs();

	}

};

/***********************************************************/
void Prototype::SetName (string name) {

	m_name = name;

	if (m_node == NULL) return;
        ((DOMElement*) m_node)->setAttribute( StrX("Name").XMLchar(), StrX(name).XMLchar() );

};

/***********************************************************/
vector<string>  Prototype::Tokenize(const string& str, const string& delimiters ) {

    vector<string> tokens;
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {

        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);

    }

    return tokens;

}

/***********************************************************/
void Prototype::SetExceptionalAttrib (string attrib) {

	for (int i=0; i<m_excpetional_attribs.size(); i++)
		if ( attrib == m_excpetional_attribs.at(i) ) return;

	m_excpetional_attribs.push_back(attrib);
};

/***********************************************************/
bool Prototype::Prepare  (PrepareMode mode){

	//nothing to be done for updating modules
	if (mode==PREP_UPDATE) return(true);

	m_aux = false;
	m_prepared = true;

	bool retval = true;

	//connect XML attributes and member variables:
	//every module might have a Name and might connect to a ConcatSequence.
	ATTRIBUTE("Name" , &m_name);

	//special case of the loopable vector
	ATTRIBUTE("Vector"   , NULL);

	//loop over all attributes in the maps m_Attrib{Type,Addr} of this module
	map<string,string>::iterator itype;

	for(itype = m_attrib_type.begin(); itype != m_attrib_type.end(); itype++) {

	    string atr = itype->first;
		string val; //the attribute value is written to val

		if ( GetAttribute(val,atr) ) {//attribute exists in m_node

		    //special cases: attributes for which standard preparation is omitted
			bool cont = false;

			for (int i=0; i<m_excpetional_attribs.size(); i++)
				if ( atr == m_excpetional_attribs.at(i) ) cont=true;

			if (cont) continue;

			//another special case: set the vector attribute
			if (atr == "Vector") {

				m_vector.clear();
				vector<string> vp = Tokenize(val," ");

				for (int i=0; i<vp.size(); i++) m_vector.push_back(atof(vp[i].c_str()));

				continue;
			}

			//yet another special case: set the modules for observation
			if (atr == "Observe") {

				m_observed_modules.clear();
				m_observed_attribs.clear();
				vector<string> vp = Tokenize(val,"/");

				for (unsigned int i=0;i<vp.size();i++) {

					vector<string> vs = Tokenize(vp[i],",");

					if (vs.size() != 2) {
						if (mode == PREP_VERBOSE)
							cout << GetName() << ": wrong syntax in Observe \n";
						retval = false;
						continue;
					}

					Prototype* M = GetModuleByAttributeValue("Name",vs[0]);

					if (M!=NULL)
						m_observed_modules.push_back(M);
					else {
						if (mode == PREP_VERBOSE)
							cout << GetName() << ": Observe error: module "
							     << vs[0] << " does not exist  in sequence\n";
						retval = false;
						continue;
					}

					//observed subjects need to be prepared first!
					if (!M->IsPrepared())
					    M->Prepare(PREP_INIT);

					if (M->HasAttribute(vs[1]))
					    m_observed_attribs.push_back(vs[1]);
					else {
						if (mode == PREP_VERBOSE)
							cout << GetName() << ": Observe error: module "
							     << M->GetName() << " has no attribute " << vs[1] << endl;
						retval = false;
						continue;
					}

				}

				continue; //proceed with next attribute

			}

			// Set the GiNaC Expression of this attribute.
			// In case of success, evaluate its expression.
			if (SetExpression(atr,val,mode)) {

				try {

				    EvalExpressions(atr,mode);

				} catch (exception &p) {

				    if ( mode == PREP_VERBOSE ) {

						cout	<< "Warning in " << GetName() << ": attribute " << atr
							<< " can not evaluate its GiNaC expression";
        					map<string,string>::iterator iex = m_attrib_expr_str.find(atr);

						if (iex != m_attrib_expr_str.end()) cout << " E = " << iex->second  << ".";

						cout   << " Reason: " << p.what() << endl;

					}

					retval = false;

				}
				continue;
			}

	        //static cases: write value to the member variable
			if ( WriteAttribAddress(atr,val)) continue;

			//if we are still in the loop, the attribute/value pair is not valid
			if (mode == PREP_VERBOSE) {

				cout << "Warning in Module '" << GetName();
				cout << "' : cannot set attribute  '" << atr;
				cout << "' to value '"<< val;
				cout << "' due to unkown keywords / data type" << endl;

			}

			retval = false;

		}

	}

	return retval;

}

/***********************************************************/
void     Prototype::SetAttribSymbol(string attrib) {

	if (m_attrib_symbol.find(attrib) != m_attrib_symbol.end() )
		m_attrib_symbol.erase(attrib);

	m_attrib_symbol.insert(pair<string,GiNaC::symbol>( attrib, GiNaC::realsymbol(GetName()+"_"+attrib) ) );

	return;

};

/***********************************************************/
GiNaC::symbol     Prototype::GetAttribSymbol(string attrib) {

	map<string,GiNaC::symbol>::iterator it = m_attrib_symbol.find(attrib);

	if (it != m_attrib_symbol.end() )
	 	return it->second;

	else {
		SetAttribSymbol(attrib);
		return GetAttribSymbol(attrib);
	}

};

/***********************************************************/
bool            Prototype::HasDOMattribute    (const string attribute) {
	string dummy;
	return GetAttribute(dummy,attribute);
};

/***********************************************************/
bool		Prototype::ReplaceString(string& str, const string& s1, const string& s2) {

	bool ret = false;
	string::size_type loc = 0;

	for (;;) {

        loc = str.find( s1 , loc );

		if (loc == string::npos ) break;

		str.replace(str.find(s1,loc),s1.size(),s2) ;
		ret = true;

	}

	return ret;

};

/***********************************************************/
bool    Prototype::SetExpression (string attrib, string sexpr, PrepareMode mode) {

    //nothing to link -> return false
    if (m_observed_modules.empty() || m_observed_attribs.empty()) return false;

    //clear the Subject list of this attribute
    m_attrib_subject.erase(attrib);

    //clear the symlist of this attribute's expression
    GiNaC::lst symlist;

    //find every occurence of 'step' and replace it with 'csgn' (no GiNaC step function in versions < 1.4)
    ReplaceString(sexpr,"step","csgn");

    //find occurence of 'I' and remember this expression has an imaginary part
    if (sexpr.find("I", 0)!=string::npos) m_has_imag_part = true;

    //replace strings "a1", "a2", ... with the corresponding Name_Attribute string
    int min = (m_observed_modules.size()<m_observed_attribs.size()?m_observed_modules.size():m_observed_attribs.size());

    for (int i=0; i<min; i++) {

        Prototype* Msubj   = m_observed_modules.at(i);
        string     Asubj   = m_observed_attribs.at(i);
        string  SymbolName = Msubj->GetName()+"_"+Asubj;

        stringstream sAttrString;
        sAttrString << "a" << i+1;

        if (!ReplaceString(sexpr,sAttrString.str(),SymbolName)) continue;
        //observe this attribute
        //if ( attrib != "Shape" && attrib != "NLG_field"         )
        Observe( Asubj, Msubj, attrib );
        //add subject to the symbol list
        m_attrib_subject.insert( pair <string, ObsModAttr>(attrib,ObsModAttr(Msubj,Asubj)) );
        symlist.append( Msubj->GetAttribSymbol(Asubj) );

    }

    //the attribute's value was not an expression, since no "a{i}" is a substring
    if ( symlist.nops() == 0 ) return false;

    //debug output
    DEBUG_PRINT("The attribute " << attrib << " of module " << GetName() << " has GiNaC expression " << sexpr
               << " which links to other modules/attributes via the GiNaC symbols " << symlist << endl;)

    //insert expression of this attribute to the map
    try {

        if (m_attrib_expr_str.find(attrib) != m_attrib_expr_str.end() )
            m_attrib_expr_str.erase(attrib);
        m_attrib_expr_str.insert( pair<string,string> (attrib,sexpr) );
        if (m_attrib_expr_dif.find(attrib) != m_attrib_expr_dif.end() )
            m_attrib_expr_dif.erase(attrib);
        m_attrib_expr_dif.insert( pair<string,int> (attrib,0) );
	//test, if expression can be build
	GiNaC::ex(sexpr,symlist);


    } catch (exception &p) {

        if ( mode == PREP_VERBOSE )
            cout	<< "Warning in " << GetName() << "::SetExpression -> attribute " << attrib
                << " can not evaluate the expression " << sexpr << ". Reason: " << p.what() << endl;

        return false;

    }

   return true;

}

/***********************************************************/
void    Prototype::EvalExpressions (string attrib,PrepareMode mode) {

	map<string,string>::iterator is = m_attrib_expr_str.find(attrib);
	if (is == m_attrib_expr_str.end() ) return;
	string sexpr = is->second;

	//symbolic differentiation of this expression?
	int dif = 0;
	map<string,int>::iterator id = m_attrib_expr_dif.find(attrib);
	if (id != m_attrib_expr_dif.end() ) dif = id->second;

	//Loop over all subjects (<Module*,attribute> pairs) for this attribute
	//to find symbols and values of the linked attributes, in order to substitute them
	//into the GiNaC::expression of this attribute.
	pair<multimap< string , ObsModAttr > ::iterator, multimap< string ,ObsModAttr > ::iterator> itp;
	itp = m_attrib_subject.equal_range(attrib);

	multimap< string , ObsModAttr >::iterator it;
	GiNaC::lst symlist;
	GiNaC::lst numlist;

	for (it=itp.first; it!=itp.second; ++it) {

		Prototype* Msubj  = it->second.first  ;
		string     Asubj  = it->second.second ;
		string t          = Msubj->GetAttribTypeID(Asubj);
		void*  p          = Msubj->GetAttribAddress(Asubj);
		string SymbolName = Msubj->GetName()+"_"+Asubj;
		GiNaC::symbol sym = Msubj->GetAttribSymbol(Asubj);
		symlist.append( sym );

		if (t==typeid(double*).name()) {
			double d =  *((double*)   p);
			if (isnan(d)) return;
			numlist.append( d );
			continue;

		}

		if (t==typeid(int*).name())	{

			int d =  *((int*)   p);
			if (isnan(d)) return;
			numlist.append( d );
			continue;

		}

		if (t == typeid(long*).name()) {

			long d =  *((long*)   p);
			if (isnan(d)) return;
			numlist.append( d );
			continue;

		}

		if ( t == typeid(unsigned*).name()) {

			unsigned d =  *((unsigned*)   p);
			if (isnan(d)) return;
			numlist.append( d );
			continue;

		}

	}
	//the expression
	GiNaC::ex e(sexpr,symlist);
	//time derrivative ?
	if (dif>0) {
		e = e.diff(GetAttribSymbol("AnalyticTime"),dif);
		DEBUG_PRINT("\n (d/dt)^"<< idiff << " E = " << e << endl << endl;)
	}

	//numeric evaluation of GiNaC expression for this attribute
	e = e.subs(symlist,numlist);
	m_static_vector = &m_vector; // (static pointer to evaluate the Vector function)
	e = GiNaC::evalf(e);

	double d = 0.0;
	if (GiNaC::is_a<GiNaC::numeric>(e)) {
		if ( m_has_imag_part ) //remember imaginary part, if exists
			m_imag_part = -0.5 * GiNaC::ex_to<GiNaC::numeric>( (e-e.conjugate())*GiNaC::I ).to_double();
		d = GiNaC::ex_to<GiNaC::numeric>( e ).to_double();//default is real-part
	} else if (mode==PREP_VERBOSE) {
	    cout	<< GetName() << ": GiNaC numeric evaluation problems for attribute "
				<< attrib << "=" << e << " " << endl;
	};

	string t = GetAttribTypeID(attrib);
	void* p  = GetAttribAddress(attrib);

	if (t==typeid(double*).name()  ) {*((double*)   p) = ((double)   d );}
	if (t==typeid(int*).name()     ) {*((int*)      p) = ((int)      d );}
	if (t==typeid(long*).name()    ) {*((long*)     p) = ((long)     d );}
	if (t==typeid(unsigned*).name()) {*((unsigned*) p) = ((unsigned) d );}

	Notify(p); //Notify other modules about change of this attribute

	return ;

}

/***********************************************************/
bool Prototype::WriteAttribAddress (string attribute,string value) {

	void* p  = GetAttribAddress(attribute);

	if (p==NULL) return true;

	string type  = GetTypeID(p);

	if (type==typeid(double*).name()   ) {*((double*)   p) = atof(value.c_str());      return true;}
	if (type==typeid(float*).name()    ) {*((float*)    p) = atof(value.c_str());      return true;}
	if (type==typeid(int*).name()      ) {*((int*)      p) = atoi(value.c_str());      return true;}
	if (type==typeid(unsigned*).name() ) {*((unsigned*) p) = abs(atoi(value.c_str())); return true;}
	if (type==typeid(bool*).name()     ) {*((bool*)     p) = abs(atoi(value.c_str())); return true;}
	if (type==typeid(long*).name()     ) {*((long*)     p) = atol(value.c_str());      return true;}
	if (type==typeid(string*).name()   ) {*((string*)   p) = value;                    return true;}
	if (type==typeid(PulseAxis*).name()) {
		if (value == "RF"  ) *( (PulseAxis*) p) = AXIS_RF;
		if (value == "GX"  ) *( (PulseAxis*) p) = AXIS_GX;
		if (value == "GY"  ) *( (PulseAxis*) p) = AXIS_GY;
		if (value == "GZ"  ) *( (PulseAxis*) p) = AXIS_GZ;
		if (value == "NONE") *( (PulseAxis*) p) = AXIS_VOID;
		return true;
	}

	return false;

};

/***********************************************************/
string          Prototype::GetTypeID(void* p){

	map<string,void *>::iterator iaddr;

	for (iaddr=m_attrib_addr.begin(); iaddr!=m_attrib_addr.end(); iaddr++)
		if (iaddr->second == p) return GetAttribTypeID(iaddr->first);

	return  "";

};

/***********************************************************/
void Prototype::Observe (string subj_attrib, Prototype* subj_mod, string obs_attrib ){

	ObsModAttr observer(this, obs_attrib);
	subj_mod->AttachObserver(subj_attrib, observer);

};

/***********************************************************/
string          Prototype::GetAttribTypeID(string attrib) {

	map<string,string>::iterator itype = m_attrib_type.find(attrib);
	if ( itype == m_attrib_type.end() ) return ""; //empty string if not member
	return  (itype->second);                       //type of the member variable

};

/***********************************************************/
bool            Prototype::GetAttribute    (string &s, const string attribute) {

	s = StrX(((DOMElement*) m_node)->getAttribute (StrX(attribute).XMLchar())).std_str() ;
	if (s.empty()) return false;
	return true;

};

/***********************************************************/
void Prototype::AttachObserver (string attrib, ObsModAttr observer) {

	//if attrib is listed as "unoberservable", do nothing and silently return
	map<string,bool>::iterator ito = m_attrib_observable.find(attrib);
	if (ito != m_attrib_observable.end())
		if (!ito->second) return;

	//iterators to find all keys==attrib in the AttribObserver multimap
	pair<multimap< string , ObsModAttr > ::iterator, multimap< string , ObsModAttr > ::iterator> itp;
	itp = m_attrib_observer.equal_range(attrib);
	multimap< string , ObsModAttr >::iterator it;

	//check, if this observer is already in the multimap
	for (it=itp.first; it!=itp.second; ++it) {
		ObsModAttr obs = it->second ;
		if ( obs.first == observer.first && obs.second == observer.second ) return;
	}

	//add the (attrib,observer) pair
	m_attrib_observer.insert(pair< string , ObsModAttr >(attrib,observer));

	//create copy of my private member for checking its state
	map<string,string>::iterator itype = m_attrib_type.find(attrib);
	map<string,void*>::iterator  iaddr = m_attrib_addr.find(attrib);
	map<void*,string>::iterator  itest = m_subject_type.find(iaddr->second);

	if (itest == m_subject_type.end()) {

		m_subject_type.insert(pair<void*,string>(iaddr->second,itype->second));

		if (itype->second == typeid(double*).name())       {
			double* p ;
			p = new double(*((double*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(float*).name())        {
			float* p ;
			p = new float(*((float*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(unsigned int*).name()) {
			unsigned int* p ;
			p = new unsigned int(*((unsigned int*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(bool*).name())         {
			bool* p ;
			p = new bool(*((bool*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(int*).name() )         {
			int* p ;
			p = new int(*((int*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(long*).name())         {
			long* p ;
			p = new long(*((long*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(string*).name())       {
			string* p ;
			p = new string(*((string*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

		if (itype->second == typeid(PulseAxis*).name())    {
			PulseAxis* p ;
			p = new PulseAxis(*((PulseAxis*) iaddr->second));
			m_subject_state.insert(pair<void*,void*>( iaddr->second, (void*) p ) );
		}

	}

};
