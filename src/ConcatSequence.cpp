/** @file ConcatSequence.cpp
 *  @brief Implementation of JEMRIS ConcatSequence
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
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

#include "ConcatSequence.h"
#include "SequenceTree.h"

/***********************************************************/
ConcatSequence::ConcatSequence  (const ConcatSequence& cs ) {
    m_repetitions = 1;
    m_counter = 0;
}

/***********************************************************/
bool    ConcatSequence::Prepare (const PrepareMode mode){

	m_type = MOD_CONCAT;

	ATTRIBUTE("Repetitions", m_repetitions);
	HIDDEN_ATTRIBUTE("Counter", m_counter);
	if (mode != PREP_UPDATE) GetDuration();

	if (mode != PREP_UPDATE)
		SetRepCounter( 0);

	    return Sequence::Prepare(mode);
}

/***********************************************************/
void   ConcatSequence::SetRepCounter  (unsigned int val,bool record){
	m_counter=val;
	if ( m_counter != GetMyRepetitions() )
		Notify(m_counter);
}

/***********************************************************/
void    ConcatSequence::SetRepetitions (unsigned int val){

   if (val) m_repetitions = val;
}

/***********************************************************/
inline double ConcatSequence::GetDuration () {

	 double duration = 0.;

	vector<Module*> children = GetChildren();

	for (RepIter r=begin(); r<end(); ++r)
		for (unsigned int j=0; j<children.size() ; ++j)
			duration += children[j]->GetDuration();

	m_duration = duration;
	DEBUG_PRINT("  ConcatSequence::GetDuration() of " << GetName() << " calculates  duration = " << duration << endl;)

	Notify(m_duration);

	return duration;
}

/***********************************************************/
int  ConcatSequence::GetNumOfTPOIs (){

	int ntp = 0;
	vector<Module*> children = GetChildren();

	for (RepIter r=begin(); r<end(); ++r)
		for (unsigned int j=0; j<children.size() ; ++j)
			ntp += children[j]->GetNumOfTPOIs() ;

	return ntp;

}

/***********************************************************/
void  ConcatSequence::GetValue (double * dAllVal, double const time) {

	if (time < 0.0 || time > m_duration) { return ; }

	double dRemTime  =  time;
	vector<Module*> children = GetChildren();

	for (RepIter r=begin(); r<end(); ++r)
		for (unsigned int j=0; j<children.size() ; ++j) {

			if (dRemTime < children[j]->GetDuration()) {
				children[j]->GetValue(dAllVal,dRemTime);
				return ;
			}
		dRemTime -= children[j]->GetDuration();
	}

	cout << "???" << endl; //this should never happen !!!

}

/***********************************************************/
string          ConcatSequence::GetInfo() {
	stringstream s;
	s << " Repetitions = " << m_repetitions;
	return s.str();
}

/***********************************************************/
void ConcatSequence::CollectSeqData(NDData<double>& seqdata, double& t, long& offset) {

	vector<Module*> children = GetChildren();

	for (RepIter r=begin(); r<end(); ++r)
		for (unsigned int j=0; j<children.size() ; ++j)
			((Sequence*) children[j])->CollectSeqData(seqdata, t, offset);

}

