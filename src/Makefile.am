bin_PROGRAMS = jemris sanityck
if MPI
  CXXLD = @MPICXX@
  CXX   = @MPICXX@
  bin_PROGRAMS += pjemris
endif

SOURCES = AnalyticGradPulse.cpp AnalyticGradPulse.h AnalyticRFPulse.cpp AnalyticRFPulse.h AtomicSequence.cpp AtomicSequence.h BiotSavartLoop.cpp BiotSavartLoop.h Bloch_CV_Model.cpp Bloch_CV_Model.h Coil.cpp Coil.h CoilArray.cpp CoilArray.h CoilPrototypeFactory.cpp CoilPrototypeFactory.h ConcatSequence.cpp ConcatSequence.h ConstantGradPulse.cpp ConstantGradPulse.h DOMTreeErrorReporter.cpp DOMTreeErrorReporter.h Debug.h Declarations.h DelayAtomicSequence.cpp DelayAtomicSequence.h EmptyPulse.cpp EmptyPulse.h ExternalCoil.cpp ExternalCoil.h ExternalGradPulse.cpp ExternalGradPulse.h ExternalRFPulse.cpp ExternalRFPulse.h GaussianRFPulse.cpp GaussianRFPulse.h GradPulse.cpp GradPulse.h HardRFPulse.cpp HardRFPulse.h IdealCoil.cpp IdealCoil.h LTARFPulse.cpp LTARFPulse.h LTASpiralGradPulse.cpp LTASpiralGradPulse.h LissajousGradPulse.cpp LissajousGradPulse.h Model.cpp Model.h Module.cpp Module.h ModulePrototypeFactory.cpp ModulePrototypeFactory.h Parameters.cpp Parameters.h Prototype.cpp Prototype.h PrototypeFactory.cpp PrototypeFactory.h Pulse.cpp Pulse.h RFPulse.cpp RFPulse.h RepIter.cpp RepIter.h STARFPulse.cpp STARFPulse.h STASpiralGradPulse.cpp STASpiralGradPulse.h Sample.cpp Sample.h SechRFPulse.cpp SechRFPulse.h Sequence.cpp Sequence.h SequenceTree.cpp SequenceTree.h Signal.cpp Signal.h Simulator.cpp Simulator.h SincRFPulse.cpp SincRFPulse.h SpiralGradPulse.cpp SpiralGradPulse.h StrX.cpp StrX.h TPOI.cpp TPOI.h TrapGradPulse.cpp TrapGradPulse.h TxRxPhase.cpp TxRxPhase.h World.cpp World.h XMLRead.cpp XMLRead.h ginac_functions.h rng.h rng.cpp 
jemris_SOURCES = $(SOURCES) jemris.cpp
sanityck_SOURCES = $(SOURCES) sanityck.cpp
pjemris_SOURCES = $(SOURCES) mpi_Model.h pjemris.cpp
