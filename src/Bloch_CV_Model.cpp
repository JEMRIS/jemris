/** @file Bloch_CV_Model.cpp
 *  @brief Implementation of JEMRIS Bloch_CV_Model
 */

/*
 *  JEMRIS Copyright (C) 2007-2009  Tony Stöcker, Kaveh Vahedipour
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

#include "Bloch_CV_Model.h"
#include "Trajectory.h"

/**********************************************************/
static int bloch (realtype t, N_Vector y, N_Vector ydot, void *pWorld) {

	t = (double) t;
    double Mxy,phi,Mz; /*cylndrical components of mangetization*/
    double s,c,Mx,My,Mx_dot,My_dot,Mz_dot,DeltaB;
    Mz_dot = 0.0;

    World* pW = (World*) pWorld;

    //get current B-field values from the sequence
    if (t<0.0 || t>pW->pAtom->GetDuration()) {
    	// this case can happen when searching for step size; in this area no solution is needed
    	// -> set ydot to any defined value.
    	NV_Ith_S(ydot,AMPL) = 0; NV_Ith_S(ydot,PHASE) = 0; NV_Ith_S(ydot,ZC) = 0;
    	return 0;
    }

    double  d_SeqVal[5]={0.0,0.0,0.0,0.0,0.0}; //[B1magn,B1phase,Gx,Gy,Gz]
    pW->pAtom->GetValue( d_SeqVal, t );        // calculates also pW->NonLinGradField
    if (pW->pStaticAtom != NULL) pW->pStaticAtom->GetValue( d_SeqVal, t );        // calculates also pW->NonLinGradField
    double Bx=0.0,By=0.0,Bz=0.0;

    //RF field
    Bx = d_SeqVal[RF_AMP]*cos(d_SeqVal[RF_PHS]);
    By = d_SeqVal[RF_AMP]*sin(d_SeqVal[RF_PHS]);

    //Gradient field
    Bz = pW->Values[XC]*d_SeqVal[GRAD_X]+ pW->Values[YC]*d_SeqVal[GRAD_Y]+ pW->Values[ZC]*d_SeqVal[GRAD_Z];

    DeltaB = pW->deltaB;
    // Variable T2Prime:
    if (pW->m_VarT2Prime != NULL) {
    	//deltab == 0.001*m_val[DB] + tan(PI*(m_rng.uniform()-.5))*m_r2prime
    	double dummy = 0.001*pW->Values[DB];
    	DeltaB = (DeltaB - dummy)/ pW->m_VarT2Prime->GetData(pW->total_time + t) + dummy;
    }

    //other off-resonance contributions
    Bz += DeltaB + pW->ConcomitantField(&d_SeqVal[GRAD_X]) + pW->NonLinGradField;

    //NV_Ith_S is the solution magn. vector with components AMPL,PHASE,ZC
    //important: restrict phase to [0, 2*PI]
    NV_Ith_S(y,PHASE) = fmod(NV_Ith_S(y,PHASE),6.28318530717958);
    Mxy = NV_Ith_S(y,AMPL); phi = NV_Ith_S(y,PHASE); Mz = NV_Ith_S(y,ZC);

    //avoid CVODE warnings (does not change physics!)

    //trivial case: no transv. magnetisation AND no excitation
    if (Mxy<ATOL1 && d_SeqVal[RF_AMP]<BEPS) {
        NV_Ith_S(y,AMPL)    = 0; NV_Ith_S(y,PHASE)    = 0;
        NV_Ith_S(ydot,AMPL) = 0; NV_Ith_S(ydot,PHASE) = 0;

        //further, longit. magnetisation already fully relaxed
        if (fabs(pW->Values[M0] - Mz)<ATOL3) {
	    NV_Ith_S(y,ZC)    = pW->Values[M0]; NV_Ith_S(ydot,ZC) = 0;
            return 0;
        }

    } else {

        //compute cartesian components of transversal magnetization
        c = cos(phi);
        s = sin(phi);
        Mx = c*Mxy;
        My = s*Mxy;

        //compute bloch equations
        Mx_dot = -pW->Values[R2]*Mx + Bz*My                                              - By*Mz;
        My_dot = -Bz*Mx             - pW->Values[R2]*My                                  + Bx*Mz;
        Mz_dot =  By*Mx             - Bx*My ;

    	//compute derivatives in cylindrical coordinates
    	NV_Ith_S(ydot,AMPL)  = c*Mx_dot + s*My_dot;
    	NV_Ith_S(ydot,PHASE) = (c*My_dot - s*Mx_dot) / (Mxy>BEPS?Mxy:BEPS); //avoid division by zero
    }

    //longitudinal relaxation
    Mz_dot +=  pW->Values[R1]*(pW->Values[M0] - Mz);
    NV_Ith_S(ydot,ZC)    = Mz_dot;

    return 0;

}

