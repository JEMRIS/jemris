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

import numpy as np

from gui.instances import Sequence
from gui.caller import caller

from numpy.fft import fftshift
from numpy.fft.fftpack import fftn, ifftn

from PyQt5 import QtCore, QtWidgets


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
        # Get sequence parameters
        sequence.readXML()
        param = sequence.xml.xml
        sequence.readh5()
        A = sequence.data

        simulation.readh5()
        B = simulation.data

        self.param = dict()
        self.param['FOV'] = (int(param.get('FOVx')),
                             int(param.get('FOVy')),
                             1)
        self.param['N'] = (int(param.get('Nx')), int(param.get('Ny')), 1)

        self.S = (B['S'][:, 1, :] - 1j*B['S'][:, 0, :])

        self.Kx = A['KX'][A['Iadc']][:, None]
        self.Ky = A['KY'][A['Iadc']][:, None]
        self.Kz = A['KZ'][A['Iadc']][:, None]

        self.thread_finished.connect(eFunction)

    def run(self):

        recon = CartRecon(self.S, self.Kx, self.Ky, self.Kz,
                          self.reco, self.param)

        self.thread_finished.emit(recon)


def CartRecon(S, Kx, Ky, Kz, reco, param, mode):

    S = S.reshape(param['N'] + S.shape[1:])

    if reco['PRx'] in ['Sum of Squares']:

        x = icfftn(S)

        x = np.sqrt(np.sum(np.abs(x)**2, axis=-1))

    else:

        x = icfftn(S[:, reco['CH']])

    return x


def cfft(x, axes=(0, 1)):

    x = fftshift(x, axes=axes)
    x = fftn(x, axes=axes, norm='ortho')
    x = fftshift(x, axes=axes)

    return x


def icfftn(x, axes=(0, 1)):

    x = fftshift(x, axes=axes)
    x = ifftn(x, axes=axes, norm='ortho')
    x = fftshift(x, axes=axes)

    return x
