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

from __future__ import division

import os
import numpy as np
import numexpr as ne

from gui.instances import Sequence, Simulation
from gui.caller import caller

from numpy.fft import fftshift
from numpy.fft.fftpack import fftn, ifftn

slice_enc = False

from PyQt5 import QtCore, QtGui, QtWidgets

from nfft import nufft


class Reconstructor(QtCore.QThread):

    thread_finished = QtCore.pyqtSignal(object)

    def __init__(self, simulation, reco, exitFunction, parent=None):

        super(Reconstructor, self).__init__(parent)
        self.simulation = simulation
        self.reco = reco
        self.statusbar = parent.window().statusBar()

        progressbar = QtWidgets.QProgressBar()
        progressbar.setMaximumHeight(15)
        progressbar.setMinimum(0)
        progressbar.setMaximum(0)
        self.statusbar.addWidget(progressbar)

        def eFunction(I):
            self.statusbar.removeWidget(progressbar)
            exitFunction(I)

        jcaller = caller()
        jcaller.connect()
#        jcaller.setStatusBar(None)
        jcaller.setMultiThreaded(False)

        sequence = Sequence(None, simulation.getSequence())
        sequence.make(jcaller, None)
        sequence.readh5()
        A = sequence.data

        simulation.readh5()
        B = simulation.data

#        self.S = B['S'][:, 0, :] + 1j*B['S'][:, 1, :]
        self.S = (B['S'][:, 1, :] - 1j*B['S'][:, 0, :])

        self.Kx = A['KX'][A['Iadc']][:, None]
        self.Ky = A['KY'][A['Iadc']][:, None]
        self.Kz = A['KZ'][A['Iadc']][:, None]

        self.thread_finished.connect(eFunction)

    def run(self):

        if self.reco['type'] == 'naive':
            I = MultiChannelReconstruction(self.S, self.Kx, self.Ky, self.Kz, self.reco, 'Naive')
        if self.reco['type'] == 'cartesian':
            I = MultiChannelReconstruction(self.S, self.Kx, self.Ky, self.Kz, self.reco, 'Cartesian')
        if self.reco['type'] == 'epik':
            I = epik(self.S, self.Kx, self.Ky, self.Kz, self.reco)
        if self.reco['type'] == 'cssme':
            I = MultiChannelReconstruction(self.S, self.Kx, self.Ky, self.Kz, self.reco, 'cssme')

        self.thread_finished.emit(I)


def MultiChannelReconstruction(S, Kx, Ky, Kz, reco, mode):

    if mode == 'Naive':
        Reconstruction = NaiveReconstruction
    elif mode == 'Cartesian':
        Reconstruction = CartesianReconstruction
    elif mode == 'cssme':
        Reconstruction = cssme

    if reco['PRx'] in ['Sum of Squares']:

        I = list()

        for i in range(np.shape(S)[-1]):

            I.append(Reconstruction(S[:, i][:, None], Kx, Ky, Kz, reco))

        I = np.array(I)
        I = np.sqrt(np.sum(np.abs(I)**2, axis=0))

    else:

        I = Reconstruction(S[:, reco['CH']][:, None], Kx, Ky, Kz, reco)

    return I


def NaiveReconstruction(S, Kx, Ky, Kz, reco):

    FOV = reco['FOV']
    RES = reco['RES']
    repetitions = reco['repetitions']

#    # NAIVE RECONSTRUCTION
#
#    grid = [np.linspace(-np.floor(fov/2.),
#                        +np.ceil(fov/2.),
#                        fov/res) for fov, res in zip(FOV, RES)]
#
#    # Desired image size
#    y, x, z = np.meshgrid(grid[0], grid[1], grid[2])
#
#    X = x.ravel()[None, :]
#    Y = y.ravel()[None, :]
#    Z = z.ravel()[None, :]
#
#    print 'Start reconstruction ...'
#
#    subS = np.split(S, repetitions)
#    subKx = np.split(Kx, repetitions)
#    subKy = np.split(Ky, repetitions)
#    subKz = np.split(Kz, repetitions)
#
#    expr = "sum(exp(+1j * (sKx*X + sKy*Y + sKz*Z)) * sS, axis=0)"
#
#    I = list()
#    for sS, sKx, sKy, sKz in zip(subS, subKx, subKy, subKz):
#        I.append(ne.evaluate(expr).reshape(np.shape(x)))
#
#    I = np.array(I).transpose(1, 2, 3, 0)
#

#    print Kx.shape, Ky.shape, S.shape

    nfft = nufft([FOV[0]/RES[0], FOV[1]/RES[1]], [Kx.ravel()*RES[0]/2/np.pi,
                                                  Ky.ravel()*RES[1]/2/np.pi])
    I = nfft.ifft(S.ravel(), True)[..., None, None]

#    print I.shape
#
#    print '... finished.'

    return I


