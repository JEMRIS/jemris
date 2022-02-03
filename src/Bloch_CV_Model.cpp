/** @file Bloch_CV_Model.cpp
 *  @brief Implementation of JEMRIS Bloch_CV_Model
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2020  Tony Stoecker
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

#include "Bloch_CV_Model.h"
#include "DynamicVariables.h"
//MODIF
#include <iostream>
#include <fstream>
//MODIF***


/**********************************************************/
inline static int bloch (realtype rt, N_Vector y, N_Vector ydot, void *pWorld) {

    World* pW = (World*) pWorld;
    DynamicVariables* dv = DynamicVariables::instance();
	double t = (double) rt;

    if (t < 0.0 || t > pW->pAtom->GetDuration()) {
    	// this case can happen when searching for step size; in this area no solution is needed
    	// -> set ydot to any defined value.
    	NV_Ith_S(ydot,AMPL) = 0;
    	NV_Ith_S(ydot,PHASE) = 0;
    	NV_Ith_S(ydot,ZC) = 0;
    	return 0;
    }

	double time = pW->total_time+t;
    double Mxy, phi, Mz; /*cylndrical components of mangetization*/
    double s, c, Mx, My, Mx_dot, My_dot, Mz_dot;
    Mz_dot = 0.0;

    //sample variables:
    double r1 = pW->Values[R1];
    double r2 = pW->Values[R2];
    double m0 = pW->Values[M0];
    double position[3];
    position[0] = pW->Values[XC];position[1]=pW->Values[YC];position[2]=pW->Values[ZC];
    double DeltaB = pW->deltaB;

    // update sample variables if they are dynamic:
    dv->m_Diffusion->GetValue(time, position);
//MODIF
    if(pW->logFile)
    {
        //cout<<"Spin number: "<<pW->SpinNumber<<endl;
        if((pW->SpinNumber==0) && (time==0))  {
            fstream log0("FLOW.log",ios::out|ios::trunc);
            log0.close();
            }
        fstream log1("FLOW.log",ios::out|ios::app);
        log1<<"t "<<time<<"  "<<"spin"<<pW->SpinNumber<<" position: "<<position[0]<<" "<<position[1]<<" "<<position[2]<<" Activation: "<<dv->m_Flow->spinActivation(pW->SpinNumber)<<endl;
        log1.close();
    }
    long trajNumber=pW->getTrajBegin()+pW->SpinNumber;
    dv->m_Flow->GetValue(time, position, trajNumber);
    if(pW->logFile)
    {
        fstream log2("FLOW.log",ios::out|ios::app);
        log2<<"t "<<time<<"  "<<"spin"<<pW->SpinNumber<<" position: "<<position[0]<<" "<<position[1]<<" "<<position[2]<<" Activation: "<<dv->m_Flow->spinActivation(pW->SpinNumber)<<endl<<endl;
        log2.close();
    }
    if(pW->logTrajectories)
    {
        if((pW->SpinNumber==0) && (time==0))  {
            fstream logTraj0("trajectories.log",ios::out|ios::trunc);
            logTraj0.close();
            }
        if(dv->m_Flow->spinActivation(pW->SpinNumber))  {
            fstream logTraj("trajectories.log",ios::out|ios::app);
            if(time==0)  logTraj<<endl;
            logTraj<<pW->SpinNumber<<" "<<time<<" "<<position[0]<<" "<<position[1]<<" "<<position[2]<<endl;
            logTraj.close();
            }
    }
//MODIF***
//Mod
    dv->m_Respiration->GetValue(time, position);
//
    dv->m_Motion->GetValue(time, position);
    dv->m_T2prime->GetValue(time, &DeltaB);
    dv->m_R1->GetValue(time, &r1);
    dv->m_R2->GetValue(time, &r2);
    dv->m_M0->GetValue(time, &m0);

//MODIF
    //check spin active: if not, set transv. magnetization to 0
    if (! dv->m_Flow->spinActivation(pW->SpinNumber)) {
    	NV_Ith_S(y,AMPL) = 0;
    	NV_Ith_S(ydot,PHASE) = 0;
    	NV_Ith_S(ydot,ZC) = 0;
    	return 0;
    }
//MODIF***

    //get current B-field values from the sequence
    double  d_SeqVal[5]={0.0,0.0,0.0,0.0,0.0};									// [B1magn,B1phase,Gx,Gy,Gz]
    pW->pAtom->GetValue( d_SeqVal, t );        								    // calculates also pW->NonLinGradField
    if (pW->pStaticAtom != NULL) pW->pStaticAtom->GetValue( d_SeqVal, time );	// calculates static offsets
    pW->pAtom->GetValueLingeringEddyCurrents(d_SeqVal,t);					    // calculates lingering eddy currents

    double Bx, By, Bz;

    //Transverse Components: RF field
    Bx = d_SeqVal[RF_AMP]*cos(d_SeqVal[RF_PHS]);
    By = d_SeqVal[RF_AMP]*sin(d_SeqVal[RF_PHS]);

    //Longitudinal component: Gradient field and off-resonance contributions
    Bz = position[0]*d_SeqVal[GRAD_X]+ position[1]*d_SeqVal[GRAD_Y]+ position[2]*d_SeqVal[GRAD_Z]
         + DeltaB + pW->ConcomitantField(&d_SeqVal[GRAD_X]) + pW->NonLinGradField;

    //NV_Ith_S is the solution magn. vector with components AMPL,PHASE,ZC
    // check if double precision is still enough for sin/cos:
    if (fabs(NV_Ith_S(y,PHASE))>1e11 ) {
        //important: restrict phase to [0, 2*PI]
        NV_Ith_S(y,PHASE) = fmod (NV_Ith_S(y,PHASE), TWOPI);
    }

    Mxy = NV_Ith_S(y,AMPL);
    phi = NV_Ith_S(y,PHASE);
    Mz  = NV_Ith_S(y,ZC);

    //avoid CVODE warnings (does not change physics!)
    //trivial case: no transv. magnetisation AND no excitation
    if (Mxy<ATOL1*m0 && d_SeqVal[RF_AMP]<BEPS) {

        NV_Ith_S(ydot,AMPL) = 0;
        NV_Ith_S(ydot,PHASE) = 0;

        //further, longit. magnetisation already fully relaxed
        if (fabs(m0 - Mz)<ATOL3) {
        	NV_Ith_S(ydot,ZC) = 0;
            return 0;
        }

    } else {

        //compute cartesian components of transversal magnetization
        c = cos(phi);
        s = sin(phi);
        Mx = c*Mxy;
        My = s*Mxy;

        //compute bloch equations
        Mx_dot =   Bz*My - By*Mz - r2*Mx;
        My_dot = - Bz*Mx + Bx*Mz - r2*My;
        Mz_dot =   By*Mx - Bx*My ;

    	//compute derivatives in cylindrical coordinates
    	NV_Ith_S(ydot,AMPL)  =  c*Mx_dot + s*My_dot;
    	NV_Ith_S(ydot,PHASE) = (c*My_dot - s*Mx_dot) / (Mxy>BEPS?Mxy:BEPS); //avoid division by zero
    }

    //longitudinal relaxation
    Mz_dot +=  r1*(m0 - Mz);
    NV_Ith_S(ydot,ZC) = Mz_dot;

//MODIF
/*  NV_Ith_S(ydot,AMPL) = 0;
    NV_Ith_S(ydot,PHASE) = 0;
    NV_Ith_S(ydot,ZC) = 0;*/
//MODIF***

    return 0;

}

