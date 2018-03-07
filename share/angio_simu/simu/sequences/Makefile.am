EXTRA_DIST = GRE_2D_tra.xml GRE_3D_tra.xml PC_2D_tra_1dVx.xml PC_2D_tra_1dVy.xml PC_2D_tra_1dVz.xml PC_2D_tra_3dVxyz.xml TOF_2D_tra.xml Makefile.am

examplesdir = $(datadir)/jemris/angio_simu/simu/sequences
examples_DATA = $(EXTRA_DIST)

SUBDIRS = PC_3D_tra_3dVxyz     

clean-local:
	rm -f *.out *_signal.h5 

