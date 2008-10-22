/******************************************************************
 *                                                                *
 * File          : llnlmath.c                                     *
 * Programmers   : Scott D. Cohen and Alan C. Hindmarsh @ LLNL    *
 * Version of    : 1 September 1994                               *
 *----------------------------------------------------------------*
 * This is the implementation file for a C math library.          *
 *                                                                *
 ******************************************************************/


#include <stdio.h>
#include <math.h>
#include "llnlmath.h"
#include "llnltyps.h"


#define ZERO RCONST(0.0)
#define ONE  RCONST(1.0)
#define TWO  RCONST(2.0)


cvreal UnitRoundoff(void)
{
  cvreal u;
  volatile cvreal one_plus_u;
  
  u = ONE;
  one_plus_u = ONE + u;
  while (one_plus_u != ONE) {
    u /=  TWO;
    one_plus_u = ONE + u;
  }
  u *=  TWO;
  
  return(u);
}


cvreal RPowerI(cvreal base, int exponent)
{
  int i, expt;
  cvreal prod;

  prod = ONE;
  expt = ABS(exponent);
  for(i=1; i <= expt; i++) prod *= base;
  if (exponent < 0) prod = ONE/prod;
  return(prod);
}


cvreal RPowerR(cvreal base, cvreal exponent)
{
 
  if (base <= ZERO) return(ZERO);

  return((cvreal)pow((double)base,(double)exponent));
}


cvreal RSqrt(cvreal x)
{
  if (x <= ZERO) return(ZERO);

  return((cvreal) sqrt((double) x));
}
