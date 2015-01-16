/** @file Container.cpp
 *  @brief Implementation of JEMRIS Container
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#include "Container.h"
#include "ContainerSequence.h"
#include "SequenceTree.h"

/***********************************************************/
Container::Container  (const Container& as) {
	m_container_seq_name = "";
	m_container_seq = NULL;
	m_container_seqtree = NULL;
}

/***********************************************************/
Container::~Container  () {
	if (m_container_seqtree !=NULL) delete m_container_seqtree;
}

/***********************************************************/
bool Container::Prepare (const PrepareMode mode) {

    bool b=true;
    ATTRIBUTE("Filename", m_container_seq_name);

    //read ContainerSequence and build up its sequence tree
    if (m_container_seq==NULL && !m_container_seq_name.empty() ) {
    	cout << GetName() << " tries to load " << m_container_seq_name << endl;
    	m_container_seqtree = new SequenceTree;
    	m_container_seqtree->Initialize(m_container_seq_name);
    	m_container_seqtree->Populate();
    	m_container_seq = m_container_seqtree->GetContainerSequence();
    }

	if (mode != PREP_UPDATE) m_type = MOD_CONTAINER;

    if (m_container_seq!=NULL) {
    	b = m_container_seq->Prepare(mode);
    }

    b = ( Sequence::Prepare(mode) && b);


    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of Container '" << GetName() << "' not successful. " << endl;

    return b;

}

/***********************************************************/
inline double Container::GetDuration () {

	if (m_container_seq==NULL) return 0.;

	m_duration = m_container_seq->GetDuration();

	Notify(m_duration);

	return m_duration;

}

/***********************************************************/
void  Container::GetValue (double * dAllVal, double const time) {

	if (m_container_seq==NULL) return;

	m_container_seq->GetValue(dAllVal,time);

}

/***********************************************************/
int  Container::GetNumOfTPOIs () {

	if (m_container_seq==NULL) return 0;

	return m_container_seq->GetNumOfTPOIs();

}

/***********************************************************/
long  Container::GetNumOfADCs () {

	if (m_container_seq==NULL) return 0;

	return m_container_seq->GetNumOfADCs();

}

/***********************************************************/
void Container::CollectSeqData(NDData<double>& seqdata, double& t, long& offset) {

	if (m_container_seq==NULL) return;

	m_container_seq->CollectSeqData(seqdata, t, offset);

}

/***********************************************************/
string          Container::GetInfo () {

	string ret = "ContainerSequence filename: " + m_container_seq_name;

	return ret;

}

