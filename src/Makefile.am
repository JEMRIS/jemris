
lib_LTLIBRARIES = libjembase.la libjemseq.la libjemsim.la

bin_PROGRAMS = jemris sanityck
TESTS      = testsequences.sh testsignals.sh testcoils.sh
if MPI
  bin_PROGRAMS += pjemris
  TESTS += testmpi.sh
endif

libjembase_la_SOURCES = Attribute.cpp Attribute.h DOMTreeErrorReporter.cpp DOMTreeErrorReporter.h Debug.h Declarations.h Prototype.cpp Prototype.h PrototypeFactory.cpp PrototypeFactory.h StrX.cpp StrX.h TPOI.cpp TPOI.h TxRxPhase.cpp TxRxPhase.h XMLRead.cpp XMLRead.h XMLWriter.cpp XMLWriter.h ginac_functions.h rng.h rng.cpp var_array.h var_array++.h var_array.c list.h list_c.h
libjemseq_la_SOURCES = AnalyticGradPulse.cpp AnalyticGradPulse.h AnalyticRFPulse.cpp AnalyticRFPulse.h AtomicSequence.cpp AtomicSequence.h ConcatSequence.cpp ConcatSequence.h ConstantGradPulse.cpp ConstantGradPulse.h DelayAtomicSequence.cpp DelayAtomicSequence.h EmptyPulse.cpp EmptyPulse.h ExternalGradPulse.cpp ExternalGradPulse.h ExternalRFPulse.cpp ExternalRFPulse.h GaussianRFPulse.cpp GaussianRFPulse.h GradPulse.cpp GradPulse.h HardRFPulse.cpp HardRFPulse.h Module.cpp Module.h ModulePrototypeFactory.cpp ModulePrototypeFactory.h Parameters.cpp Parameters.h Pulse.cpp Pulse.h RFPulse.cpp RFPulse.h RepIter.cpp RepIter.h SechRFPulse.cpp SechRFPulse.h Sequence.cpp Sequence.h SequenceTree.cpp SequenceTree.h SincRFPulse.cpp SincRFPulse.h SpiralGradPulse.cpp SpiralGradPulse.h TrapGradPulse.cpp TrapGradPulse.h TriangleGradPulse.cpp TriangleGradPulse.h World.cpp World.h
libjemsim_la_SOURCES = AnalyticCoil.h AnalyticCoil.cpp BiotSavartLoop.cpp BiotSavartLoop.h Bloch_CV_Model.cpp Bloch_CV_Model.h Coil.cpp Coil.h CoilArray.cpp CoilArray.h CoilPrototypeFactory.cpp CoilPrototypeFactory.h DynamicVariables.cpp DynamicVariables.h ExternalCoil.cpp ExternalCoil.h IdealCoil.cpp IdealCoil.h Model.cpp Model.h Sample.cpp Sample.h  SampleReorderShuffle.cpp SampleReorderShuffle.h SampleReorderStrategyInterface.h Signal.cpp Signal.h Simulator.cpp Simulator.h Trajectory.cpp Trajectory.h Trajectory1D.cpp Trajectory1D.h TrajectoryEmpty.h TrajectoryT2s.cpp TrajectoryT2s.h TrajectoryInterface.cpp TrajectoryInterface.h TrajectoryMotion.cpp TrajectoryMotion.h 

libjembase_la_LDFLAGS = $(all_libraries) --version-info 1.0.0 

libjemseq_la_LIBADD = libjembase.la
libjemseq_la_LDFLAGS = $(all_libraries) --version-info 1.0.0 

libjemsim_la_LIBADD = libjembase.la
libjemsim_la_LDFLAGS = $(all_libraries) --version-info 1.0.0 

jemris_LDADD     = libjemseq.la libjemsim.la
jemris_SOURCES   = jemris.cpp

sanityck_LDADD   = libjemseq.la libjemsim.la
sanityck_SOURCES = sanityck.cpp

pjemris_LDADD     = libjemseq.la libjemsim.la
pjemris_SOURCES   = Mpi2Evolution.cpp Mpi2Evolution.h mpi_Model.h pjemris.cpp

EXTRA_DIST = testsequences.sh testsignals.sh testcoils.sh testmpi.sh
