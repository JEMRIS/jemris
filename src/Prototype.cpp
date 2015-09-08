/** @file Prototype.cpp
 *  @brief Implementation of JEMRIS Prototype
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
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

#include "AtomicSequence.h"


/***********************************************************/
void Prototype::SetName (string name) {

	m_name = name;
	if (m_node == NULL) return;
    ((DOMElement*) m_node)->setAttribute( StrX("Name").XMLchar(), StrX(name).XMLchar() );

}

/***********************************************************/
bool Prototype::ReplaceString(string& str, const string& s1, const string& s2) {

	bool ret = false;
	string::size_type loc = 0;

	for (;;) {

        loc = str.find( s1 , loc );
		if (loc == string::npos ) break;
		str.replace(loc,s1.size(),s2) ;
		loc += s2.size();
		ret = true;

	}

	return ret;

}

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
Attribute*	Prototype::GetAttribute(string name){

	map<string,Attribute*>::iterator iter = m_attributes.find(name);
	if( iter != m_attributes.end() ) return iter->second;
	return NULL;

}

/***********************************************************/
void	Prototype::CopyObservers(Attribute* a1, Attribute* a2){

	if (a1 == NULL || a2 == NULL) return;

	std::vector<Attribute*> o1 = a1->GetObservers();
	std::vector<Attribute*> o2 = a2->GetObservers();

	for (unsigned int i=0; i<o1.size(); i++)
   			a2->AttachObserver(o1[i]);
}

/***********************************************************/
void           Prototype::HideAttribute (string attrib, bool observable){

	Attribute* a  = GetAttribute(attrib);
	if (a!=NULL) {
		a->SetPublic(false);
		a->SetObservable(observable);
	}

}

/***********************************************************/
bool Prototype::Observe (Attribute* attrib, string prot_name, string attrib_name, bool verbose){
 return Observe(attrib,prot_name,attrib_name,prot_name+"_"+attrib_name,verbose);
}

/***********************************************************/
bool Prototype::Observe (Attribute* attrib, string prot_name, string attrib_name, string attrib_keyword, bool verbose){

	Prototype* M = GetPrototypeByAttributeValue("Name",prot_name);
	if (M==NULL ) {
		if (verbose)	cout << GetName() << ": Observe error: module "
							 << prot_name << " does not exist  in sequence\n";
		return false;
	}

	//observed subjects need to be prepared first ...
	if (!M->IsPrepared())  M->Prepare(PREP_INIT);
	//... then get the requested Attribute and add it to the list
	Attribute* a = M->GetAttribute(attrib_name);
	if (a!=NULL) {
    	for(unsigned int i = 0; i < m_obs_attribs.size(); i++)
    	    if ( m_obs_attribs.at(i) == a ) m_obs_attribs.erase(m_obs_attribs.begin()+i);
		m_obs_attribs.push_back(a);
		for(unsigned int i = 0; i < m_obs_attrib_keyword.size(); i++)
			if ( m_obs_attrib_keyword.at(i) == attrib_keyword ) m_obs_attrib_keyword.erase(m_obs_attrib_keyword.begin()+i);
		m_obs_attrib_keyword.push_back(attrib_keyword);
		if ( attrib !=NULL && attrib->IsObservable()  ) { attrib->AttachSubject(a);	}
	}
	else {
		if (verbose)
			cout << GetName() << ": Observe error: module "
			     << M->GetName() << " has no attribute " << attrib_name << endl;
		return false;
	}

	return true;

}

/***********************************************************/
bool Prototype::Prepare  (PrepareMode mode){

	//nothing to be done for updating modules
	if (mode==PREP_UPDATE) return(true);

	m_aux = false;
	m_prepared = true;

	bool retval = true;

	//every module must have a Name, may observe other attributes, and may has a Vector
	ATTRIBUTE("Name", m_name);
	UNOBSERVABLE_ATTRIBUTE("Observe");
	UNOBSERVABLE_ATTRIBUTE("Vector" );

	//loop over all attributes
	map<string,Attribute*>::iterator iter;

	for(iter = m_attributes.begin(); iter != m_attributes.end(); iter++) {

	    string     keyword   = iter->first;
	    Attribute* attribute = iter->second;
		string val = GetDOMattribute(keyword); //the attribute value is written to val

		if ( !val.empty() ) {//attribute exists in m_node

			//set the double vector attribute
			if (keyword == "Vector") {

				m_vector.clear();
				vector<string> vp = Tokenize(val," ");

				for (unsigned int i=0; i<vp.size(); i++) m_vector.push_back(atof(vp[i].c_str()));

				continue;
			}

			//set the modules for observation
			if (keyword == "Observe") {

				string obsattriblist=val;
				ReplaceString(obsattriblist," ","");

				//backward compatibility
				// support old XML syntax (JEMRIS version <= 2.8.0)
				// Observe="Module1,Attribute1/Module2,Attribute2/..."
				vector<string> vhelp = Tokenize(val,".");
				if (vhelp.size() == 1) {
					vector<string> vpold = Tokenize(val,"/");
					for (unsigned int i=0;i<vpold.size();i++) {
						vector<string> vsold = Tokenize(vpold[i],",");
						if (vsold.size() == 2 && i==0 ) obsattriblist="";
						stringstream attribkey; attribkey << "a" << i+1;
						obsattriblist += (vsold[0]+"."+vsold[1]+"="+attribkey.str());
						if (i+1<vpold.size()) obsattriblist += ",";
					}
				}

				// new XML syntax (JEMRIS version >= 2.8.1)
				// Observe="Module1.Attribute1=Keyword1,Module2.Attribute2=Keyword2,..."
				m_obs_attribs.clear();
				m_obs_attrib_keyword.clear();
				vector<string> vp = Tokenize(obsattriblist,",");
				for (unsigned int i=0;i<vp.size();i++) {

					vector<string> vs = Tokenize(vp[i],".");
					if (vs.size() != 2) {
						if (mode == PREP_VERBOSE)	cout << GetName() << ": wrong syntax in Observe \n";
						retval = false;
						continue;
					}

					vector<string> va = Tokenize(vs[1],"=");
					if (va.size() != 2) {
						if (mode == PREP_VERBOSE)	cout << GetName() << ": wrong syntax in Observe \n";
						retval = false;
						continue;
					}

					retval = Observe(attribute,vs[0],va[0],va[1],mode == PREP_VERBOSE);
					continue;
				}
				continue; //proceed with next attribute

			}

			//standard case: set my private member via the Attribute (possibly GiNaC evaluation)
			if ( attribute->IsObservable()  ) retval = attribute->SetMember(val, m_obs_attribs, m_obs_attrib_keyword, mode == PREP_VERBOSE);


		}

	}

	return retval;

}