/**********************************************************/
Bloch_CV_Model::Bloch_CV_Model     () : m_tpoint(0) {

    m_world->solverSettings = new nvec;
/*    for (int i=0;i<OPT_SIZE;i++) {m_iopt[i]=0; m_ropt[i]=0.0;}
    m_iopt[MXSTEP] = 100000;
    m_ropt[HMAX]   = 100000.0;// the maximum stepsize in msec of the integrator*/
    m_reltol       = RTOL;

    //cvode2.5:
    // create cvode memory pointer; no mallocs done yet.
    m_cvode_mem = CVodeCreate (CV_ADAMS, CV_FUNCTIONAL);

    // cvode allocate memory.
    // do CVodeMalloc with dummy values y0,abstol once here;
    // -> CVodeReInit can later be used
    N_Vector y0, abstol;
    y0		= N_VNew_Serial(NEQ);
    abstol	= N_VNew_Serial(NEQ);
    ((nvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(NEQ);

    NV_Ith_S(y0, AMPL)  = 0;
    NV_Ith_S(y0, PHASE) = 0;
    NV_Ith_S(y0, ZC)    = 0;

   //MODIF  Define CVODE errors with external file if exists
    /*
    NV_Ith_S(abstol, AMPL)  = ATOL1;
    NV_Ith_S(abstol, PHASE) = ATOL2;
    NV_Ith_S(abstol, ZC)    = ATOL3;  */

    ifstream CVODEfile;
    long int MXSTEP;

    CVODEfile.open("CVODEerr.dat", ifstream::in);
    if (CVODEfile.is_open()) {
        CVODEfile>>m_reltol>>NV_Ith_S(abstol, AMPL)>>NV_Ith_S(abstol, PHASE)>>NV_Ith_S(abstol, ZC)>>MXSTEP;
        cout<<"CVODE file open"<<endl;
    }
    else  {
        m_reltol       = RTOL;
        NV_Ith_S(abstol, AMPL)  = ATOL1;
        NV_Ith_S(abstol, PHASE) = ATOL2;
        NV_Ith_S(abstol, ZC)    = ATOL3;
        MXSTEP=100000;
    }
    //MODIF***

#ifndef CVODE26
    if(CVodeMalloc(m_cvode_mem,bloch,0,y0,CV_SV,m_reltol,abstol) != CV_SUCCESS ) {
    	cout << "CVodeMalloc failed! aborting..." << endl;exit (-1);
    }
    if(CVodeSetFdata(m_cvode_mem, (void *) m_world) !=CV_SUCCESS) {
    	cout << "CVode function data could not be set. Panic!" << endl;exit (-1);
    }

#else
    if(CVodeInit(m_cvode_mem,bloch,0,y0) != CV_SUCCESS ) {
    	cout << "CVodeInit failed! aborting..." << endl;exit (-1);
    }
    if(CVodeSVtolerances(m_cvode_mem, m_reltol, abstol)!= CV_SUCCESS){
    	cout << "CVodeSVtolerances failed! aborting..." << endl;exit (-1);
    }
    if(CVodeSetUserData(m_cvode_mem, (void *) m_world) !=CV_SUCCESS) {
    	cout << "CVode function data could not be set. Panic!" << endl;exit (-1);
    }
#endif

    N_VDestroy_Serial(y0);
    N_VDestroy_Serial(abstol);


    CVodeSetMaxNumSteps(m_cvode_mem, MXSTEP);//MODIF

    // maximum number of warnings t+h = t (if number negative -> no warnings are issued )
    CVodeSetMaxHnilWarns(m_cvode_mem,2);


}

/**********************************************************/
void Bloch_CV_Model::InitSolver    () {

    ((nvec*) (m_world->solverSettings))->y = N_VNew_Serial(NEQ);
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,AMPL )  = m_world->solution[AMPL] ;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,PHASE ) = fmod(m_world->solution[PHASE],TWOPI) ;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,ZC )    = m_world->solution[ZC] ;

    ((nvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(NEQ);
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,AMPL )  = ATOL1*m_accuracy_factor;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,PHASE ) = ATOL2*m_accuracy_factor;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,ZC )    = ATOL3*m_accuracy_factor;

    m_reltol = RTOL*m_accuracy_factor;

