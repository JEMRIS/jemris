## Particle tracing with ParaView and JEMRIS export
## This is an Eulerian-to-Lagrangian script module converter to calculate flow particles trajectories for Jemris
## This is done with particle seeding and particle tracking in an Eulerian mesh (format .vtk and .vtu)


How-to-use


***** Put the .vtk and .vtu flow mesh (with eulerian velocities) in the "vtk" folder.

***** Set correctly the parameters of ConfigFile.in and ConfigMRI.in

ConfigFile.in:
	*FinalT: Particle tracking duration. Set it to the max duration of the sequences to simulate (WARNING: generated files can be huge for long sequences).
	*DeltaT: Indicative time step to store the particles positions (precise value will be calculated by the software). Set accordingly to the desired trajectories precision level.
	*CycleT: If unsteady flow, the same trajectories will be reused periodically over a cycle of duration CycleT.
	*SampleCenter: Center of an imaginary sphere containing the whole sample.
	*SampleRadius: Radius of the sphere.
	*LabelsEntries: vtk labels of the entries of the mesh (no exit!).
	*Vmax: Max velocities at the entries (if high variability from one entry to an other, treat other entries separetely with SampleRadius=0).
	*NodesInterval: Spatial interval between neighboring nodes (will determine the interspins distance, must be inferior to the voxel size!).
	*ForceReinjection(steady=0,unsteady=1): For steady flow, entries will be seeded once, and the same trajectories will be reused in jemris to cover the whole sequence duration. For periodic unsteady flow, entries will be seeded over CycleT and reused periodically in jemris.

ConfigMRI.in: MRI acquisition sequence parameters


***** Launch ./go.sh

***** Create an HDF5 sample as specified

***** Launch jemris with the Flow.dat and simu_flow.xml files generated