/** @file Bloch_McConnell_CV_Model.cpp
 *  @brief Implementation of JEMRIS Bloch_McConnell_CV_Model
 */

/*
 *  JEMRIS Copyright (C)
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#include "Bloch_McConnell_CV_Model.h"

#define EPS		1e-10								//small number to avoid division by 0


/* MT functions ***************************************************/

// exchange of magnetisation between all pools, happens all the time
double exchangeTerms (int ncompartments , int currentPool, double* ler, double M_n[]) {

	double output = 0.0;

	for(int i = 0; i < ncompartments; i++)
		if(i != currentPool)
			output += ler[i*ncompartments+currentPool]*M_n[i];

	return output;

}

// Modified relaxation terms, exchange affects the relaxation
double exchangeConstants (int ncompartments, int currentPool, double* ler){

	double output = 0.0;

	for(int i=0; i< ncompartments; i++)
		if(i != currentPool)
			output += ler[currentPool*ncompartments+i];

	return output;

}

// Compute local exchange rates per spin
void LocalExchangeRates (double* global_exchange_rates, double* local_exchange_rates, double* m0, int no_pools) {

	if (no_pools > 1) { /* upper triangle loops*/

		for (int i = 0; i < no_pools; i++) {
			for (int j = i+1; j < no_pools; j++)
				local_exchange_rates[i*no_pools+j] = global_exchange_rates [i*no_pools+j];
		}

		for (int i = 0; i < no_pools; i++)
			local_exchange_rates[i*no_pools+i] = 0.0;

		for (int i = 0; i < no_pools; i++) { /* lower triangle loops*/
			for (int j = 0;  j < i ; j++) {
				if (m0[i] > 0.0 and m0[j] > 0.0)
					local_exchange_rates[i*no_pools+j] = global_exchange_rates [j*no_pools+i] * m0[j] / m0[i];
				else {
					local_exchange_rates[i*no_pools+j] = 0.0;
					local_exchange_rates[j*no_pools+i] = 0.0;
				}
			}
		}

	} else 
		local_exchange_rates[0] = 0.0;

}

