/** @file ConcatSequence.cpp
 *  @brief Implementation of JEMRIS ConcatSequence
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

#include "ConcatSequence.h"
#include "SequenceTree.h"

/***********************************************************/
ConcatSequence::ConcatSequence  (const ConcatSequence& cs ) {
    m_repetitions = 1;
};

/***********************************************************/
bool    ConcatSequence::Prepare(PrepareMode mode){

	m_type = MOD_CONCAT;

	ATTRIBUTE("Repetitions", &m_repetitions);
	HIDDEN_ATTRIBUTE("Counter"     , &m_counter);

	if (mode != PREP_UPDATE) SetRepCounter(0);

        return Sequence::Prepare(mode);
};

/***********************************************************/
void   ConcatSequence::SetRepCounter  (unsigned int val){
	m_counter=val;
	if ( m_counter != GetMyRepetitions() )
		Notify("Counter"); //always notify, i.e. no Newstate() check
} ;

/***********************************************************/
void    ConcatSequence::SetRepetitions (unsigned int val){

   if (val) m_repetitions = val;
};

/***********************************************************/
double     ConcatSequence::GetDuration  (){

	double duration = 0.0;
	vector<Module*> children = GetChildren();

	for (RepIter r=begin(); r<end(); ++r)
		for (unsigned int j=0; j<children.size() ; ++j)
			duration += children[j]->GetDuration();

	m_duration = duration;
	DEBUG_PRINT("  ConcatSequence::GetDuration() of " << GetName() << " calculates  duration = " << duration << endl;)

	Notify(&m_duration);

	return duration;

};

/***********************************************************/
void  ConcatSequence::GetValue (double * dAllVal, double const time) {

        if (time < 0.0 || time > GetDuration()) { return ; }

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

};

/***********************************************************/
string          ConcatSequence::GetInfo() {

	stringstream s;
	s << " Repetitions = " << m_repetitions;
	return s.str();
};
