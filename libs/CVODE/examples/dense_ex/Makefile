#----------------------------------------------------------------------------
#          Makefile for the Dense Example Program cvdx.c
#----------------------------------------------------------------------------


COMPILER = g++

INC = ../../include

LIB = ../../lib

OPTS = -I$(INC)

HDRS = $(INC)/llnltyps.h $(INC)/cvode.h $(INC)/cvdense.h $(INC)/nvector.h \
       $(INC)/dense.h

cvdx: cvdx.o
	$(COMPILER) -L$(LIB) -o cvdx cvdx.o -lcvode -lm

cvdx.o: cvdx.c $(HDRS)
	$(COMPILER) $(OPTS) -c cvdx.c