/**********************************************************/
static int bloch (realtype t, N_Vector y, N_Vector ydot, void *pWorld) {

	World*   pW        = (World*) pWorld;
	BMAux*   m_bmaux   = (BMAux*) pW->auxiliary;

	int      m_nprops  = pW->m_noofspinprops;
	int      m_ncomp   = pW->m_noofspincompartments;
	int 	 m_ncoprops = (m_nprops - 4) / m_ncomp;
	double*  m_exrates = m_bmaux->exrates;
	bool     m_single  = m_bmaux->single;

    // Magnetisation for each poosl
	double*   Mxy    = new double [m_ncomp];
	double*   phi    = new double [m_ncomp];
	double*   Mz     = new double [m_ncomp];
	double*   s      = new double [m_ncomp];
	double*   c      = new double [m_ncomp];
	double*   Mx     = new double [m_ncomp];
	double*   My     = new double [m_ncomp];
	double*   Mx_dot = new double [m_ncomp];
	double*   My_dot = new double [m_ncomp];
	double*   Mz_dot = new double [m_ncomp];

    for (int i = 0; i < m_ncomp; i++) {

    	Mx_dot[i] = 0.0;
    	My_dot[i] = 0.0;
		Mz_dot[i] = 0.0;
		Mxy   [i] = 0.0;
		phi   [i] = 0.0;
		Mx    [i] = 0.0;
		My    [i] = 0.0;
		Mz    [i] = 0.0;
		s     [i] = 0.0;
		c     [i] = 0.0;

    }

    //get current B-field values from the sequence
    if (t<=0.0 || t>pW->pAtom->GetDuration()) {		// Kaveh, hier hab ich gemogelt

    	// this case can happen when searching for step size; in this area no solution is needed
    	// -> set ydot to any defined value.
    	for(int i = 0; i< m_ncomp*NEQ; i+=NEQ) {
    		NV_Ith_S(ydot,XC+i) = 0.0; NV_Ith_S(ydot,YC+i) = 0.0; NV_Ith_S(ydot,ZC+i) = 0.0;
		}

    	return 0;
    }

    double  d_SeqVal[5] = {0.0,0.0,0.0,0.0,0.0};                           // [B1magn,B1phase,Gx,Gy,Gz]
    pW->pAtom->GetValue( d_SeqVal, t );                                    // calculates also pW->NonLinGradField
    if (pW->pStaticAtom != NULL) pW->pStaticAtom->GetValue( d_SeqVal, t ); // calculates also pW->NonLinGradField
    double Bx=0.0, By=0.0, Bz=0.0;
    double* BzPool = new double[m_ncomp];

    //RF field
    Bx = d_SeqVal[RF_AMP]*cos(d_SeqVal[RF_PHS]);
    By = d_SeqVal[RF_AMP]*sin(d_SeqVal[RF_PHS]);


    //Gradient field
    Bz = pW->Values[XC]*d_SeqVal[GRAD_X]+ pW->Values[YC]*d_SeqVal[GRAD_Y]+ pW->Values[ZC]*d_SeqVal[GRAD_Z];

    //other off-resonance contributions (differs for every pool)
    for(int i = 0; i< m_ncomp; i++)
    	BzPool[i] = Bz + pW->Values[i*m_ncoprops+DB] + pW->ConcomitantField(&d_SeqVal[GRAD_X]) + pW->NonLinGradField;


    //NV_Ith_S is the solution magn. vector with components AMPL,PHASE,ZC
    //important: restrict phase to [0, 2*PI]
    int pool =0;
   /* for(int i = 0; i< m_ncomp*NEQ; i+=NEQ){		// loop over pools
    	NV_Ith_S(y,PHASE+i) 				= fmod(NV_Ith_S(y,PHASE+i),6.28318530717958);
    	Mxy[pool] = NV_Ith_S(y,AMPL+i); phi[pool] = NV_Ith_S(y,PHASE+i); Mz[pool] = NV_Ith_S(y,ZC+i);
    	pool++;
    }*/

    for (int i = 0; i< m_ncomp*NEQ; i+=NEQ) {		// loop over pools

       	Mx[pool] = NV_Ith_S(y,XC+i); 
		My[pool] = NV_Ith_S(y,YC+i); 
		Mz[pool] = NV_Ith_S(y,ZC+i);
       	pool++;

    }

    pool = 0;
    
	for (int eqpool = 0; eqpool< ((m_ncomp)*NEQ); eqpool+=NEQ) {		// loop over pools, NEQ steps

    	//avoid CVODE warnings (does not change physics!)
    	if (pW->Values[pool*m_ncoprops + M0] == 0.0) {
    		// this pool is empty, the number of pools reduces
    		NV_Ith_S(y,XC+eqpool)    = 0.0; NV_Ith_S(y,YC+eqpool)    = 0.0;
    		NV_Ith_S(ydot,XC+eqpool) = 0.0; NV_Ith_S(ydot,YC+eqpool) = 0.0;
			NV_Ith_S(y,ZC+eqpool)    = 0.0;
    		NV_Ith_S(ydot,ZC+eqpool) = 0.0;

			
    	} else if ((Mx[pool]*Mx[pool] + My[pool]*My[pool]) < ATOL1 * pW->Values[pool*m_ncoprops+M0] && d_SeqVal[RF_AMP]<BEPS) {
        	//trivial case: no transv. magnetisation AND no excitation
    		NV_Ith_S(y,XC+eqpool)    = 0.0; NV_Ith_S(y,YC+eqpool)    = 0.0;
    		NV_Ith_S(ydot,XC+eqpool) = 0.0; NV_Ith_S(ydot,YC+eqpool) = 0.0;

    		//further, longit. magnetisation already fully relaxed
    		if (fabs(pW->Values[pool*m_ncoprops +M0] - Mz[pool])<ATOL3  *pW->Values[pool*m_ncoprops +M0]) {

    			NV_Ith_S(y,ZC+eqpool)    = pW->Values[pool*m_ncoprops +M0];
    			NV_Ith_S(ydot,ZC+eqpool) = 0.0;

    			if(eqpool == (((m_ncomp)*NEQ)-1))
					return 0;

    		}

    	} else {

    		//compute cartesian components of transverse magnetization

    		/*c[pool] 	= cos(phi[pool]);
    		s[pool] 	= sin(phi[pool]);


    		Mx[pool] 	= c[pool]*Mxy[pool];
    		My[pool] 	= s[pool]*Mxy[pool];*/


    		// Built the n-pool bloch equation, check if each pool exists

    		if (m_ncomp > 1 ){  // do this only if really necessary - time consuming!
    			if (m_single == false) {
    				Mx_dot[pool] = -(pW->Values[pool*m_ncoprops +R2] + exchangeConstants(m_ncomp,pool,m_exrates)) *Mx[pool] 		+ BzPool[pool]*My[pool]    - By*Mz[pool] + exchangeTerms(m_ncomp,pool,m_exrates,Mx);
    				My_dot[pool] = - BzPool[pool]*Mx[pool]  - (pW->Values[pool*m_ncoprops +R2]+exchangeConstants(m_ncomp,pool,m_exrates)) *My[pool]  		   + Bx*Mz[pool] + exchangeTerms(m_ncomp,pool,m_exrates,My);
    				Mz_dot[pool] =   By*Mx[pool]            	 																	   - Bx*My[pool] ;
    			} else {
       				Mx_dot[pool] = -(pW->Values[pool*m_ncoprops +R2]) *Mx[pool] 		+ BzPool[pool]*My[pool]    - By*Mz[pool];
        			My_dot[pool] = - BzPool[pool]*Mx[pool]  - (pW->Values[pool*m_ncoprops +R2]) *My[pool]  	   + Bx*Mz[pool];
        			Mz_dot[pool] =   By*Mx[pool]            	 										   - Bx*My[pool] ;
    			}
    		} else {
    			// use single pool model, much faster
    			Mx_dot[pool] = - pW->Values[pool*m_ncoprops +R2] *Mx[pool] 		+ BzPool[pool]*My[pool]    - By*Mz[pool];
				My_dot[pool] = - BzPool[pool]*Mx[pool]  - pW->Values[pool*m_ncoprops +R2] *My[pool]  		   + Bx*Mz[pool];
				Mz_dot[pool] =   By*Mx[pool]              	 										   - Bx*My[pool] ;
    		}

    		//compute bloch equations old version
    		//      Mx_dot = -pW->Values[R2]*Mx + Bz*My                                              - By*Mz;
    		//      My_dot = -Bz*Mx             - pW->Values[R2]*My                                  + Bx*Mz;
    		//      Mz_dot =  By*Mx             - Bx*My ;

    		//compute derivatives in cylindrical coordinates

    		//NV_Ith_S(ydot,AMPL+eqpool)   = c[pool]*Mx_dot[pool] + s[pool]*My_dot[pool];
    		//NV_Ith_S(ydot,PHASE+eqpool)  = (c[pool]*My_dot[pool] - s[pool]*Mx_dot[pool]) / (Mxy[pool]>BEPS?Mxy[pool]:BEPS); //avoid division by zero
    		NV_Ith_S(ydot,XC+eqpool)  = Mx_dot[pool];
    		NV_Ith_S(ydot,YC+eqpool)  = My_dot[pool];


    	}

    	//longitudinal relaxation
		if (m_ncomp > 1){
			if(m_single == false){
				Mz_dot[pool] 			 +=  pW->Values[pool*m_ncoprops +R1]*(pW->Values[pool*m_ncoprops +M0] - Mz[pool]) - exchangeConstants(m_ncomp,pool,m_exrates)*Mz[pool] + exchangeTerms(m_ncomp,pool,m_exrates,Mz);
			} else{
				Mz_dot[pool] 			 +=  pW->Values[pool*m_ncoprops +R1]*(pW->Values[pool*m_ncoprops +M0] - Mz[pool]);
			}
		} else
			Mz_dot[pool] 			 	 +=  pW->Values[pool*m_ncoprops +R1]*(pW->Values[pool*m_ncoprops +M0] - Mz[pool]);


		NV_Ith_S(ydot,ZC+eqpool) = Mz_dot[pool];

		pool++;
    }

	// Kaveh, hier stehen noch unterschiedliche Zahlen in den verschiedenen Pools
	//cout<<NV_Ith_S(ydot,ZC+0) << "  " <<NV_Ith_S(ydot,ZC+3) 		<<endl;

	delete [] Mxy;
	delete [] phi;
	delete [] Mz;
	delete [] s;
	delete [] c;
	delete [] Mx;
	delete [] My;
	delete [] Mx_dot;
	delete [] My_dot;
	delete [] Mz_dot;
	delete [] BzPool;

    return 0;

}

