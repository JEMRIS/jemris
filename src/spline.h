#ifndef SPLINE_H
#define SPLINE_H 1
/* spline.c
 Cubic interpolating spline. */

/************************************************/
/*                                              */
/*  CMATH.  Copyright (c) 1989 Design Software  */
/*                                              */
/************************************************/

/*-----------------------------------------------------------------*/
template <class T>
inline T linspace(T *Array, double d1, double d2, int n){
    int i,j;
    double Increment;

    j = 0;
    Increment = (d2-d1)/((double)(n-1));
    
    for (i = 0; i < n-1; i++){
        Array[i] = d1+ j*Increment;
        j++;
    }
    Array[n-1] = d2;
    
    return 0.0;
}

inline int spline (int n, int end1, int end2,
            double slope1, double slope2,
            double x[], double y[],
            double b[], double c[], double d[],
            int *iflag)


/* Purpose ...
 -------
 Evaluate the coefficients b[i], c[i], d[i], i = 0, 1, .. n-1 for
 a cubic interpolating spline
 
 S(xx) = Y[i] + b[i] * w + c[i] * w**2 + d[i] * w**3
 where w = xx - x[i]
 and   x[i] <= xx <= x[i+1]
 
 The n supplied data points are x[i], y[i], i = 0 ... n-1.
 
 Input :
 -------
 n       : The number of data points or knots (n >= 2)
 end1,
 end2    : = 1 to specify the slopes at the end points
 = 0 to obtain the default conditions
 slope1,
 slope2  : the slopes at the end points x[0] and x[n-1]
 respectively
 x[]     : the abscissas of the knots in strictly
 increasing order
 y[]     : the ordinates of the knots
 
 Output :
 --------
 b, c, d : arrays of spline coefficients as defined above
 (See note 2 for a definition.)
 iflag   : status flag
 = 0 normal return
 = 1 less than two data points; cannot interpolate
 = 2 x[] are not in ascending order
 
 This C code written by ...  Peter & Nigel,
 ----------------------      Design Software,
 42 Gubberley St,
 Kenmore, 4069,
 Australia.
 
 Version ... 1.1, 30 September 1987
 -------     2.0, 6 April 1989    (start with zero subscript)
 remove ndim from parameter list
 2.1, 28 April 1989   (check on x[])
 2.2, 10 Oct   1989   change number order of matrix
 
 Notes ...
 -----
 (1) The accompanying function seval() may be used to evaluate the
 spline while deriv will provide the first derivative.
 (2) Using p to denote differentiation
 y[i] = S(X[i])
 b[i] = Sp(X[i])
 c[i] = Spp(X[i])/2
 d[i] = Sppp(X[i])/6  ( Derivative from the right )
 (3) Since the zero elements of the arrays ARE NOW used here,
 all arrays to be passed from the main program should be
 dimensioned at least [n].  These routines will use elements
 [0 .. n-1].
 (4) Adapted from the text
 Forsythe, G.E., Malcolm, M.A. and Moler, C.B. (1977)
 "Computer Methods for Mathematical Computations"
 Prentice Hall
 (5) Note that although there are only n-1 polynomial segments,
 n elements are requird in b, c, d.  The elements b[n-1],
 c[n-1] and d[n-1] are set to continue the last segment
 past x[n-1].
 */

/*----------------------------------------------------------------*/