//cvode2.5:
    int flag;
#ifndef CVODE26
    flag = CVodeReInit(m_cvode_mem,bloch,0,((nvec*) (m_world->solverSettings))->y,CV_SV,m_reltol,
                       ((nvec*) (m_world->solverSettings))->abstol);
#else
    flag = CVodeReInit(m_cvode_mem,0,((nvec*) (m_world->solverSettings))->y);
#endif
    if(flag != CV_SUCCESS ) {
    	cout << "CVodeReInit failed! aborting..." << endl;
    	if (flag == CV_MEM_NULL) cout << "MEM_NULL"<<endl;
    	if (flag == CV_NO_MALLOC) cout << "CV_NO_MALLOC"<<endl;
    	if (flag == CV_ILL_INPUT) cout << "CV_ILL_INPUT"<<endl;

    	exit (-1);
    }


}

/**********************************************************/
void Bloch_CV_Model::FreeSolver    () {

	N_VDestroy_Serial(((nvec*) (m_world->solverSettings))->y     );
	N_VDestroy_Serial(((nvec*) (m_world->solverSettings))->abstol);

}

/**********************************************************/
bool Bloch_CV_Model::Calculate(double next_tStop){

	if ( m_world->time <= 0.0)  m_world->time = RTOL;

	m_world->solverSuccess=true;

	CVodeSetStopTime(m_cvode_mem, next_tStop);

	int flag;
#ifndef CVODE26
	flag = CVode(m_cvode_mem, m_world->time, ((nvec*) (m_world->solverSettings))->y, &m_tpoint, CV_NORMAL_TSTOP);
#else
	do {
		flag=CVode(m_cvode_mem, m_world->time, ((nvec*) (m_world->solverSettings))->y, &m_tpoint, CV_NORMAL);
	} while ((flag==CV_TSTOP_RETURN) && (m_world->time-TIME_ERR_TOL > m_tpoint ));

#endif

	if(flag < 0) { m_world->solverSuccess=false; }

	//reinit needed?
	if (m_world->phase == -2.0 && m_world->solverSuccess) {
#ifndef CVODE26
		CVodeReInit (m_cvode_mem,bloch,m_world->time + TIME_ERR_TOL,((nvec*) (m_world->solverSettings))->y,
                     CV_SV,m_reltol,((nvec*) (m_world->solverSettings))->abstol);
#else
		CVodeReInit(m_cvode_mem,m_world->time + TIME_ERR_TOL,((nvec*) (m_world->solverSettings))->y);
#endif
		// avoiding warnings: (no idea why initial guess of steplength does not work right here...)
		CVodeSetInitStep(m_cvode_mem,m_world->pAtom->GetDuration()/1e9);
	}

	m_world->solution[AMPL]  = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, AMPL );
	m_world->solution[PHASE] = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, PHASE );
	m_world->solution[ZC]    = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, ZC );

	//higher accuracy than 1e-10 not useful. Return success and hope for the best.
	if(m_accuracy_factor < 1e-10) { m_world->solverSuccess=true; }

	return m_world->solverSuccess;
}

/**********************************************************/
void Bloch_CV_Model::PrintFinalStats () {

//    printf("\nFinal Statistics.. \n\n");
//    printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n", m_iopt[NST], m_iopt[NFE] , m_iopt[NSETUPS]);
//    printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n"   , m_iopt[NNI], m_iopt[NCFN], m_iopt[NETF]);

}
