%
% Jemris Matlab GUI
%
% Table of Contents (T0C)
% -----------------------
%  JEMRIS_seq.m	  sequence development GUI
%  JEMRIS_sim.m	  simulation GUI
%
%  calcBfromXi.m  calculate field inhomogeneities from matlab command line
%
%  userSample.mat example matlab file for a self defined sample
%
%  MNIbrain.mat	  MNI human brain phantom from http://www.bic.mni.mcgill.ca/brainweb/
%  MNIdeltaB.mat  corresponding susceptibility-induced B0-shift
%
%  All remaining files are small helper functions for the GUIs:
%
%  brainSample.m
%  changeSeqAttributes.m
%  changeSeqTree.m
%  getAllModules.m
%  parseXMLseq.m
%  plotseq.m
%  plotSeqTree.m
%  plotsim.m
%  readEvol.m
%  seqcad_uitoolbar.m
%  writeSample.m
%  writeXMLseq.m
%  ModuleIcons.mat
%

%
%  JEMRIS Copyright (C) 
%                        2006-2019  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2019  Daniel Pflugfelder
%                                  
%
%  This program is free software; you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation; either version 2 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program; if not, write to the Free Software
%  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
%