def CartesianReconstruction(S, Kx, Ky, Kz, reco):

    repetitions = reco['repetitions']

    # Assuming Nx = Ny, Nz = 1

    Nx = int(np.sqrt(np.size(S)/repetitions))
    Ny = Nx
    Nz = 1

    def gridding(K, N):
        K = K - K.min()
        if K.max() > 0:
            K = np.round(K/K.max()*(N-1)).astype(int)
        else:
            K = K.astype(int)
        return K

    Kx = gridding(Kx, Nx)
    Ky = gridding(Ky, Ny)
    Kz = gridding(Kz, Nz)

    subS = np.split(S, repetitions)
    subKx = np.split(Kx, repetitions)
    subKy = np.split(Ky, repetitions)
    subKz = np.split(Kz, repetitions)

    I = list()
    for sS, sKx, sKy, sKz in zip(subS, subKx, subKy, subKz):
        spectrum = np.zeros((Nx, Ny, Nz)).astype(np.complex)
        spectrum[sKx, sKy, sKz] = sS
        I.append(spectrum)

    I = np.array(I)

    I = I.transpose(1, 2, 3, 0)

    I = mifftn(I)

#    I = fftshift(ifftn(I, axes=(0, 1)), axes=(0, 1))

    print(np.shape(I))

    return I


def epik(S, Kx, Ky, Kz, reco):

    # TODO: Naming inccorect. Shoots = Repetitions, Subsamp = Shots

    shots = reco['repetitions']
    subsamp = reco['shots']
    keyhole = reco['keyhole']

    Nx, Ny, Nz = [64, 64, 1]

    # EPIK RECONSTRUCTION

    def gridding(K, N):
        K = K - K.min()
        if K.max() > 0:
            K = np.round(K/K.max()*(N-1)).astype(int)
        else:
            K = K.astype(int)
        return K

    Kx = gridding(Kx, Nx)
    Ky = gridding(Ky, Ny)
    Kz = gridding(Kz, Nz)

#    Kx = Kx - Kx.min()
#    Ky = Ky - Ky.min()
#    Kz = Kz - Kz.min()
#    if Kx.max() > 0:
#        Kx = np.round(Kx/Kx.max()*(Nx-1)).astype(int)
#    if Ky.max() > 0:
#        Ky = np.round(Ky/Ky.max()*(Ny-1)).astype(int)
#    if Kz.max() > 0:
#        Kz = np.round(Kz/Kz.max()*(Nz-1)).astype(int)

    subS = np.split(S, subsamp*shots)
    subKx = np.split(Kx, subsamp*shots)
    subKy = np.split(Ky, subsamp*shots)
    subKz = np.split(Kz, subsamp*shots)

    I = list()
    for sS, sKx, sKy, sKz in zip(subS, subKx, subKy, subKz):
        spectrum = np.zeros((Nx, Ny, Nz)).astype(np.complex)
        spectrum[sKx, sKy, sKz] = sS
        I.append(spectrum)

    I = np.array(I)

    Irec = np.zeros(np.shape(I), dtype=complex)

    for idx in range(0, subsamp-1):

        Irec[idx, :, :(Ny-keyhole)/2, :] = np.sum(I[:subsamp, :, :(Ny-keyhole)/2, :], axis=0)
        Irec[idx, :, (Ny+keyhole)/2:, :] = np.sum(I[:subsamp, :, (Ny+keyhole)/2:, :], axis=0)
        Irec[idx, :, (Ny-keyhole)/2:(Ny+keyhole)/2, :] = I[idx, :, (Ny-keyhole)/2:(Ny+keyhole)/2, :]

    for idx in range(subsamp-1, I.shape[0]):

        Irec[idx, :, :(Ny-keyhole)/2, :] = np.sum(I[idx-subsamp+1:idx+1, :, :(Ny-keyhole)/2, :], axis=0)
        Irec[idx, :, (Ny+keyhole)/2:, :] = np.sum(I[idx-subsamp+1:idx+1, :, (Ny+keyhole)/2:, :], axis=0)
        Irec[idx, :, (Ny-keyhole)/2:(Ny+keyhole)/2, :] = I[idx, :, (Ny-keyhole)/2:(Ny+keyhole)/2, :]

    I = Irec

    I = I.transpose(1, 2, 3, 0)

#    I[0:2, :, (Ny-keyhole)/2:(Ny+keyhole)/2] = 0
#
#    I = np.sum(I, axis=0)

    I = fftshift(ifftn(I, axes=(0, 1)), axes=(0, 1))

#    I[...,1:6] = I[...,1:6] - I[...,0:5]

#    I = I[:,:, None, :]

    return I


def cssme(S, Kx, Ky, Kz, reco):

    echoes = reco['echo']
    phencs = reco['ky']

    def reorder(X):

        X = np.split(X, phencs)
        X = [np.split(x, echoes) for x in X]
        X = np.array(X)
        X = np.transpose(X, (0, 2, 1))

#        # Bipolar Read-Out
#        X[:, :, 1::2] = X[:, ::-1, 1::2]
        return X

    S = reorder(S.ravel())

    I = S

    I = I[:, :, None, :]

#    I[:, :, :, 1::2] = I[:, ::-1, :, 1::2]

    I = mifftn(I)

#    I = fftshift(ifftn(I, axes=(0, 1)), axes=(0, 1))

    I = I.transpose(1, 0, 2, 3)

    return I


def mfftn(x):

    x = fftshift(x, axes=(0, 1))
    x = fftn(x, axes=(0, 1), norm='ortho')
    x = fftshift(x, axes=(0, 1))
    return x


def mifftn(x):

    x = fftshift(x, axes=(0, 1))
    x = ifftn(x, axes=(0, 1), norm='ortho')
    x = fftshift(x, axes=(0, 1))
    return x