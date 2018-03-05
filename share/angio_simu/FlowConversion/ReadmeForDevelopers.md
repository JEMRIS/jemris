## Particle tracing with ParaView and JEMRIS export
## Detailed explanation of the scripts, for developers

There are two tools for the particle tracing:
### `ParticleTracer.py`
This first script is not to be used by itself, but directly in ParaView.
To do so you must first edit the script to put the path to your data in which you want to use the particle tracer.
When editing the file, modify the path to the data. By default, the path goes to a `vtk` directory.   
Then you can launch ParaView.   
In ParaView, go into Tools -> Python Shell ...   
Then Run Script in the new window that appears and select the `ParticleTracer.py` script.   
Normally, everything should be set up automatically in the interface of ParaView, if not re-check the path to your data.   
Eventually, you just have to click on the Play button of ParaView and the particles will be simultaneously traced and saved (The save directory is /tmp/PT).   

#### How does the pipeline work ?
In the interface of ParaView, you have the simulation data, a PointSource component (that allows to generate particles), a ParticleTracer filter (that allows to do the particle tracing) and a Programmable filter (that allows to write the traced particles in the JEMRIS format). As for now, the particles are written in separate files (one file per particle).

#### How to parametrize the pipeline ?
* In the PointSource source, you can act on several parameters.
The particles are randomly distributed in a sphere. You can set the number of generated particles (Number of Points), the center of the sphere (Point) and the radius of the sphere (Radius).
* In the ParticleTracer filter, there is an option called "Force Reinjection every Nsteps". This option allows to reinject particles every n time steps. The default value is set to 100 by default for the sample dataset with ~5000 time steps.

### MergeDatJEMRIS.py
This second script allows to merge all the .dat files, each one corresponding to the path of a particle, in a single file in the JEMRIS format.
To use this script, you have to launch it the following way:
./MergeDatJEMRIS.py /tmp/PT

/tmp/PT is the default directory for particle storage.
The resulting file will be stored in /tmp/PT/result.

