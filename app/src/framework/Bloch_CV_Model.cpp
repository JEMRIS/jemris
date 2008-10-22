/** @file Bloch_CV_Model.cpp
 *  @brief Implementation of JEMRIS Bloch_CV_Model
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

#include "Bloch_CV_Model.h"

/**********************************************************/
static void bloch (integer N, double t, N_Vector y, N_Vector ydot, void *pWorld) {

    double Mxy,phi,Mz; /*cylndrical components of mangetization*/
    double s,c,Mx,My,Mx_dot,My_dot,Mz_dot;
    Mz_dot = 0.0;

    World* pW = (World*) pWorld;

    //get current B-field values from the sequence
    if (t<0.0 || t>pW->pAtom->GetDuration()) {
    	// this case can happen when searching for step size; in this area no solution is needed
    	// -> set ydot to any defined value.
    	N_VIth(ydot,AMPL) = 0; N_VIth(ydot,PHASE) = 0; N_VIth(ydot,ZC) = 0;
    	return;
    }

    double  d_SeqVal[5]={0.0,0.0,0.0,0.0,0.0}; //[B1magn,B1phase,Gx,Gy,Gz]
    pW->pAtom->GetValue( d_SeqVal, t );        // calculates also pW->NonLinGradField
    double Bx=0.0,By=0.0,Bz=0.0;

    //RF field
    Bx = d_SeqVal[RF_AMP]*cos(d_SeqVal[RF_PHS]);
    By = d_SeqVal[RF_AMP]*sin(d_SeqVal[RF_PHS]);

    //Gradient field
    Bz = pW->Values[XC]*d_SeqVal[GRAD_X]+ pW->Values[YC]*d_SeqVal[GRAD_Y]+ pW->Values[ZC]*d_SeqVal[GRAD_Z];

    //other off-resonance contributions
    Bz += pW->deltaB + pW->ConcomitantField(&d_SeqVal[GRAD_X]) + pW->NonLinGradField;

    //N_VIth is the solution magn. vector with components AMPL,PHASE,ZC
    //important: restrict phase to [0, 2*PI]
    N_VIth(y,PHASE) = fmod(N_VIth(y,PHASE),6.28318530717958);
    Mxy = N_VIth(y,AMPL); phi = N_VIth(y,PHASE); Mz = N_VIth(y,ZC);

    //avoid CVODE warnings (does not change physics!)

    //trivial case: no transv. magnetisation AND no excitation
    if (Mxy<ATOL1 && d_SeqVal[RF_AMP]<BEPS) {
        N_VIth(y,AMPL)    = 0; N_VIth(y,PHASE)    = 0;
        N_VIth(ydot,AMPL) = 0; N_VIth(ydot,PHASE) = 0;
        //further, longit. magnetisation already fully relaxed
        if (fabs(pW->Values[M0] - Mz)<ATOL3) {
	    N_VIth(y,ZC)    = pW->Values[M0]; N_VIth(ydot,ZC) = 0;
            return;
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
    	N_VIth(ydot,AMPL)  = c*Mx_dot + s*My_dot;
    	N_VIth(ydot,PHASE) = (c*My_dot - s*Mx_dot) / (Mxy>BEPS?Mxy:BEPS); //avoid division by zero

    }

    //longitudinal relaxation
    Mz_dot +=  pW->Values[R1]*(pW->Values[M0] - Mz);
    N_VIth(ydot,ZC)    = Mz_dot;

}

/**********************************************************/
Bloch_CV_Model::Bloch_CV_Model     () {

    m_world->solverSettings = new nvec;
    for (int i=0;i<OPT_SIZE;i++) {m_iopt[i]=0; m_ropt[i]=0.0;}
    m_iopt[MXSTEP] = 100000;
    m_ropt[HMAX]   = 100000.0;// the maximum stepsize in msec of the integrator
    m_reltol       = RTOL;

}

/**********************************************************/
void Bloch_CV_Model::InitSolver    () {

    ((nvec*) (m_world->solverSettings))->y = N_VNew(NEQ, NULL);
    N_VIth( ((nvec*) (m_world->solverSettings))->y,AMPL )  = m_world->solution[AMPL] ;
    N_VIth( ((nvec*) (m_world->solverSettings))->y,PHASE ) = m_world->solution[PHASE] ;
    N_VIth( ((nvec*) (m_world->solverSettings))->y,ZC )    = m_world->solution[ZC] ;

    ((nvec*) (m_world->solverSettings))->abstol = N_VNew(NEQ, NULL);
    N_VIth( ((nvec*) (m_world->solverSettings))->abstol,AMPL )  = ATOL1;
    N_VIth( ((nvec*) (m_world->solverSettings))->abstol,PHASE ) = ATOL2;
    N_VIth( ((nvec*) (m_world->solverSettings))->abstol,ZC )    = ATOL3;


    m_cvode_mem = CVodeMalloc(
            NEQ,
            bloch,
            0,
            ((nvec*) (m_world->solverSettings))->y,
            ADAMS,
            FUNCTIONAL,
            SV,
            &m_reltol,
            ((nvec*) (m_world->solverSettings))->abstol,
            m_world,
            NULL,
            TRUE,
            m_iopt,
            m_ropt,
            NULL
            );

}

/**********************************************************/
void Bloch_CV_Model::FreeSolver    () {

    N_VFree(((nvec*) (m_world->solverSettings))->y     );
    N_VFree(((nvec*) (m_world->solverSettings))->abstol);
    CVodeFree(m_cvode_mem);

}

/**********************************************************/
void Bloch_CV_Model::Calculate(){

	if ( m_world->time < RTOL)
	    m_world->time += RTOL;

	CVode(m_cvode_mem, m_world->time, ((nvec*) (m_world->solverSettings))->y, &m_tpoint, NORMAL);

	m_world->solution[AMPL]  = N_VIth(((nvec*) (m_world->solverSettings))->y, AMPL );
    m_world->solution[PHASE] = N_VIth(((nvec*) (m_world->solverSettings))->y, PHASE );
    m_world->solution[ZC]    = N_VIth(((nvec*) (m_world->solverSettings))->y, ZC );

};

/**********************************************************/
void Bloch_CV_Model::PrintFinalStats () {

    printf("\nFinal Statistics.. \n\n");
    printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n", m_iopt[NST], m_iopt[NFE] , m_iopt[NSETUPS]);
    printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n"   , m_iopt[NNI], m_iopt[NCFN], m_iopt[NETF]);

};
