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
#PBS -l nodes=8:ppn=4:quad
##PBS -l nodes=8:ppn=8:oct

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
mpdboot --totalnum=`cat $PBS_NODEFILE | uniq | wc -l` -f $PBS_NODEFILE
mpiexec -machinefile $PBS_NODEFILE -np `cat $PBS_NODEFILE | wc -l|awk '{print $1}'` /vol/bin/pjemris simu.xml > .sim.out
mpdallexit

