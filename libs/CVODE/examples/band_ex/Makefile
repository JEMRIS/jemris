#----------------------------------------------------------------------------
#          Makefile for the Band Example Program cvbx.c
#----------------------------------------------------------------------------


COMPILER = gcc

INC = ../../include

LIB = ../../lib

OPTS = -I$(INC)

HDRS = $(INC)/llnltyps.h $(INC)/cvode.h $(INC)/cvband.h $(INC)/nvector.h \
       $(INC)/band.h


cvbx: cvbx.o
	$(COMPILER) -L$(LIB) -o cvbx cvbx.o -lcvode -lm

cvbx.o: cvbx.c $(HDRS)
	$(COMPILER) $(OPTS) -c cvbx.c
