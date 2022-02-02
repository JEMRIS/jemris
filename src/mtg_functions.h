#ifndef MTG_FUNCTIONS
#define MTG_FUNCTIONS 1

/*This file contains all the functions used to calculated the time optimal gradient waveforms.

minTimeGradientRIV   -   Computes the rotationally invariant solution
     RungeKutte_riv  -   Used to solve the ODE using RK4
     beta            -   calculates sqrt (gamma^2 * smax^2 - k^2 * st^4) in the ODE. Used in RungeKutte_riv
minTimeGradientRV    -   Computes the rotationally variant solution
     RungeKutte_rv   -   Used to solve the ODE using RK4
     sdotdot         -   calculates the maximum possible value for d^2s/dt^t, used in RugeKutte_rv */

#include "spline.h"
#include "float.h"
#include "math.h"

inline double beta(double k, double st, double smax, double gamma = 4.25756) {
    /* calculates sqrt (gamma^2 * smax^2 - k^2 * st^4) used in RK4 method for rotationally invariant ODE solver */
    return  sqrt(sqrt((gamma*gamma*smax*smax - k*k*st*st*st*st)*(gamma*gamma*smax*smax - k*k*st*st*st*st)));
}

inline double RungeKutte_riv(double ds, double st, double k[], double smax, double gamma = 4.25756) {
    /*  Solves ODE for rotationally invariant solution using Runge-Kutte method*/
    double k1 = ds * (1/st) * beta(k[0], st, smax, gamma);
    double k2 = ds * 1 / (st + k1/2) * beta(k[1], st + k1/2, smax, gamma);
    double k3 = ds * 1 / (st + k2/2) * beta(k[1], st + k2/2, smax, gamma);
    double k4 = ds * 1 / (st + k3/2) * beta(k[2], st + k3/2, smax, gamma);
    return (k1/6 + k2/3 + k3/3 + k4/6);
}
    