/**********************************************************/
Bloch_McConnell_CV_Model::Bloch_McConnell_CV_Model     () {

    m_world->solverSettings = new bmnvec;
   /* for (int i=0;i<OPT_SIZE;i++) {m_iopt[i]=0; m_ropt[i]=0.0;}
    m_iopt[MXSTEP] = 1000000;
    m_ropt[HMAX]   = 10000.0;// the maximum stepsize in msec of the integrator*/
    m_reltol       = RTOL;

    //cvode2.5:
    // create cvode memory pointer; no mallocs done yet.
 //   m_cvode_mem = CVodeCreate(CV_BDF,CV_NEWTON);
   m_cvode_mem = CVodeCreate(CV_ADAMS,CV_FUNCTIONAL);

    // cvode allocate memory.
    // do CVodeMalloc with dummy values y0,abstol once here;
    // -> CVodeReInit can later be used



    //HACK
    int pools = m_world->GetNoOfCompartments();

    N_Vector y0,abstol;
    y0		= N_VNew_Serial(NEQ*pools);
    abstol	= N_VNew_Serial(NEQ*pools);
    ((bmnvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(pools*NEQ);

    for(int i = 0; i< pools*NEQ; i+=NEQ){
    	NV_Ith_S(y0,AMPL+i) = 0.0; NV_Ith_S(y0,PHASE+i) = 0.0; NV_Ith_S(y0,ZC+i) = 0.0;
    	NV_Ith_S(abstol,AMPL+i) = ATOL1; NV_Ith_S(abstol,PHASE+i) = ATOL2; NV_Ith_S(abstol,ZC+i) = ATOL3;
    }
	
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

	/*    int flag;
	int blub = (3*pools);
	flag = CVDense(m_cvode_mem, blub);
	if (flag == CVDENSE_SUCCESS)
		cout<< "great" <<endl;
	else
		cout<< "bad" <<endl;
	*/

    N_VDestroy_Serial(y0);
    N_VDestroy_Serial(abstol);

    /*if(CVodeSetFdata(m_cvode_mem, (void *) m_world) !=CV_SUCCESS) {
    	cout << "CVode function data could not be set. Panic!" << endl;
    	exit (-1);
    }
	
    // set CVODE initial step size
  //    CVodeSetInitStep(m_cvode_mem, 1e-4);
      // set CVODE maximum step size
      CVodeSetMaxErrTestFails(m_cvode_mem, 10);
      // set CVODE minimum step size
      CVodeSetMinStep(m_cvode_mem, 1e-15);
	*/
    CVodeSetMaxNumSteps(m_cvode_mem, 10000000);

   	// maximum number of warnings t+h = t (if number negative -> no warnings are issued )
   	CVodeSetMaxHnilWarns(m_cvode_mem,2);

}

/**********************************************************/
void Bloch_McConnell_CV_Model::InitSolver    () {

	m_world     = World::instance();
	
	m_ncomp     = m_world->GetNoOfCompartments();
	m_nprops    = m_world->GetNoOfSpinProps();
	int m_ncoprops = (m_nprops - 4) / m_ncomp;
	
	double* m0s = new double[m_ncomp];
	int     n   = 0;
	
	for (int i = 0; i < m_ncomp; i++) {

		m0s[i] = m_world->Values[m_ncoprops*i+M0];
		if (m0s[i] > 0.0)
			n++;
	}

	m_bmaux.Init(m_ncomp); 
	
	if (n == 1)
		m_bmaux.single  = true;


	//Compute local exchange rates and return them in member matrix
	LocalExchangeRates (m_world->Helper(), m_bmaux.exrates, m0s, m_ncomp);

	delete [] m0s;
	
	m_world->auxiliary = (void*) (&m_bmaux);
	
    ((bmnvec*) (m_world->solverSettings))->y = N_VNew_Serial(NEQ*m_ncomp);
	
    // loop over pools, stepsize NEQ
    for ( int i = 0; i < m_ncomp*NEQ; i += NEQ ){

    	NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->y,XC+i )  = m_world->solution[AMPL+i]*cos(m_world->solution[PHASE+i]);// polar coordinates
    	NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->y,YC+i )  = m_world->solution[AMPL+i]*sin(m_world->solution[PHASE+i]);
    	NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->y,ZC+i )  = m_world->solution[ZC+i];

    }
	
    ((bmnvec*) (m_world->solverSettings))->abstol = N_VNew_Serial(NEQ*m_ncomp);
	
    // loop over pools, stepsize NEQ
    int poolVal = 0;
    for ( int i = 0; i< m_ncomp*NEQ; i+=NEQ ){

    	if (fabs(m_world->Values[m_ncoprops*poolVal+M0])<BEPS){

    		NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->abstol,XC+i )  = ATOL1;
			NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->abstol,ZC+i )  = ATOL3;

    	} else{

	  		NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->abstol,XC+i )  = ATOL1*m_world->Values[m_ncoprops*poolVal+M0];
			NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->abstol,ZC+i )  = ATOL3*m_world->Values[m_ncoprops*poolVal+M0];
		}

		NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->abstol,YC+i ) = ATOL2;
		poolVal++;

    }
	
    int flag;
