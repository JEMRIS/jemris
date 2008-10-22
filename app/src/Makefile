CC    = g++
ARCH := $(shell uname -m | sed -e s/i.86/i386/)
SYS  := $(shell uname -s)
HOME := $(shell echo $HOME)
CP    = /bin/cp

CLUSTER_INSTALL_DIR = /srv/apps/prod/misc/bin

# Architecture check
ifeq ($(ARCH), i386)
    MPICC=mpicxx
endif
ifeq ($(ARCH), x86_64)
    MPICC=mpicxx
endif
ifeq ($(ARCH), ppc64)
    MPICC=mpixlcxx_r 
    CC   =bgxlc++_r
endif
ifeq ($(SYS), MINGW32_NT-5.1)
    MPICC=g++ 
    W32_FLAGS=-DXML_LIBRARY ../../../lib/libmpi.a 
endif  
# Libraries source code directories
BUILD  = ./build/
DEBUG  = ./debug/
SRC    = ./framework/
MATLB  = ../matlab/

# 3rd party libraries 
CVODE  = ../../libs/CVODE
RNG    = ../../libs/RNG
BOOST  = ../../libs/BOOST
LOCAL  = ../../..
CVLIB  = $(CVODE)/lib
CVINC  = $(CVODE)/include
LOCALL = $(LOCAL)/lib
LOCALI = $(LOCAL)/include


# Compiler option
CCOPTS = -O3 -w $(W32_FLAGS) -D$(ARCH)$(SYS) -I$(LOCALI) -I$(RNG) -I$(CVINC) -I$(SRC) -I$(BOOST) 

#Linker options
LDOPTS = $(W32_FLAGS) -L$(CVLIB) -L$(LOCALL) $(RNG)/rng.o -lxerces-c -lm -lcvode -lginac -lcln 

OBS := $(patsubst $(SRC)%.cpp,$(BUILD)%.o,$(wildcard $(SRC)*.cpp))
INC := $(wildcard $(SRC)*.h)

#TO DO: all objects + main program
all   : jemris pjemris sanityck

# commands for model framework
$(BUILD)%.o       : $(SRC)%.cpp
	 $(CC) $(CCOPTS) -c $< -o $@

#commands for main programs
sanityck : $(OBS) $(INC) sanityck.cpp
	$(CC) sanityck.cpp $(OBS) $(CCOPTS) $(LDOPTS) -o sanityck
jemris   : $(OBS) $(INC) jemris.cpp
	$(CC) jemris.cpp $(OBS) $(CCOPTS) $(LDOPTS) -o jemris
	test -e jemris && cp jemris $(MATLB)
pjemris  : $(OBS) $(INC) pjemris.cpp
	$(MPICC) pjemris.cpp $(OBS) $(CCOPTS) $(LDOPTS) -o pjemris 
	test -e pjemris && cp pjemris $(MATLB)

#check
seqdiag_check :
	./sanityck xmpl 1
simulation_check :
	./sanityck xmpl 2
coil_check :
	./sanityck xmpl 3

mpi_check :
	@echo
	@echo "Test Case 4: testing parallel execution"
	@echo "======================================="
	mpirun -np 3 ./pjemris ../xmpl/approved/mpi_simu.xml > mpirun.out
	@cat mpirun.out
	@echo
	@if test `grep Finished mpirun.out` = Finished ;  \
	then echo Success;   \
	else echo Error; \
	fi
	@echo

check :  seqdiag_check simulation_check coil_check mpi_check 
#clean up
remove-objects :
	rm -f $(BUILD)*.o *.o 
remove-binaries :
	rm -f jemris pjemris sanityck
clean : remove-objects remove-binaries

