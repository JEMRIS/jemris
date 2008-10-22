#----------------------------------------------------------------------------
#          Makefile for the Example Program bloch_ex.c
#----------------------------------------------------------------------------

COMPILER = g++
INC = ../../include
LIB = ../../lib
OPTS = -I$(INC)
HDRS = $(INC)/llnltyps.h $(INC)/cvode.h $(INC)/nvector.h 

bloch_ex: bloch_ex.o
	$(COMPILER) -L$(LIB) -o bloch_ex bloch_ex.o -lcvode -lm

bloch_ex.o: bloch_ex.cpp $(HDRS)
	$(COMPILER) $(OPTS) -c bloch_ex.cpp

