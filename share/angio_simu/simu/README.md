### Angiography simulations with Jemris ###

#The flow file can be ASCII or HDF5 with the following format (quadruple the negative indicators for HDF5):

	t0 spin0_x spin0_y spin0_z
	-111				<- SPIN ACTIVATION
	t1 spin0_x spin0_y spin0_z
	t2 spin0_x spin0_y spin0_z
	…
	tN spin0_x spin0_y spin0_z
	-222				<- SPIN DEACTIVATION
	-999999				<- SPIN CHANGE INDICATOR

	t0 spin1_x spin1_y spin1_z
	-111
	t1 spin1_x spin1_y spin1_z
	…

 The structure for HDF5 is:
	Group '/flow' 
        Dataset 'trajectories' 
            Size:  4xNtraj
            MaxSize:  4xNtraj
            Datatype:   H5T_IEEE_F64LE (double)
            ChunkSize:  []
            Filters:  none
            FillValue:  0.000000


#In the *.xml simu file, provide the path of the flow file containing the Lagrangian spins trajectories, e.g.:

<?xml version="1.0" encoding="utf-8"?>
<simulate name="JEMRIS">
   <sample name="vessels" FlowTrajectories="Flow.dat" uri="./sample.h5"/>
   <RXcoilarray uri="./uniform.xml"/>
   <TXcoilarray uri="./uniform.xml"/>
   <parameter ConcomitantFields="0" EvolutionPrefix="evol" EvolutionSteps="0" RandomNoise="0"/>
   <sequence name="angio" uri="./angio.xml"/>
   <model name="Bloch" type="CVODE"/>
</simulate>


#Start the simulation with "jemris <SimuFile>.xml" (use pjemris for parallel)




### Loop trajectories option ###

The same flow trajectories can be reused periodically to reduce the amount of data, e.g. for steady flow or periodic flow (cardiac cycle).
In the *.xml simu files, set e.g.:

<sample name="cardiac" FlowTrajectories="Flow.dat" FlowLoopDuration="850" 
FlowLoopNumber="1500" uri="./sample.h5"/>

'FlowLoopDuration' is the duration of the flow cycle (ms). If the sample size exceeds the total number of trajectories, the N last trajectories corresponding to 'FlowLoopNumber' wil be reused periodically with a time shift equal to 'FlowLoopDuration' (note: the spins stays at their initial position until the beginning of their trajectory).
Thus, to cover the whole sequence, the input sample size must be:
 	NonLoopedSpins + FlowLoopNumber*SequenceDuration/FlowLoopDuration

E.g., we can imagine a vessel initially containing 10000 spins, which entry is seeded with 1500 spins during one cardiac cycle of 850 ms. Then, the input flow file will contain 11500 trajectories, and a sequence of 400000 ms will be simulated with a sample of 10000 + 1500*400000/850 = 715882 spins.



### Image reconstruction from HDF5 output signal ###

-For basic sequences, images can be visualized with the JEMRIS_sim Matlab interface.
-For PC sequences and 3D sequences, use the specific Recon_*.m Matlab scripts for image reconstruction.