{  /* begin procedure spline() */
	
	int    nm1, ib, i;
	double t;
	
	nm1    = n - 1;
	*iflag = 0;
	
	if (n < 2) {  
        /* no possible interpolation */
		*iflag = 1;
		return 0;
	}
	
	for (i = 1; i < n; ++i)  {
        if (x[i] <= x[i-1]) {
            *iflag = 2;    
		    return 0;
        }
    }
	
	if (n >= 3) {    
        /* ---- At least quadratic ---- */
		
		/* ---- Set up the symmetric tri-diagonal system
		 b = diagonal
		 d = offdiagonal
		 c = right-hand-side  */
		d[0] = x[1] - x[0];
		c[1] = (y[1] - y[0]) / d[0];
		for (i = 1; i < nm1; ++i) {
			d[i]   = x[i+1] - x[i];
			b[i]   = 2.0 * (d[i-1] + d[i]);
			c[i+1] = (y[i+1] - y[i]) / d[i];
			c[i]   = c[i+1] - c[i];
		}
		
		/* ---- Default End conditions
		 Third derivatives at x[0] and x[n-1] obtained
		 from divided differences  */
		b[0]   = -d[0];
		b[nm1] = -d[n-2];
		c[0]   = 0.0;
		c[nm1] = 0.0;
		if (n != 3) {
			c[0]   = c[2] / (x[3] - x[1]) - c[1] / (x[2] - x[0]);
			c[nm1] = c[n-2] / (x[nm1] - x[n-3]) - c[n-3] / (x[n-2] - x[n-4]);
			c[0]   = c[0] * d[0] * d[0] / (x[3] - x[0]);
			c[nm1] = -c[nm1] * d[n-2] * d[n-2] / (x[nm1] - x[n-4]);
		}
		
		/* Alternative end conditions -- known slopes */
		if (end1 == 1) {
			b[0] = 2.0 * (x[1] - x[0]);
			c[0] = (y[1] - y[0]) / (x[1] - x[0]) - slope1;
		}
		if (end2 == 1) {
			b[nm1] = 2.0 * (x[nm1] - x[n-2]);
			c[nm1] = slope2 - (y[nm1] - y[n-2]) / (x[nm1] - x[n-2]);
		}
		
		/* Forward elimination */
		for (i = 1; i < n; ++i) {
			t    = d[i-1] / b[i-1];
			b[i] = b[i] - t * d[i-1];
			c[i] = c[i] - t * c[i-1];
		}
		
		/* Back substitution */
		c[nm1] = c[nm1] / b[nm1];
		for (ib = 0; ib < nm1; ++ib) {
			i    = n - ib - 2;
			c[i] = (c[i] - d[i] * c[i+1]) / b[i];
		}
		
		/* c[i] is now the sigma[i] of the text */
		
		/* Compute the polynomial coefficients */
		b[nm1] = (y[nm1] - y[n-2]) / d[n-2] + d[n-2] * (c[n-2] + 2.0 * c[nm1]);
		for (i = 0; i < nm1; ++i) {
			b[i] = (y[i+1] - y[i]) / d[i] - d[i] * (c[i+1] + 2.0 * c[i]);
			d[i] = (c[i+1] - c[i]) / d[i];
			c[i] = 3.0 * c[i];
		}
		c[nm1] = 3.0 * c[nm1];
		d[nm1] = d[n-2];
		
	} else { /* if n >= 3 */
	   /* linear segment only  */
		b[0] = (y[1] - y[0]) / (x[1] - x[0]);
		c[0] = 0.0;
		d[0] = 0.0;
		b[1] = b[0];
		c[1] = 0.0;
		d[1] = 0.0;
	}
	
	return 0;
}  /* end of spline() */
/*-------------------------------------------------------------------*/


inline double seval (int n, double u,
              double x[], double y[],
              double b[], double c[], double d[],
              int *last)

/*Purpose ...
 -------
 Evaluate the cubic spline function
 
 S(xx) = y[i] + b[i] * w + c[i] * w**2 + d[i] * w**3
 where w = u - x[i]
 and   x[i] <= u <= x[i+1]
 Note that Horner's rule is used.
 If u < x[0]   then i = 0 is used.
 If u > x[n-1] then i = n-1 is used.
 
 Input :
 -------
 n       : The number of data points or knots (n >= 2)
 u       : the abscissa at which the spline is to be evaluated
 Last    : the segment that was last used to evaluate U
 x[]     : the abscissas of the knots in strictly increasing order
 y[]     : the ordinates of the knots
 b, c, d : arrays of spline coefficients computed by spline().
 
 Output :
 --------
 seval   : the value of the spline function at u
 Last    : the segment in which u lies
 
 Notes ...
 -----
 (1) If u is not in the same interval as the previous call then a
 binary search is performed to determine the proper interval.
 
 */
/*-------------------------------------------------------------------*/

{  /* begin function seval() */
	
	int    i, j, k;
	double w;
	
	i = *last;
	if (i >= n-1) 
        i = 0;
    else if (i < 0)  
        i = 0;

	if ((x[i] > u) || (x[i+1] < u)) {  
        /* ---- perform a binary search ---- */
		i = 0;
		j = n;
		do {
			k = (i + j) / 2;         /* split the domain to search */
			if (u < x[k])            /* move the upper bound */ 
                j = k;    
            else                     /* move the lower bound */
                i = k;    
		}  while (j > i+1);          /* there are no more segments to search */
	}
	*last = i;
	
	/* ---- Evaluate the spline ---- */
	w = u - x[i];
	w = y[i] + w * (b[i] + w * (c[i] + w * d[i]));
	return (w);
}
/*-------------------------------------------------------------------*/


inline double deriv (int n, double u,
              double x[],
              double b[], double c[], double d[],
              int *last)

/* Purpose ...
 -------
 Evaluate the derivative of the cubic spline function
 
 S(x) = B[i] + 2.0 * C[i] * w + 3.0 * D[i] * w**2
 where w = u - X[i]
 and   X[i] <= u <= X[i+1]
 Note that Horner's rule is used.
 If U < X[0] then i = 0 is used.
 If U > X[n-1] then i = n-1 is used.
 
 Input :
 -------
 n       : The number of data points or knots (n >= 2)
 u       : the abscissa at which the derivative is to be evaluated
 last    : the segment that was last used
 x       : the abscissas of the knots in strictly increasing order
 b, c, d : arrays of spline coefficients computed by spline()
 
 Output :
 --------
 deriv : the value of the derivative of the spline
 function at u
 last  : the segment in which u lies
 
 Notes ...
 -----
 (1) If u is not in the same interval as the previous call then a
 binary search is performed to determine the proper interval.
 
 */
/*-------------------------------------------------------------------*/

