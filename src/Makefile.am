bin_PROGRAMS = jemris sanityck
TESTS      = testsequences.sh testsignals.sh testcoils.sh
if MPI
  bin_PROGRAMS += pjemris
  TESTS += testmpi.sh
endif

AM_CXXFLAGS = $(OPENMP_CXXFLAGS)

SOURCES = AnalyticCoil.cpp AnalyticCoil.h AnalyticGradPulse.cpp AnalyticGradPulse.h AnalyticPulseShape.cpp AnalyticPulseShape.h AnalyticRFPulse.cpp AnalyticRFPulse.h AtomicSequence.cpp AtomicSequence.h Attribute.cpp Attribute.h BinaryIO.h BinaryIO.cpp BiotSavartLoop.cpp BiotSavartLoop.h Bloch_McConnell_CV_Model.cpp Bloch_McConnell_CV_Model.h Bloch_CV_Model.cpp Bloch_CV_Model.h Coil.cpp Coil.h CoilArray.cpp CoilArray.h CoilPrototypeFactory.cpp CoilPrototypeFactory.h ConcatSequence.cpp ConcatSequence.h ConstantGradPulse.cpp ConstantGradPulse.h DOMTreeErrorReporter.cpp DOMTreeErrorReporter.h Debug.h Declarations.h DelayAtomicSequence.cpp DelayAtomicSequence.h DynamicVariables.cpp DynamicVariables.h EmptyPulse.cpp EmptyPulse.h ExternalCoil.cpp ExternalCoil.h ExternalGradPulse.cpp ExternalGradPulse.h ExternalPulseData.cpp ExternalPulseData.h ExternalRFPulse.cpp ExternalRFPulse.h GaussianRFPulse.cpp GaussianRFPulse.h GradPulse.cpp GradPulse.h HardRFPulse.cpp HardRFPulse.h HDF5IO.cpp HDF5IO.h IdealCoil.cpp IdealCoil.h Microstructure.cpp Microstructure.h MicrostructureBoxes.cpp MicrostructureBoxes.h MicrostructureCrossingFiber.cpp MicrostructureCrossingFiber.h MicrostructureKissingFiber.cpp MicrostructureKissingFiber.h MicrostructureSingleFiber.cpp MicrostructureSingleFiber.h Model.cpp Model.h Module.cpp Module.h ModulePrototypeFactory.cpp ModulePrototypeFactory.h Parameters.cpp Parameters.h Prototype.cpp Prototype.h PrototypeFactory.cpp PrototypeFactory.h Pulse.cpp Pulse.h RFPulse.cpp RFPulse.h RepIter.cpp RepIter.h MultiPoolSample.cpp MultiPoolSample.h Sample.cpp Sample.h SampleReorderShuffle.cpp SampleReorderShuffle.h SampleReorderStrategyInterface.h SechRFPulse.cpp SechRFPulse.h Sequence.cpp Sequence.h SequenceTree.cpp SequenceTree.h Signal.cpp Signal.h SimpleIO.h SimpleIO.cpp Simulator.cpp Simulator.h SincRFPulse.cpp SincRFPulse.h SpiralGradPulse.cpp SpiralGradPulse.h StrX.cpp StrX.h TPOI.cpp TPOI.h Trajectory.cpp Trajectory.h Trajectory1D.cpp Trajectory1D.h TrajectoryDiffusion.cpp TrajectoryDiffusion.h TrajectoryEmpty.h TrajectoryInterface.cpp TrajectoryInterface.h TrajectoryMotion.cpp TrajectoryMotion.h TrajectoryT2s.cpp TrajectoryT2s.h TrapGradPulse.cpp TrapGradPulse.h TriangleGradPulse.cpp TriangleGradPulse.h TxRxPhase.cpp TxRxPhase.h World.cpp World.h XMLIO.cpp XMLIO.h config.h ginac_functions.h list.h list_c.h rng.cpp rng.h var_array++.h var_array++_cpp.h var_array.c var_array.h

#SampleReorderShuffle.cpp SampleReorderShuffle.h

jemris_SOURCES   = $(SOURCES) jemris.cpp
jemris_CXXFLAGS = $(AM_CXXFLAGS) 
sanityck_SOURCES = $(SOURCES) sanityck.cpp
pjemris_SOURCES   = $(SOURCES) Mpi2Evolution.cpp Mpi2Evolution.h mpi_Model.h pjemris.cpp

EXTRA_DIST = $(TESTS)

do_subst = sed $(do_dlopen) \
	-e 's,[@]MPIRUN[@],$(MPIRUN),g' \
	-e 's,[@]MPIRUN_PROC_OPT[@],$(MPIRUN_PROC_OPT),g'

testmpi.sh: testmpi.sh.in Makefile
	$(do_subst) < testmpi.sh.in > testmpi.sh.tmp
	chmod +x testmpi.sh.tmp
	mv testmpi.sh.tmp testmpi.sh

