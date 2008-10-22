/** @file DelayAtomicSequence.cpp
 *  @brief Implementation of JEMRIS DelayAtomicSequence
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

#include "DelayAtomicSequence.h"
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
	m_mod_start  = NULL;
	m_mod_stop   = NULL;

};

/***********************************************************/
bool DelayAtomicSequence::Prepare (PrepareMode mode) {

    bool b=true;

    ATTRIBUTE("Delay"    , &m_await_time);
    ATTRIBUTE("ADCs"     , &m_adc       );
    ATTRIBUTE("PhaseLock", &m_phase_lock);
    ATTRIBUTE("StartSeq" , &m_start     );
    ATTRIBUTE("StopSeq"  , &m_stop      );
    ATTRIBUTE("DelayType", &m_delay_type);

    //insert pulse and module-attributes for observation only once
    if (mode == PREP_INIT) {
		
		//insert empty pulse
        if (GetNumberOfChildren()==0) 
			b =InsertChild("EmptyPulse");
		
		//add atributes to link with durations of other modules
		for (int i=0;i<20;i++) {
			char modules[10];
			sprintf( modules, "Module%02d", i ) ;
			UNOBSERVABLE_ATTRIBUTE(modules, &m_durations[i] );
		}
    }

    //set delaytype for a quick later check in PREP_UPDATE
    if (mode != PREP_UPDATE) {
		if (m_delay_type == "B2E") m_dt = DELAY_B2E;
		if (m_delay_type == "C2E") m_dt = DELAY_C2E;
		if (m_delay_type == "B2C") m_dt = DELAY_B2C;
		if (m_delay_type == "C2C") m_dt = DELAY_C2C;
    }

    b            = ( SearchStartStopSeq() && b);
    double delay = GetDelay(mode);
    b            = (delay >= 0.0 && b);

    if (GetNumberOfChildren()>0) {
		((Pulse*) GetChild(0))->SetNADC(m_adc);  //pass my ADCs to the EmptyPulse
		((Pulse*) GetChild(0))->SetPhaseLock(m_phase_lock);
		((Pulse*) GetChild(0))->SetDuration(delay);
		if (mode != PREP_UPDATE)
			((Pulse*) GetChild(0))->SetName("eP_"+GetName());
    }

    b = ( AtomicSequence::Prepare(mode) && b);

    // Hide XML attributes which were set by AtomicSequence::Prepare()
    // delay atoms don't need a rot-matrix.
    if (mode != PREP_UPDATE) {
		HideAttribute("RotAngle",false);
		HideAttribute("Inclination",false);
		HideAttribute("Azimut",false);
    }

    if (!b && mode == PREP_VERBOSE)
		cout << "Preparation of DelayAtomicSequence '" << GetName() << "' not succesful. Delay = " << delay << " ms" << endl;

    return b;

}

/***********************************************************/
double DelayAtomicSequence::GetDelay(PrepareMode mode) {
	
	double dDelayTime = m_await_time;
	
	//if (m_mod_start==NULL && m_mod_stop==NULL) return dDelayTime;
	
	Module* pMod =  GetParent();
	if (pMod == NULL) return -1.0;
	
	//find other sequences between pModStart, myself, and pModStop
	int iMYpos=0, iS1pos=10000, iS2pos=-1;
	for (int i=0;i<pMod->GetNumberOfChildren();++i) {
		if(       this == pMod->GetChild(i)) iMYpos=i;
		if(m_mod_start == pMod->GetChild(i)) iS1pos=i;
		if(m_mod_stop  == pMod->GetChild(i)) iS2pos=i;
	}
	iS1pos = (iMYpos<iS1pos)?iMYpos:iS1pos;
	iS2pos = (iMYpos>iS2pos)?iMYpos:iS2pos;
	
	//Observe these sequences
	for (int i=iS1pos;i<=iS2pos;++i)
		if (i!=iMYpos && mode == PREP_VERBOSE) {
			stringstream module; module << "Module" << i;
			Observe("Duration",pMod->GetChild(i),module.str() );
		}
	
	//subtract duration of other sequences between pModStart, myself, and pModStop
	//do this twice, since cross-dependencies may hinder success in first attempt
	for (int j=0;j<2;j++) {
		dDelayTime = m_await_time;
		for (int i=iS1pos;i<=iS2pos;++i) {
			double dfact = ( ( i==iS2pos && (m_dt==DELAY_B2C || m_dt==DELAY_C2C) ) ||
							 ( i==iS1pos && (m_dt==DELAY_C2E || m_dt==DELAY_C2C) )   )?0.5:1.0;
			if (i!=iMYpos)
				dDelayTime -= dfact * pMod->GetChild(i)->GetDuration();
			
		}
	}

	
#ifdef DEBUG
	cout	<< "  DELAYTOMICSEQUENCE: " << GetName() << " mode = " << mode << " , m_await_time = " << m_await_time
			<< " , (iS1pos, iMYpos, iS2pos) = (" << iS1pos << "," << iMYpos << "," << iS2pos << ")"
			<< "  =>  delay = " << dDelayTime << endl;
#endif
	
	
	return dDelayTime;
		
}

/***********************************************************/
bool DelayAtomicSequence::SearchStartStopSeq () {

    m_mod_start = NULL;
    m_mod_stop  = NULL;
	
    Module* pMod = GetParent();
    if (pMod == NULL) return false;
	
    int i1=0, i2=pMod->GetNumberOfChildren();
    for (int i=0;i<pMod->GetNumberOfChildren();++i) {
        if( m_start == pMod->GetChild(i)->GetName() ) { m_mod_start = pMod->GetChild(i); i1=i; }
        if( m_stop  == pMod->GetChild(i)->GetName() ) { m_mod_stop  = pMod->GetChild(i); i2=i; }
    }

    return (i1<i2);

}

/***********************************************************/
string          DelayAtomicSequence::GetInfo () {

	string ret;
	
	switch (m_dt) {
	case DELAY_B2E : ret=" DelayType = B2E "; break;
	case DELAY_C2C : ret=" DelayType = C2C "; break;
	case DELAY_B2C : ret=" DelayType = B2C "; break;
	case DELAY_C2E : ret=" DelayType = C2E "; break;
	default: ret=" unkown DelayType ";
	}
	
	if (!m_start.empty()) ret = ret+" , StartSeq = "+m_start;
	if (!m_stop.empty() ) ret = ret+" , StopSeq = "+m_stop;
	
	return ret;
	
};

