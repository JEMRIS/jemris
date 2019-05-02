"""
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
"""


import os
import numpy as np
import numexpr as ne

from gui.caller import caller
from gui.instances import Sequence, Simulation

import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

import nibabel as nib

phencs = 32 # number of phase encoding step
echoes = 10 # number of echoes

path = '/home/user/python/jemris/jemris/simulations/km_se'
#simu = 'simu_km_se01'
#simu = 'simu_km_se02'
#simu = 'simu_km_se01b'
#simu = 'simu_km_se02b'
simu = 'simu_km_se03'

fname = os.path.join(path, simu)
#fname = os.path.join(path, 'simu_km_se02')

# Split in number of phase encodes, then in number of echoes
def restructure(X, phencs, echoes):
    subX = np.split(X.ravel(), phencs)
    tmp = [np.split(sX, echoes) for sX in subX]
    return np.array(tmp).transpose((0,2,1))

def func(x, a, b):
    return a * np.exp(-x/b)

jemris_caller = caller()

simulation = Simulation(fname)
simulation.readXML()
FOV = [128, 128, 1]
RES = [1, 1, 1]

sequence = Sequence(None, simulation.getSequence())
sequence.makeseq(jemris_caller)
A = sequence.readh5()

B = simulation.readh5()

S  = (B['S'][:,0] + 1j*B['S'][:,1])[:, None]

Kx = A['KX'][A['Iadc']][:, None]
Ky = A['KY'][A['Iadc']][:, None]
Kz = A['KZ'][A['Iadc']][:, None]

subS =  restructure(S, phencs, echoes)
subKx = restructure(Kx, phencs, echoes)
subKy = restructure(Ky, phencs, echoes)
subKz = restructure(Kz, phencs, echoes)

grid = [np.linspace(-np.floor(fov/2.), +np.ceil(fov/2.), fov/res) for fov, res in zip(FOV, RES)]

# Desired image size
x, y, z = np.meshgrid(grid[0], grid[1], grid[2])

X = x.ravel()[None, :]
Y = y.ravel()[None, :]
Z = z.ravel()[None, :]

shape = np.shape(x)
I = list()
for idx in range(np.shape(subS)[-1]):
    sS = subS[..., idx].ravel()[:, None]
    sKx = subKx[..., idx].ravel()[:, None]
    sKy = subKy[..., idx].ravel()[:, None]
    sKz = subKz[..., idx].ravel()[:, None]
    I.append(ne.evaluate(
            "sum(exp(+1j * (sKx*X + sKy*Y + sKz*Z)) * sS, axis=0)"
            ).reshape(shape))

I = np.array(I)
I = I.transpose((1,2,3,0))

#T = B['T']
#idx = np.where((T<8170) & (T>8000))
#
#plt.figure()
#plt.plot(T[idx],np.abs(S[idx]))
#plt.grid()
#plt.savefig(os.path.join(path,'%s_timesignal.svg' % simu))
##plt.savefig('/home/user/Desktop/se.svg')
#
#plt.figure()
#plt.imshow(np.abs(I[...,0,0]))
#
#
#
#TE = 15 + 15*np.arange(10)
#sTE = np.abs(I[64, 64, 0, :]).ravel()
#
#popt, pcov = curve_fit(func, TE, sTE)
#
#print popt[1]
#
#plt.figure()
#plt.plot(TE, sTE, '-o')
#plt.grid()
#plt.plot(TE, func(TE, popt[0], popt[1]))
##    plt.plot(TE, 0.74*np.exp(-TE/100.))
#plt.savefig(os.path.join(path,'%s_fit.svg' % simu))
#
#
#nii = nib.Nifti1Image(np.abs(I), np.eye(4))
#nib.save(nii, os.path.join(path, '%s.nii' % simu))





