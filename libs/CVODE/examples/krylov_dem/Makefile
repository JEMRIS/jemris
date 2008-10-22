#----------------------------------------------------------------------------
#          Makefile for the Krylov Demonstration Program cvdemk.c
#----------------------------------------------------------------------------


COMPILER = gcc

INC = ../../include

LIB = ../../lib

OPTS = -I$(INC)

HDRS = $(INC)/llnltyps.h $(INC)/cvode.h $(INC)/iterativ.h $(INC)/cvspgmr.h \
       $(INC)/dense.h $(INC)/nvector.h $(INC)/llnlmath.h


cvdemk: cvdemk.o
	$(COMPILER) -L$(LIB) -o cvdemk cvdemk.o -lcvode -lm

cvdemk.o: cvdemk.c $(HDRS)
	$(COMPILER) $(OPTS) -c cvdemk.c