#ifndef CVODE26
    flag = CVodeReInit(m_cvode_mem,bloch,0,((bmnvec*) (m_world->solverSettings))->y,CV_SV,m_reltol,((bmnvec*) (m_world->solverSettings))->abstol);
#else
    flag = CVodeReInit(m_cvode_mem,0,((bmnvec*) (m_world->solverSettings))->y);
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
void Bloch_McConnell_CV_Model::FreeSolver    () {
	
	N_VDestroy_Serial(((bmnvec*) (m_world->solverSettings))->y     );
	N_VDestroy_Serial(((bmnvec*) (m_world->solverSettings))->abstol);
	
}

/**********************************************************/
bool Bloch_McConnell_CV_Model::Calculate(double next_tStop){
	
	if ( m_world->time < RTOL)
	    m_world->time += RTOL;
	
	CVodeSetStopTime(m_cvode_mem,next_tStop);
	cout<<NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->y,ZC ) << " "<< NV_Ith_S( ((bmnvec*) (m_world->solverSettings))->y,ZC+3 )<<endl;
	
	int flag;
#ifndef CVODE26
	flag = CVode(m_cvode_mem, m_world->time, ((bmnvec*) (m_world->solverSettings))->y, &m_tpoint, CV_NORMAL_TSTOP);