{  /* begin function deriv() */
	
	int    i, j, k;
	double w;
	
	i = *last;
	if (i >= n-1) 
        i = 0;
	else if (i < 0) 
        i = 0;
			
	if ((x[i] > u) || (x[i+1] < u)) {  
        /* ---- perform a binary search ---- */
		i = 0;
		j = n;
		do {
			k = (i + j) / 2;          /* split the domain to search */
			if (u < x[k])             /* move the upper bound */
                j = k;
            else                      /* move the lower bound */
                i = k;
		} while (j > i+1);            /* there are no more segments to search */
	}
	*last = i;
	
	/* ---- Evaluate the derivative ---- */
	w = u - x[i];
	w = b[i] + w * (2.0 * c[i] + w * 3.0 * d[i]);
	return (w);
	
} /* end of deriv() */

/*-------------------------------------------------------------------*/


inline double sinteg (int n, double u,
               double x[], double y[],
               double b[], double c[], double d[],
               int *last)

/*Purpose ...
 -------
 Integrate the cubic spline function
 
 S(xx) = y[i] + b[i] * w + c[i] * w**2 + d[i] * w**3
 where w = u - x[i]
 and   x[i] <= u <= x[i+1]
 
 The integral is zero at u = x[0].
 
 If u < x[0]   then i = 0 segment is extrapolated.
 If u > x[n-1] then i = n-1 segment is extrapolated.
 
 Input :
 -------
 n       : The number of data points or knots (n >= 2)
 u       : the abscissa at which the spline is to be evaluated
 Last    : the segment that was last used to evaluate U
 x[]     : the abscissas of the knots in strictly increasing order
 y[]     : the ordinates of the knots
 b, c, d : arrays of spline coefficients computed by spline().
 
 Output :
 --------
 sinteg  : the value of the spline function at u
 Last    : the segment in which u lies
 
 Notes ...
 -----
 (1) If u is not in the same interval as the previous call then a
 binary search is performed to determine the proper interval.
 
 */
/*-------------------------------------------------------------------*/

{  /* begin function sinteg() */
	
	int    i, j, k;
	double sum, dx;
	
	i = *last;
	if (i >= n-1) 
        i = 0;
	else if (i < 0)  
        i = 0;
			
	if ((x[i] > u) || (x[i+1] < u)) {
		/* ---- perform a binary search ---- */
	    i = 0;
	    j = n;
	    do {
		    k = (i + j) / 2;         /* split the domain to search */
            if (u < x[k])             /* move the upper bound */
                j = k;
            else                      /* move the lower bound */
                i = k;
	    } while (j > i+1);           /* there are no more segments to search */
    }
	*last = i;
	
	sum = 0.0;
	/* ---- Evaluate the integral for segments x < u ---- */
	for (j = 0; j < i; ++j) {
		dx = x[j+1] - x[j];
		sum += dx *
		(y[j] + dx *
		 (0.5 * b[j] + dx *
          (c[j] / 3.0 + dx * 0.25 * d[j])));
	}
	
	/* ---- Evaluate the integral fot this segment ---- */
	dx = u - x[i];
	sum += dx *
	(y[i] + dx *
	 (0.5 * b[i] + dx *
	  (c[i] / 3.0 + dx * 0.25 * d[i])));
	
	return (sum);
}
/*-------------------------------------------------------------------*/


inline double deriv2 (int n, double u,
               double x[],
               double b[], double c[], double d[],
               int *last)


/* Purpose ...
 -------
 Evaluate the 2nd derivative of the cubic spline function
 
 S(x) =  2.0 * C[i]+ 6.0 * D[i] * w
 where w = u - X[i]
 and   X[i] <= u <= X[i+1]
 Note that Horner's rule is used.
 If U < X[0] then i = 0 is used.
 If U > X[n-1] then i = n-1 is used.
 
 Input :
 -------
 n       : The number of data points or knots (n >= 2)
 u       : the abscissa at which the derivative is to be evaluated
 last    : the segment that was last used
 x       : the abscissas of the knots in strictly increasing order
 b, c, d : arrays of spline coefficients computed by spline()
 
 Output :
 --------
 deriv : the value of the derivative of the spline
 function at u
 last  : the segment in which u lies
 
 Notes ...
 -----
 (1) If u is not in the same interval as the previous call then a
 binary search is performed to determine the proper interval.
 
 */
/*-------------------------------------------------------------------*/

{  /* begin function deriv2() */
	//avoid compiler warning:
    (void) b;
    
	int    i, j, k;
	double w;
	
	i = *last;
	if (i >= n-1) 
        i = 0;
	else if (i < 0) 
        i = 0;
			
	if ((x[i] > u) || (x[i+1] < u)) {
		/* ---- perform a binary search ---- */
		i = 0;
		j = n;
		do {
			k = (i + j) / 2;          /* split the domain to search */
            if (u < x[k])             /* move the upper bound */
                j = k;
            else                      /* move the lower bound */
                i = k;
		} while (j > i+1);            /* there are no more segments to search */
	}
	*last = i;
	
	/* ---- Evaluate the derivative ---- */
	w = u - x[i];
	w = 2.0 * c[i] + w * 6.0 * d[i];
	return (w);
	
} /* end of deriv2() */

/*-------------------------------------------------------------------*/

#endif
