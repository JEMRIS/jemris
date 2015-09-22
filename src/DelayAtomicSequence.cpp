	/** @file DelayAtomicSequence.cpp
 *  @brief Implementation of JEMRIS DelayAtomicSequence
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

#include "DelayAtomicSequence.h"
#include "ConcatSequence.h"
#include "SequenceTree.h"
#include "EmptyPulse.h"

/***********************************************************/
DelayAtomicSequence::DelayAtomicSequence  (const DelayAtomicSequence& as) {

	m_adc        = 0;
	m_delay_time = 0.0;
	m_await_time = 0.0;
	m_phase_lock = false;
	m_delay_type = "B2E";
	m_dt         = DELAY_B2E;
	m_start      = "";
	m_stop       = "";
	m_iMYpos	 = 0;
	m_iS1pos	 = 10000;
	m_iS2pos	 = -1;

}

/***********************************************************/
bool DelayAtomicSequence::Prepare (const PrepareMode mode) {

    bool b=true;
    double delay = 0.;

    ATTRIBUTE("Delay"    , m_await_time);
    ATTRIBUTE("ADCs"     , m_adc       );
    ATTRIBUTE("PhaseLock", m_phase_lock);
    ATTRIBUTE("StartSeq" , m_start     );
    ATTRIBUTE("StopSeq"  , m_stop      );
    ATTRIBUTE("DelayType", m_delay_type);

    //insert pulse and module-attributes for observation only once
    if (mode == PREP_INIT && GetNumberOfChildren()==0)	b =InsertChild("EmptyPulse");

   	Pulse* ep  = ((Pulse*) GetChild(0));

   	//prepare delay calculation
   	if (mode != PREP_UPDATE)	{
   		ep->SetName("eP_"+GetName());
   		b  = ( PrepareDelay(mode) && b);
   	}

   	//calculate delay and pass info to the EmptyPulse
   	delay = GetDelay();
   	ep->SetNADC(m_adc);
   	ep->SetPhaseLock(m_phase_lock);
   	ep->SetDuration(delay);

	//call Prepare of base class
    b = ( AtomicSequence::Prepare(mode) && (delay >= 0.0) && b);

    // Hide XML attributes which were set by AtomicSequence::Prepare()
    // delay atoms don't need a rot-matrix.
    if (mode != PREP_UPDATE) {
		HideAttribute("RotAngle",false);
		HideAttribute("Inclination",false);
		HideAttribute("Azimuth",false);
    }

    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of DelayAtomicSequence '" << GetName() << "' not successful. Delay = " << delay << " ms" << endl;

    return b;

}

/***********************************************************/
bool DelayAtomicSequence::PrepareDelay (const PrepareMode mode) {

    //set delay-type for a quick later check in PREP_UPDATE
	if (m_delay_type == "B2E") m_dt = DELAY_B2E;
	if (m_delay_type == "C2E") m_dt = DELAY_C2E;
	if (m_delay_type == "B2C") m_dt = DELAY_B2C;
	if (m_delay_type == "C2C") m_dt = DELAY_C2C;

    Module* pMod = GetParent();
    if (pMod == NULL) return false;

    int i1 = 0, i2 = pMod->GetNumberOfChildren();

    //find positions of start and stop sequence
	m_iMYpos = 0; m_iS1pos = 10000; m_iS2pos = -1;

    for (int i=0;i<pMod->GetNumberOfChildren();++i) {
        if( m_start == pMod->GetChild(i)->GetName()	)	m_iS1pos=i;
        if( m_stop  == pMod->GetChild(i)->GetName()	)	m_iS2pos=i;
		if( this    == pMod->GetChild(i)			)	m_iMYpos=i;
    }

	m_iS1pos = (m_iMYpos<m_iS1pos)?m_iMYpos:m_iS1pos;
	m_iS2pos = (m_iMYpos>m_iS2pos)?m_iMYpos:m_iS2pos;

	if (m_iS1pos>m_iS2pos) return false;


    //get all sequences from start-sequence to stop-sequence
    int j=0; m_seqs.clear();
	for (int i=m_iS1pos;i<=m_iS2pos;++i) {
		if (i==m_iMYpos) continue;
		m_seqs.push_back( (Sequence* ) pMod->GetChild(i));
		Observe(GetAttribute("Duration"), pMod->GetChild(i)->GetName(),"Duration", mode == PREP_VERBOSE );
	}

	return true;

}

/***********************************************************/
double DelayAtomicSequence::GetDelay() {

	double delay = m_await_time;
	int j=0;

	for (int i=m_iS1pos;i<=m_iS2pos;++i) {
		double dfact = ( ( i==m_iS2pos && (m_dt==DELAY_B2C || m_dt==DELAY_C2C) ) ||
						 ( i==m_iS1pos && (m_dt==DELAY_C2E || m_dt==DELAY_C2C) )   ) ? 0.5:1.0;
		if (i!=m_iMYpos) {
			//cout << " >> " << m_seqs[j]-> GetName() << " : " << dfact*m_seqs[j]->GetDuration() << endl;
			delay -= dfact * m_seqs[j]->GetDuration();
			j++;
		}
	}
	//cout << " delay = " << delay << endl << endl;

	// Round delay time up to nearest 10us
	double delayRounded = round(100.0*fabs(delay))/100.0;
	delay = delay<0.0 ? -delayRounded : delayRounded;

#ifdef DEBUG
	cout	<< "  DELAYTOMICSEQUENCE: " << GetName() << " , m_await_time = " << m_await_time
			<< " , (iS1pos, iMYpos, iS2pos) = (" << m_iS1pos << "," << m_iMYpos << "," << m_iS2pos << ")"
			<< "  =>  delay = " << delay << endl;
#endif

	return delay;

}

/***********************************************************/
string          DelayAtomicSequence::GetInfo () {

	string ret;

	switch (m_dt) {
	case DELAY_B2E : ret=" DelayType = B2E "; break;
	case DELAY_C2C : ret=" DelayType = C2C "; break;
	case DELAY_B2C : ret=" DelayType = B2C "; break;
	case DELAY_C2E : ret=" DelayType = C2E "; break;
	default: ret=" unknown DelayType "; break;
	}

	if (!m_start.empty()) ret = ret+" , StartSeq = "+m_start;
	if (!m_stop.empty() ) ret = ret+" , StopSeq = "+m_stop;

	stringstream s;
	s << " , Delay = " << m_await_time;
	ret = ret + s.str();

	return ret;

}

/***********************************************************/
void DelayAtomicSequence::CollectSeqData(OutputSequenceData *seqdata) {
	if (GetHardwareMode()>=0) {
		vector<Event*> events;
		DelayEvent *delay = new DelayEvent();
		delay->m_delay = (long) round(GetDuration()*1e3);
		events.push_back(delay);
		seqdata->AddEvents(events, GetDuration());
		delete delay;
	}

}
