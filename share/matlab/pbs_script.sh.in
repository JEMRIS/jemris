#!/bin/bash

# Queue to submit to
#PBS -q cpu

# Job Limits
#PBS -N pjemris
#PBS -l cput=300000000

# Keep stout,-error in
#PBS -k eo

# Merge std error to stdout
#PBS -j oe

# Set the name of output file
#PBS -o test
##PBS -l nodes=8:ppn=4:quad
#PBS -l nodes=4:ppn=8:oct

# Send me email when job end
##PBS -M k.vahedipour@fz-juelich.de
##PBS -m abe

# Do not rerun this job if it fails
#PBS -r n

# Export all my environment variables to the job
#PBS -V

# Change to working directory
cd $PBS_O_WORKDIR/
# Start mpd ring run mpi program and exit mpd ring
@MPIRUN@ -machinefile $PBS_NODEFILE @MPIRUN_PROC_OPT@ `cat $PBS_NODEFILE | wc -l|awk '{print $1}'` @prefix@/bin/pjemris simu.xml > .sim.out