inline void minTimeGradientRIV(double* x, double* y, double* z, int Lp, double g0, double gfin, double gmax, double smax, 
                        double dt, double*& gx, double*& gy, double*& gz,
                        int &l_t, double ds = -1, double gamma = 4.25756) {

    /*Finds the time optimal gradient waveforms for the rotationally invariant constraints case.
    
    x            -       The input curve in x
    y            -       , y
    z            -       , and z dir.
    Lp           -       row dimension of x,y,z
    g0           -       Initial gradient amplitude.
    gfin         -       Gradient value at the end of the trajectory.
                         If given value is not possible
                         the result would be the largest possible amplitude.
    gmax         -       Maximum gradient [G/cm] (4 default)
    smax         -       Maximum slew [G/cm/ms] (15 default)
    dt           -       Sampling time intervale [ms] (4e-3 default)
    gx, gy, gz   -       pointers to gradient waveforms to be returned
    kx, ky, kz   -       pointers to k-space trajectory after interpolation
    l_t          -       Dimension of interpolated k-space trajectory (kx, ky, kz) needed for creating mex return arrays.
    */
    
    int i = 0;
    
    /* iflag used in spline method to signal error */
    int iflag;
    int last;
    
    double *p = new double[Lp];
    
    /* Representing the curve with parameter p */
    for (i = 0; i < Lp; ++i)
        p[i] = i;
    
    /* Interpolation of curve for gradient accuracy, using cubic spline interpolation */
    
    /* arrays used by spline function to store coefficients. */
    double *c1x = new double[Lp];
    double *c2x = new double[Lp];
    double *c3x = new double[Lp];
    double *c1y = new double[Lp];
    double *c2y = new double[Lp];
    double *c3y = new double[Lp];
    double *c1z = new double[Lp];
    double *c2z = new double[Lp];
    double *c3z = new double[Lp];
    
    spline(Lp, 0, 0, 1, 1, p, x, c1x, c2x, c3x, &iflag);
    spline(Lp, 0, 0, 1, 1, p, y, c1y, c2y, c3y, &iflag);
    spline(Lp, 0, 0, 1, 1, p, z, c1z, c2z, c3z, &iflag);
    
    double dp = 0.1;
    int num_evals = (int) floor((Lp-1) / dp)+ 1;
    
    double toeval = 0;
    
    /* converting to arc-length parameterization from p, using trapezoidal integration */
    double *s_of_p = new double[num_evals];
    s_of_p[0] = 0;
    
    double *sop_num = new double[num_evals];

    double sofar = 0;
    double Cp_abs_pre = 0.;
    for (i = 0; i < num_evals; ++i) {
        toeval = (double) i * dp;
        double Cpx = deriv(Lp, toeval, p, c1x, c2x, c3x, &last);
        double Cpy = deriv(Lp, toeval, p, c1y, c2y, c3y, &last);
        double Cpz = deriv(Lp, toeval, p, c1z, c2z, c3z, &last);
        /* interpolated curve in p-parameterization */
        double Cp_abs = sqrt(Cpx*Cpx + Cpy*Cpy + Cpz*Cpz);
        sofar += (Cp_abs + Cp_abs_pre)/2;
        s_of_p[i] =  dp * sofar;
        Cp_abs_pre = Cp_abs;

        sop_num[i] = i * dp;
    }
    
    /* length of the curve */
    double L = s_of_p[num_evals-1];
    
    /* decide ds and compute st for the first point */
    double stt0 = gamma*smax;   /* always assumes first point is max slew */
    double st0  = (stt0*dt)/2;  /* start at half the gradient for accuracy close to g=0 */
    double s0   = st0*dt;

    if (ds < 0) {               /* if a ds value was not specified */
//         ds = s0/1.5;            /* smaller step size for numerical accuracy */
        ds = fabs(ds) * s0/1.5;    /* pehses: to make it easier to scale ds */
    }
    
    int length_of_s =  (int) floor(L/ds);
    int half_ls     = (int) floor(L/(ds/2));
    
    double *s   = new double[length_of_s];
    double *sta = new double[length_of_s];
    double *stb = new double[length_of_s];
    
    for (i = 0; i<length_of_s; i++) {
        s[i]   = i*ds;
        sta[i] = 0;
        stb[i] = 0;
    }
    
    /* Convert from s(p) to p(s) and interpolate for accuracy */
    double *a1x = new double[num_evals];
    double *a2x = new double[num_evals];
    double *a3x = new double[num_evals];
    
    spline(num_evals, 0, 0, 1, 1, s_of_p, sop_num, a1x, a2x, a3x, &iflag);

    double *s_half  = new double[half_ls];
    double *p_of_s_half  = new double[half_ls];
    
    for (i=0; i < half_ls; ++i) {
        s_half[i]      = (double)i*(ds/2);
        p_of_s_half[i] = seval(num_evals, s_half[i], s_of_p, sop_num, a1x, a2x, a3x, &last);
    }    
    
    delete[] a1x; delete[] a2x; delete[] a3x;
    delete[] s_of_p; delete[] sop_num;
    
    /* Csp is C(s(p)) = [Cx(p(s)) Cy(p(s)) Cz(p(s))] */
    double *Cspx = new double[length_of_s];
    double *Cspy = new double[length_of_s];
    double *Cspz = new double[length_of_s];
    
    double *p_of_s = new double[length_of_s];
    
    for (i=0; i<length_of_s; ++i) {
        p_of_s[i] = p_of_s_half[2*i];
        Cspx[i]   = seval(Lp, p_of_s[i], p, x, c1x, c2x, c3x, &last);
        Cspy[i]   = seval(Lp, p_of_s[i], p, y, c1y, c2y, c3y, &last);
        Cspz[i]   = seval(Lp, p_of_s[i], p, z, c1z, c2z, c3z, &last);
    }
    delete[] p_of_s_half;
    
    /* arrays used by spline function to store coefficients. */
    double *Csp1x = new double[length_of_s];
    double *Csp2x = new double[length_of_s];
    double *Csp3x = new double[length_of_s];
    double *Csp1y = new double[length_of_s];
    double *Csp2y = new double[length_of_s];
    double *Csp3y = new double[length_of_s];
    double *Csp1z = new double[length_of_s];
    double *Csp2z = new double[length_of_s];
    double *Csp3z = new double[length_of_s];
    spline(length_of_s, 0, 0, 1, 1, s, Cspx, Csp1x, Csp2x, Csp3x, &iflag);
    spline(length_of_s, 0, 0, 1, 1, s, Cspy, Csp1y, Csp2y, Csp3y, &iflag);
    spline(length_of_s, 0, 0, 1, 1, s, Cspz, Csp1z, Csp2z, Csp3z, &iflag);
    
    int size_k = half_ls+2;    /* extend of k for RK4 */
    double *k = new double[size_k];        /* k is the curvature along the curve */
    for (i=0; i < half_ls; ++i) {
        double kx = deriv2(length_of_s, s_half[i], s, Csp1x, Csp2x, Csp3x, &last);
        double ky = deriv2(length_of_s, s_half[i], s, Csp1y, Csp2y, Csp3y, &last);
        double kz = deriv2(length_of_s, s_half[i], s, Csp1z, Csp2z, Csp3z, &last);
        k[i] =  sqrt(kx*kx + ky*ky + kz*kz);        /* the curvature, magnitude of the second derivative of the curve in arc-length parameterization */
    }
    k[size_k-2] = k[size_k-3];
    k[size_k-1] = k[size_k-3];
    
    delete[] s_half;
    delete[]  Cspx;  delete[]  Cspy; delete[]  Cspz;
    delete[] Csp1x;  delete[] Csp1y; delete[] Csp1z;
    delete[] Csp2x;  delete[] Csp2y; delete[] Csp2z;
    delete[] Csp3x;  delete[] Csp3y; delete[] Csp3z;
    
    /* computing geomtry dependent constraints (forbidden line curve) */
    double *sdot = new double[half_ls];
    
    /* Calculating the upper bound for the time parametrization */
    /* sdot (which is a non scaled max gradient constaint) as a function of s. */
    /* sdot is the minimum of gamma*gmax and sqrt(gamma*gmax / k) */
    double gammagmax = gamma*gmax;
    for (i=0; i< half_ls; ++i) {
        double sdot2 = sqrt((gamma*smax) / (fabs(k[i]+(DBL_EPSILON))));
        if (gammagmax < sdot2)
            sdot[i] = gammagmax;
        else
            sdot[i] = sdot2;
    }
    
    double g0gamma = g0*gamma + st0;
    if (g0gamma < gammagmax)
        sta[0] = g0gamma;
    else
        sta[0] = gammagmax;
    
    /* Solving ODE Forward */
    
    for (i=1; i<length_of_s; ++i) {
        double k_rk[3];
        k_rk[0] = k[2*i-2];
        k_rk[1] = k[2*i-1];
        k_rk[2] = k[2*i];
        
        double dstds = RungeKutte_riv(ds, sta[i-1], k_rk, smax, gamma);
        double tmpst = sta[i-1] + dstds;
        
        if (sdot[2*i+1] < tmpst)
            sta[i] = sdot[2*i+1];
        else
            sta[i] = tmpst;
    }
    
    /*Solving ODE Backwards: */
    
    double max;
    if(gfin < 0 ) {
        /*if gfin is not provided */
        stb[length_of_s-1] = sta[length_of_s - 1];
    } else {
        
        if (gfin * gamma > st0)
            max = gfin*gamma;
        else
            max = st0;
        
        if (gammagmax < max)
            stb[length_of_s-1] = gammagmax;
        else
            stb[length_of_s-1] = max;
    }
    
    for (i=length_of_s-2; i>-1; --i) {
        double k_rk[3];
        k_rk[0] = k[2*i+2];
        k_rk[1] = k[2*i+1];
        k_rk[2] = k[2*i];
        
        double dstds = RungeKutte_riv(ds, stb[i+1], k_rk, smax, gamma);
        double tmpst = stb[i+1] + dstds;
        
        if (sdot[2*i] < tmpst)
            stb[i] = sdot[2*i];
        else
            stb[i] = tmpst;
    }
    delete[] k;
    delete[] sdot;
    
    /*Final interpolation */    
    
    /* Converting to the time parameterization, t(s) using trapezoidal integration. t(s) = integral (1/st) ds */
    double st_of_s;
    if (sta[0] < stb[0])
        st_of_s = sta[0];
    else
        st_of_s = stb[0];
    double st_ds_i_pre = ds*(1/st_of_s);
    
    double *t_of_s = new double[length_of_s];
    t_of_s[0] = 0;
    for (i=1; i < length_of_s; ++i) {
        if (sta[i] < stb[i])
            st_of_s = sta[i];
        else
            st_of_s = stb[i];
        
        double st_ds_i = ds*(1/st_of_s);         /* ds * 1/st(s) used in below calculation of t(s) */
        
        t_of_s[i] =  t_of_s[i-1] + (st_ds_i+ st_ds_i_pre)/2;
        
        st_ds_i_pre = st_ds_i;
    }
    delete[] sta; delete[] stb;
    
    l_t = (int) floor(t_of_s[length_of_s-1]/dt);
    
    
    /* coefficient arrays for spline interpolation of t(s) to get s(t) */
    double *t1x = new double[length_of_s];
    double *t2x = new double[length_of_s];
    double *t3x = new double[length_of_s];
    
    spline(length_of_s, 0, 0, 1, 1, t_of_s, s, t1x, t2x, t3x, &iflag);

    
    /* coefficient arrays for spline interpolation of p(s) with s(t) to get p(s(t)) = p(t) */
    double *p1x = new double[length_of_s];
    double *p2x = new double[length_of_s];
    double *p3x = new double[length_of_s];
    
    spline(length_of_s, 0, 0, 1, 1, s, p_of_s, p1x, p2x, p3x, &iflag);
    
    double *p_of_t = new double[l_t];
    
    for (i=0; i < l_t; ++i) {
        double s_of_t = seval(length_of_s,   i*dt, t_of_s, s, t1x, t2x, t3x, &last);
        p_of_t[i]     = seval(length_of_s, s_of_t, s, p_of_s, p1x, p2x, p3x, &last);
    }
    
    delete[] t1x; delete[] t2x; delete[] t3x;
    delete[] t_of_s;
    delete[] s; delete[] p_of_s;
    delete[] p1x; delete[] p2x; delete[] p3x;
    
    /*  interpolated k-space trajectory */
    double *Cx = new double[l_t];
    double *Cy = new double[l_t];
    double *Cz = new double[l_t];
    
    for (i=0; i<l_t; i++) {
        Cx[i] = seval(Lp, p_of_t[i], p, x, c1x, c2x, c3x, &last);
        Cy[i] = seval(Lp, p_of_t[i], p, y, c1y, c2y, c3y, &last);
        Cz[i] = seval(Lp, p_of_t[i], p, z, c1z, c2z, c3z, &last);
    }
    delete[] p_of_t; delete[] p;
    delete[] c1x; delete[] c1y; delete[] c1z;
    delete[] c2x; delete[] c2y; delete[] c2z;
    delete[] c3x; delete[] c3y; delete[] c3z;
    
    /* Final gradient waveforms to be returned */
    gx = new double[l_t];
    gy = new double[l_t];
    gz = new double[l_t];
    
    for (i=0; i< l_t - 1; ++i) {
        gx[i] = (Cx[i+1] - Cx[i]) / (gamma * dt);
        gy[i] = (Cy[i+1] - Cy[i]) / (gamma * dt);
        gz[i] = (Cz[i+1] - Cz[i]) / (gamma * dt);
    }
    delete[] Cx; delete[] Cy; delete[] Cz;
    
    gx[l_t-1] = 2*gx[l_t-2] - gx[l_t-3];
    gy[l_t-1] = 2*gy[l_t-2] - gy[l_t-3];
    gz[l_t-1] = 2*gz[l_t-2] - gz[l_t-3];
  
}

inline void calcTrajectory(double *gx, double *gy, double *gz, int l_t, double dt, double*& kx, double*& ky, double*& kz, double gamma = 4.25756) {
    /* k-space trajectory to be returned (calculated by integrating gradient waveforms by trapezoidal integration) */
    kx = new double[l_t];
    ky = new double[l_t];
    kz = new double[l_t];

    double sofarx = 0;
    double sofary = 0;
    double sofarz = 0;

    kx[0] = 0.;
    ky[0] = 0.;
    kz[0] = 0.;

    for (int i=1; i < l_t; ++i) {
        sofarx += (gx[i] + gx[i-1]) / 2;
        sofary += (gy[i] + gy[i-1]) / 2;
        sofarz += (gz[i] + gz[i-1]) / 2;
        
        kx[i] = sofarx * dt * gamma;
        ky[i] = sofary * dt * gamma;
        kz[i] = sofarz * dt * gamma;
    }
}

#endif
