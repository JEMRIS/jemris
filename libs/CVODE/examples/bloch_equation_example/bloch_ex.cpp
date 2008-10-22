/************************************************************************
 * Example for solving bloch equation with CVODE                        *
 ************************************************************************/

#include <stdio.h>
#include <math.h>
#include "llnltyps.h" /* definitions of types real (set to double) and     */
                      /* integer (set to int), and the constant FALSE      */
#include "cvode.h"    /* prototypes for CVodeMalloc, CVode, and CVodeFree, */
                      /* constants OPT_SIZE, BDF, NEWTON, SV, SUCCESS,     */
                      /* NST, NFE, NSETUPS, NNI, NCFN, NETF                */
#include "nvector.h"  /* definitions of type N_Vector and macro N_VIth,    */

/* Thus macro is defined in order to write code which exactly matches
   the mathematical problem. */
#define Ith(v,i)    N_VIth(v,i-1)         /* Ith numbers components 1..NEQ */

/* Problem Constants */
#define NEQ   3            /* number of equations  */
#define RTOL  1e-4         /* scalar relative tolerance            */
#define ATOL1 1e-6         /* vector absolute tolerance components */
#define ATOL2 1e-6
#define ATOL3 1e-6
#define T0    0.0          /* initial time           */
#define T1    0.1          /* first output time      */
#define TADD  0.1          /* output time factor     */
#define NOUT  20           /* number of output times */
#define PI    3.14159265358979

/*this is used as the f_data of cvode, to pass information to the user function (bloch)*/
typedef struct {real M0,R1,R2;} mr_sample;

/* Private Helper Function */
static void PrintFinalStats(long int iopt[]);

/* Function Called by the CVODE Solver */
static void bloch(integer N, real t, N_Vector y, N_Vector ydot, void *f_data);
static void f(integer N, real t, N_Vector y, N_Vector ydot, void *f_data);

/***************************** Main Program ******************************/
main()
{
  real ropt[OPT_SIZE], reltol, t, tout;
  long int iopt[OPT_SIZE];
  N_Vector y, abstol;
  void *cvode_mem;
  int iout, flag,rep;
  
  mr_sample* psample;
  psample =new mr_sample;
  //psample=malloc(sizeof(mr_sample));
  psample->M0=1.0;   /* M0 is the equilibrium magnetization */
  psample->R1=0.01; /* R1 = 1/T1 is the longitudinal relaxation rate */
  psample->R2=0.2; /* R2 = 1/T2 is the transversal relaxation rate  */
    
  y = N_VNew(NEQ, NULL);       /* Allocate y, abstol vectors */
  abstol = N_VNew(NEQ, NULL); 


  reltol = RTOL;               /* Set the scalar relative tolerance */
  Ith(abstol,1) = ATOL1;       /* Set the vector absolute tolerance */
  Ith(abstol,2) = ATOL2;
  Ith(abstol,3) = ATOL3;


 printf(" \nBloch equation with 90 deg. hard pulse, followed by gradient \n\n");

 /*outer loop of repetitions, just for performance testing*/
 for (rep=0;rep<1;rep++){ 
 
  /* Initialize y */
  Ith(y,1) = 0.0;  Ith(y,2) = 0.0;  Ith(y,3) = psample->M0;

  /* Call CVodeMalloc to initialize CVODE: 
     NEQ     is the problem size = number of equations
     f       is the user's right hand side function in y'=f(t,y)
     T0      is the initial time
     y       is the initial dependent variable vector
     BDF     specifies the Backward Differentiation Formula
     NEWTON  specifies a Newton iteration
     SV      specifies scalar relative and vector absolute tolerances
     &reltol is a pointer to the scalar relative tolerance
     abstol  is the absolute tolerance vector
     FALSE   indicates there are no optional inputs in iopt and ropt
     iopt    is an array used to communicate optional integer input and output
     ropt    is an array used to communicate optional real input and output
     A pointer to CVODE problem memory is returned and stored in cvode_mem. */

  cvode_mem = CVodeMalloc(NEQ, bloch, T0, y, ADAMS, FUNCTIONAL, SV, &reltol,
   			abstol,  psample, NULL, FALSE, iopt, ropt, NULL);
  if (cvode_mem == NULL) { printf("CVodeMalloc failed.\n"); return(1); }

 /* In loop over output points, call CVode, print results, test for error */
 
  for (iout=1, tout=T1; iout <= NOUT; iout++, tout += TADD) {
    flag = CVode(cvode_mem, tout, y, &t, NORMAL);
    if (flag != SUCCESS) { printf("CVode failed, flag=%d.\n", flag); break; }
    printf("At t = %0.4e      y =%14.6e  %10.2f  %14.6e\n", t, Ith(y,1), Ith(y,2)*180.0/PI, Ith(y,3));
  }

  CVodeFree(cvode_mem);        /* Free the CVODE problem memory */
 
 }/*end of repetition loop*/
 
  N_VFree(y);                  /* Free the y and abstol vectors */
  N_VFree(abstol);   

  PrintFinalStats(iopt);       /* Print some final statistics   */
  return(0);
}


