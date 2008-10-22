#----------------------------------------------------------------------------
#          Makefile for the Direct Demonstration Program cvdemd.c
#----------------------------------------------------------------------------


COMPILER = gcc

INC = ../../include

LIB = ../../lib

OPTS = -I$(INC)

HDRS = $(INC)/llnltyps.h $(INC)/cvode.h $(INC)/cvdense.h $(INC)/cvband.h \
       $(INC)/cvdiag.h $(INC)/nvector.h $(INC)/llnlmath.h


cvdemd: cvdemd.o
	$(COMPILER) -L$(LIB) -o cvdemd cvdemd.o -lcvode -lm

cvdemd.o: cvdemd.c $(HDRS)
	$(COMPILER) $(OPTS) -c cvdemd.c
