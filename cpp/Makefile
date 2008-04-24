ARCH  := $(shell uname -m | sed -e s/i.86/i386/)
CLUSTER_INSTALL_DIR = /srv/apps/prod/misc/bin

CC     = g++
CCOPTS = -O3 -DARCH$(ARCH)
CP     = /bin/cp

#architecture check
ifeq ($(ARCH), i386)
	MPICC=mpicxx
endif
ifeq ($(ARCH), x86_64)
	MPICC = mpicxx.mpich-gcc64
        CC    = g++-4.1
endif
ifeq ($(ARCH), ppc64)
	MPICC=scalasca -instrument mpixlcxx_r 
	CC   =scalasca -instrument mpixlcxx_r
endif

#libraries source code directories
SFW = ./SEQ_FRAMEWORK/
MOD = ./MODEL/
CVI = ../CVODE/include/
CVL = ../CVODE/lib/
CV  = ../CVODE

# On blue Gene xerces needs to be built statically using the following 
# -plinux -cbgxlc_r -xbgxlcxx_r -minmem -nsocket -tnative -rpthread -s static

#location of xerces-c
XRCL = ../xerces-c/lib/
XRCI = ../xerces-c/include/

#objects to compile
SFWOBJS=$(SFW)Sequence.o  $(SFW)AtomicSequence.o  $(SFW)ConcatSequence.o $(SFW)TGPS.o $(SFW)XmlSequence.o
MODOBJS=$(MOD)MR_Model.o $(MOD)BCV_MR_Model.o $(MOD)bloch.o  $(MOD)Sample.o $(MOD)Signal.o $(MOD)XmlSimulation.o

#dependence on header files (add new sequences to this list)
SEQH =  $(SFW)Parameter.h $(SFW)AllPulses.h $(SFW)EmptyPulse.h $(SFW)GradientPulseShape.h \
	$(SFW)RfPulseShape.h $(SFW)HardRfPulseShape.h $(SFW)SincRfPulseShape.h $(SFW)Scanner.h \
	$(SFW)ConcatPulseShape.h $(SFW)RfSpoiling.h $(SFW)RfPhaseCycling.h \
	$(SFW)PulseShape.h $(SFW)RO_TGPS.h  $(SFW)PE_TGPS.h $(SFW)SS_TGPS.h  $(SFW)DelayAtom.h\
	$(SFW)ExternalPulseShape.h $(SFW)GradientSpiral.h $(MOD)Spin.h \
	$(SFW)GradientSpiralPart.h $(SFW)GradientSpiralConcatSequence.h \
	$(SFW)GradientSpiralExtRfConcatSequence.h

#TO DO: all objects + main program
all   : $(SFWOBJS) $(MODOBJS) jemris pjemris

#commands for sequence framework
$(SFW)Sequence.o : $(SFW)Sequence.cpp
	$(CC) $(CCOPTS)  -c $(SFW)Sequence.cpp -I $(SFW) -I $(MOD) -o $(SFW)Sequence.o
$(SFW)AtomicSequence.o : $(SFW)AtomicSequence.cpp
	$(CC) $(CCOPTS) -c  $(SFW)AtomicSequence.cpp -I $(SFW) -I $(MOD) -o $(SFW)AtomicSequence.o
$(SFW)ConcatSequence.o :  $(SFW)ConcatSequence.cpp
	$(CC) $(CCOPTS) -c  $(SFW)ConcatSequence.cpp -I $(SFW) -I $(MOD) -o $(SFW)ConcatSequence.o
$(SFW)TGPS.o : $(SFW)TGPS.cpp
	$(CC)  $(CCOPTS) -c  $(SFW)TGPS.cpp -I $(SFW) -I $(MOD) -o $(SFW)TGPS.o
$(SFW)XmlSequence.o : $(SEQH) $(SFW)XmlSequence.cpp
	$(CC)  $(CCOPTS) -c  $(SFW)XmlSequence.cpp -I $(XRCI) -I $(SFW) -I $(MOD) -o $(SFW)XmlSequence.o

#commands for the model
$(MOD)bloch.o : $(MOD)bloch.cpp
	$(CC) $(CCOPTS) -c $(MOD)bloch.cpp -I $(MOD) -I $(SFW) -I$(CVI) -o $(MOD)bloch.o
$(MOD)MR_Model.o : $(MOD)MR_Model.cpp
	$(CC)  $(CCOPTS) -c  $(MOD)MR_Model.cpp -I $(MOD) -I $(SFW) -o $(MOD)MR_Model.o
$(MOD)BCV_MR_Model.o : $(MOD)BCV_MR_Model.cpp
	$(CC)  $(CCOPTS) -c  $(MOD)BCV_MR_Model.cpp -I $(MOD) -I $(SFW) -I$(CVI) -o $(MOD)BCV_MR_Model.o
$(MOD)Sample.o : $(MOD)Sample.cpp
	$(CC) $(CCOPTS) -c $(MOD)Sample.cpp -I $(MOD) -o $(MOD)Sample.o
$(MOD)Signal.o : $(MOD)Signal.cpp
	$(CC)  $(CCOPTS) -c $(MOD)Signal.cpp -I $(MOD) -o $(MOD)Signal.o
$(MOD)XmlSimulation.o : $(MOD)XmlSimulation.cpp
	$(CC)  $(CCOPTS) -c  $(MOD)XmlSimulation.cpp -I $(XRCI) -I $(MOD) -I $(SFW) -I$(CVI) -o $(MOD)XmlSimulation.o

#commands for main programs
jemris : $(SFWOBJS) $(SEQH) $(MODOBJS) jemris.cpp
	$(CC) $(CCOPTS) -I$(CVI) -L$(CVL) -L$(XRCL) -I$(XRCI) -I$(SFW) -I$(MOD) -o jemris $(SFWOBJS) $(MODOBJS) jemris.cpp -lm -lcvode -lxerces-c
pjemris : $(SFWOBJS) $(MODOBJS) $(SEQH) pjemris.cpp
	$(MPICC) $(CCOPTS) -I$(CVI) -L$(CVL) -L$(XRCL) -I$(XRCI) -I$(SFW) -I$(MOD) -o pjemris $(SFWOBJS) $(MODOBJS) pjemris.cpp -lm -lcvode -lxerces-c

#clean up
remove-objects :
	rm -f SEQ_FRAMEWORK/*.o MODEL/*.o *.o
remove-binaries :
	rm -f jemris pjemris
clean : remove-objects remove-binaries

#CVODE
cvode :
	cd $(CV); make

#install
cluster-install :	cvode all
	runner=`whoami`; if test $$runner != "root"; then echo "You are not root. Have password ready"; su -c "cp *jemris ${CLUSTER_INSTALL_DIR}"; cp pjemris jemris "${CLUSTER_INSTALL_DIR}"; fi