/**********************************************************/
Bloch_CV_Model::Bloch_CV_Model     () {

    m_world->solverSettings = new nvec;
/*    for (int i=0;i<OPT_SIZE;i++) {m_iopt[i]=0; m_ropt[i]=0.0;}
    m_iopt[MXSTEP] = 100000;
    m_ropt[HMAX]   = 100000.0;// the maximum stepsize in msec of the integrator*/
    m_reltol       = RTOL;

    //cvode2.5:
    // create cvode memory pointer; no mallocs done yet.
    m_cvode_mem = CVodeCreate(CV_ADAMS,CV_FUNCTIONAL);

    // cvode allocate memory.
    // do CVodeMalloc with dummy values y0,abstol once here;
    // -> CVodeReInit can later be used
    N_Vector y0,abstol;
    y0		= N_VNew_Serial(NEQ);
    abstol	= N_VNew_Serial(NEQ);
    ((nvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(NEQ);

    NV_Ith_S(y0,AMPL) = 0; NV_Ith_S(y0,PHASE) = 0; NV_Ith_S(y0,ZC) = 0;
    NV_Ith_S(abstol,AMPL) = ATOL1; NV_Ith_S(abstol,PHASE) = ATOL2; NV_Ith_S(abstol,ZC) = ATOL3;

    if(CVodeMalloc(m_cvode_mem,bloch,0,y0,CV_SV,m_reltol,abstol) != CV_SUCCESS ) {
    	cout << "CVodeMalloc failed! aborting..." << endl;
    	exit (-1);
    }
    N_VDestroy_Serial(y0);
    N_VDestroy_Serial(abstol);

    if(CVodeSetFdata(m_cvode_mem, (void *) m_world) !=CV_SUCCESS) {
    	cout << "CVode function data could not be set. Panic!" << endl;
    	exit (-1);
    }

    CVodeSetMaxNumSteps(m_cvode_mem, 100000);

    // maximum number of warnings t+h = t (if number negative -> no warnings are issued )
    CVodeSetMaxHnilWarns(m_cvode_mem,2);


}

/**********************************************************/
void Bloch_CV_Model::InitSolver    () {

    ((nvec*) (m_world->solverSettings))->y = N_VNew_Serial(NEQ);
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,AMPL )  = m_world->solution[AMPL] ;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,PHASE ) = m_world->solution[PHASE] ;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->y,ZC )    = m_world->solution[ZC] ;

    ((nvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(NEQ);
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,AMPL )  = ATOL1;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,PHASE ) = ATOL2;
    NV_Ith_S( ((nvec*) (m_world->solverSettings))->abstol,ZC )    = ATOL3;


//cvode2.5:
    int flag;
    flag = CVodeReInit(m_cvode_mem,bloch,0,((nvec*) (m_world->solverSettings))->y,CV_SV,m_reltol,((nvec*) (m_world->solverSettings))->abstol);
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
void Bloch_CV_Model::Calculate(double next_tStop){

	if ( m_world->time < RTOL)
	    m_world->time += RTOL;

	CVodeSetStopTime(m_cvode_mem,next_tStop);

	int flag;
	flag = CVode(m_cvode_mem, m_world->time, ((nvec*) (m_world->solverSettings))->y, &m_tpoint, CV_NORMAL_TSTOP);

	//reinit needed?
	if (m_world->phase == -2.0) {
		CVodeReInit(m_cvode_mem,bloch,m_world->time + TIME_ERR_TOL,((nvec*) (m_world->solverSettings))->y,CV_SV,m_reltol,((nvec*) (m_world->solverSettings))->abstol);
		// avoiding warnings: (no idea why initial guess of steplength does not work right here...)
		CVodeSetInitStep(m_cvode_mem,m_world->pAtom->GetDuration()/1e9);
	}

	m_world->solution[AMPL]  = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, AMPL );
    m_world->solution[PHASE] = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, PHASE );
    m_world->solution[ZC]    = NV_Ith_S(((nvec*) (m_world->solverSettings))->y, ZC );

};

/**********************************************************/
void Bloch_CV_Model::PrintFinalStats () {

//    printf("\nFinal Statistics.. \n\n");
//    printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n", m_iopt[NST], m_iopt[NFE] , m_iopt[NSETUPS]);
//    printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n"   , m_iopt[NNI], m_iopt[NCFN], m_iopt[NETF]);

};
