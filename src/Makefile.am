

bin_PROGRAMS = jemris sanityck
TESTS      = testsequences.sh testsignals.sh testcoils.sh
if MPI
  bin_PROGRAMS += pjemris
  TESTS += testmpi.sh
endif

SOURCES = AnalyticGradPulse.cpp AnalyticGradPulse.h AnalyticRFPulse.cpp AnalyticRFPulse.h AtomicSequence.cpp AtomicSequence.h Attribute.cpp Attribute.h BiotSavartLoop.cpp BiotSavartLoop.h Bloch_CV_Model.cpp Bloch_CV_Model.h Coil.cpp Coil.h CoilArray.cpp CoilArray.h CoilPrototypeFactory.cpp CoilPrototypeFactory.h ConcatSequence.cpp ConcatSequence.h ConstantGradPulse.cpp ConstantGradPulse.h DOMTreeErrorReporter.cpp DOMTreeErrorReporter.h Debug.h Declarations.h DelayAtomicSequence.cpp DelayAtomicSequence.h DynamicVariables.cpp DynamicVariables.h EmptyPulse.cpp EmptyPulse.h ExternalCoil.cpp ExternalCoil.h ExternalGradPulse.cpp ExternalGradPulse.h ExternalRFPulse.cpp ExternalRFPulse.h GaussianRFPulse.cpp GaussianRFPulse.h GradPulse.cpp GradPulse.h HardRFPulse.cpp HardRFPulse.h IdealCoil.cpp IdealCoil.h Model.cpp Model.h Module.cpp Module.h ModulePrototypeFactory.cpp ModulePrototypeFactory.h Parameters.cpp Parameters.h Prototype.cpp Prototype.h PrototypeFactory.cpp PrototypeFactory.h Pulse.cpp Pulse.h RFPulse.cpp RFPulse.h RepIter.cpp RepIter.h Sample.cpp Sample.h SechRFPulse.cpp SechRFPulse.h Sequence.cpp Sequence.h SequenceTree.cpp SequenceTree.h Signal.cpp Signal.h Simulator.cpp Simulator.h SincRFPulse.cpp SincRFPulse.h SpiralGradPulse.cpp SpiralGradPulse.h StrX.cpp StrX.h TPOI.cpp TPOI.h Trajectory.cpp Trajectory.h Trajectory1D.cpp Trajectory1D.h TrajectoryEmpty.h TrajectoryT2s.cpp TrajectoryT2s.h TrajectoryInterface.cpp TrajectoryInterface.h TrajectoryMotion.cpp TrajectoryMotion.h TrapGradPulse.cpp TrapGradPulse.h TriangleGradPulse.cpp TriangleGradPulse.h TxRxPhase.cpp TxRxPhase.h World.cpp World.h XMLRead.cpp XMLRead.h ginac_functions.h rng.h rng.cpp var_array.h var_array++.h var_array.c list.h list_c.h

jemris_SOURCES   = $(SOURCES) jemris.cpp
sanityck_SOURCES = $(SOURCES) sanityck.cpp
pjemris_SOURCES  = $(SOURCES) mpi_Model.h pjemris.cpp

EXTRA_DIST = testsequences.sh testsignals.sh testcoils.sh testmpi.sh
