#!/usr/bin/env python

# MergeDatJEMRIS.py
# Description: allows to merge particles from separate files to a single file
# Author: Alexandre Ancel <alexandre.ancel@cemosis.fr>
# Date: 07/08/15

import os
import sys

# Function that list the files of a directory and
# store the .dat
def listDatFiles(baseDir, prefix, indices):
    if(not os.path.isdir(baseDir)):
        print baseDir + " is not a directory"
        return 1

    (_, _, filenames) = os.walk(baseDir).next()

    # iterate over the files in the directory
    for f in filenames:
        name, ext = os.path.splitext(f)
        if(ext == ".dat"):
            # extract the particle id
            lst = name.rsplit(".", 1)
            if( len(lst) == 2 ):
                prefix.append(lst[0])
                indices.append(int(lst[1]))

def main():
    # Check argument: ensure correct usage
    if(len(sys.argv) != 2):
        print "usage: " + sys.argv[0] + " <directory>"
        print """Description: this script will list all the .dat files (particle trajectory) in the directory
in argument. It will then merge them in a single file by using JEMRIS file format stored in a result directory
in the same directory as the .dat files"""
        print "File Format: "
        print "<time1>\\t<particle1_x>\\t<particle1_y>\\t<particle1_z>\\n"
        print "<time2>\\t<particle1_x>\\t<particle1_y>\\t<particle1_z>\\n"
        print "..."
        print "-999999\\n"
        print "<time1>\\t<particle2_x>\\t<particle2_y>\\t<particle2_z>\\n"
        print "<time2>\\t<particle2_x>\\t<particle2_y>\\t<particle2_z>\\n"
        print "..."
        print "-999999\\n"
        print "..."
        exit(1)

    # get all the particle indices available
    prefix = []
    indices = []
    listDatFiles(sys.argv[1], prefix, indices)
    prefix.sort()
    print "prefix ", list(set(prefix))
    #print "indices: ", sorted(list(set(indices)))
    print len(list(set(indices))), "indices"
    prefix =  list(set(prefix))
    indices = sorted(list(set(indices)))


    # create a subdir to store the result
    # in case we rerun the script, it will ensure that
    # we don't use the particles.dat file as an input for the script
    dr = os.path.join(sys.argv[1], "result")
    if(not os.path.exists(dr)):
        os.mkdir(dr)

    # open the file gathering all the particles
    fpnameF = os.path.join(sys.argv[1], dr, "particles_fill.dat")
    fpF = open(fpnameF, "w")
    fpnameS = os.path.join(sys.argv[1], dr, "particles_seed.dat")
    fpS = open(fpnameS, "w")
    # Build the file gathering all the particles into one file
    # iterate over the available indices
    N = 0
    Nseed = 0
    for src in prefix:
        print "Merge ",src+".*.dat"
        for i in indices:
            fname = os.path.join(sys.argv[1], src + "." + str(i) + ".dat")
            if os.path.isfile(fname):
                fl = open(fname, "r")
                if ("Source" in src):
				    # write the data for the current particle
				    for ln in fl:
				        fpS.write(ln)
				    # write particle separator
				    fpS.write("-999999\n\n")
				    # count particles
				    N = N + 1
                                    Nseed = Nseed + 1
                else:
					# write the data for the current particle
					for ln in fl:
						fpF.write(ln)
					# write particle separator
					fpF.write("-999999\n\n")
					# count particles
					N = N + 1
    fpF.close()
    fpS.close()

    #Print particles infos
    print "Seeding particles: ", Nseed
    print "Total particles: ", N

    #Save particles info
    finfoname = os.path.join(sys.argv[1], dr, "particles_info.dat")
    finfo = open(finfoname, "w")
    finfo.write("SeedingParticles:" + " " + str(Nseed) + "\n" + "TotalParticles:" + " " + str(N) + "\n")
    finfo.close()


# Trick for reusing the script as a module
if __name__ == '__main__':
    main()