#else
	do {
		flag=CVode(m_cvode_mem, m_world->time, ((bmnvec*) (m_world->solverSettings))->y, &m_tpoint, CV_NORMAL);
	} while ((flag==CV_TSTOP_RETURN) && (m_world->time-TIME_ERR_TOL > m_tpoint ));
	
#endif
	if(flag < 0) 
		m_world->solverSuccess=false;
	
	//reinit needed?
	if ( m_world->phase == -2.0 && m_world->solverSuccess ) {

#ifndef CVODE26
		CVodeReInit(m_cvode_mem,bloch,m_world->time + TIME_ERR_TOL,((bmnvec*) (m_world->solverSettings))->y,CV_SV,m_reltol,((bmnvec*) (m_world->solverSettings))->abstol);
#else
		CVodeReInit(m_cvode_mem,m_world->time + TIME_ERR_TOL,((bmnvec*) (m_world->solverSettings))->y);
#endif
		// avoiding warnings: (no idea why initial guess of steplength does not work right here...)
		CVodeSetInitStep(m_cvode_mem,m_world->pAtom->GetDuration()/1e9);

	}



    // loop over pools, stepsize NEQ
    for ( int i = 0; i< m_ncomp*NEQ; i+=NEQ ) {
		
    	double solution_dummy_x = NV_Ith_S(((bmnvec*) (m_world->solverSettings))->y, XC+i );
    	double solution_dummy_y = NV_Ith_S(((bmnvec*) (m_world->solverSettings))->y, YC+i );
		
    	m_world->solution[AMPL+i]  = sqrt(solution_dummy_x*solution_dummy_x + solution_dummy_y*solution_dummy_y);
		
       	if (m_world->solution[AMPL+i] < EPS)
    		m_world->solution[PHASE+i] = 0;
    	else
    		if (solution_dummy_y < 0 )
   				m_world->solution[PHASE+i] = 2*PI - acos(solution_dummy_x / m_world->solution[AMPL+i]);
    		else
    			m_world->solution[PHASE+i] =        acos(solution_dummy_x / m_world->solution[AMPL+i]);
		
    	m_world->solution[ZC+i]    = NV_Ith_S(((bmnvec*) (m_world->solverSettings))->y, ZC+i );

    	// Kaveh, hier sind dann alle pools gleich :(
        //cout << "          bloch solution "<<m_world->solution[ZC+i]<< "      "    <<" i " << i <<endl;
    }


	//higher accuray than 1e-10 not useful. Return success and hope for the best.
	if(m_reltol < 1e-10) 
		m_world->solverSuccess=true;
	
	return m_world->solverSuccess;

}


/**********************************************************/
void Bloch_McConnell_CV_Model::PrintFinalStats () {
	
	//    printf("\nFinal Statistics.. \n\n");
	//    printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n", m_iopt[NST], m_iopt[NFE] , m_iopt[NSETUPS]);
	//    printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n"   , m_iopt[NNI], m_iopt[NCFN], m_iopt[NETF]);
	
}

