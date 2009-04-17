#!/bin/sh
# Queue to submit to
#PBS -q dque
# Job Limits
#PBS -l cput=300000000
# Keep stout,-error in
#PBS -k eo
# Merge std error to stdout
#PBS -j oe
# Set the name of output file
#PBS -o test
#PBS -l nodes=8:ppn=4
# send me email when job end
##PBS -M t.stoecker@fz-juelich.de
##PBS -m e
# send me email when job aborts (with an error)
##PBS -m a
# do not rerun this job if it fails
#PBS -r n
# export all my environment variables to the job
#PBS -V
#

cd $PBS_O_WORKDIR/

#machinefile for MPI

machinefile=`basename $PBS_NODEFILE.tmp`
uniq $PBS_NODEFILE > $machinefile

#one extra process for the (mostly sleeping) master

processes=`wc $PBS_NODEFILE|awk '{print 1+$1}'`

#start MPI program 
mpirun.mpich -machinefile $machinefile -np $processes /apps/prod/misc/bin/pjemris simu.xml > .sim.out 