/************************ Private Helper Function ************************/

/* Print some final statistics located in the iopt array */
static void PrintFinalStats(long int iopt[])
{
  printf("\nFinal Statistics.. \n\n");
  printf("nst = %-6ld nfe  = %-6ld nsetups = %-6ld \n",
	 iopt[NST], iopt[NFE], iopt[NSETUPS]);
  printf("nni = %-6ld ncfn = %-6ld netf = %ld\n \n",
	 iopt[NNI], iopt[NCFN], iopt[NETF]);
}


/***************** Functions Called by the CVODE Solver ******************
 *									 *
 *             computes bloch eqation in cylindrical coordinates         *
 *									 *
 *************************************************************************/
static void bloch(integer N, real t, N_Vector y, N_Vector ydot, void *f_data)
{
  real Mxy,phi,Mz; /*cylndrical components of mangetization*/
  real s,c,Mx,My,Mx_dot,My_dot,Mz_dot;
  mr_sample *psample;
  psample = (mr_sample*) f_data;
  
  real EPS=1e-10;

  real Bx=0,By=0,Bz=0;
  /*instead of the follwoing lines, Bx,By,Bz should come from the sequence*/
  real fa=PI/2.0;
  Bx=(t<1.0?fa/1.0:0.0); /*magnetic field for hard pulse in 1.0*/
  Bz=(t<1.0?0.0:5.0);    /*field gradient turned on*/
    
  Mxy = Ith(y,1); phi = Ith(y,2); Mz = Ith(y,3);
  
  /*compute cartesian components of transversal magnetization*/
   c = cos(phi); s = sin(phi);
  Mx = c*Mxy;   My = s*Mxy;

  /*compute bloch equations*/
  Mx_dot = -psample->R2*Mx      +Bz*My           -By*Mz;
  My_dot =      -Bz*Mx    -psample->R2*My        +Bx*Mz;
  Mz_dot =       By*Mx      - Bx*My       +psample->R1*(psample->M0 - Mz);
 
  /*compte derivatives in cylindrical coordinates*/
  Ith(ydot,1) = c*Mx_dot + s*My_dot;
  Ith(ydot,2) = (c*My_dot - s*Mx_dot) / (Mxy>EPS?Mxy:EPS); /*avoid division by zero*/
  Ith(ydot,3) = Mz_dot;
}

/* f routine. Compute f(t,y). */

static void f(integer N, real t, N_Vector y, N_Vector ydot, void *f_data)
{
  real y1, y2, y3, yd1, yd3;

  y1 = Ith(y,1); y2 = Ith(y,2); y3 = Ith(y,3);

  yd1 = Ith(ydot,1) = -0.04*y1 + 1e4*y2*y3;
  yd3 = Ith(ydot,3) = 3e7*y2*y2;
        Ith(ydot,2) = -yd1 - yd3;
}
