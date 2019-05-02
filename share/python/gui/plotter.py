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

from matplotlib import pyplot as plt
from matplotlib import cm
from matplotlib import lines, spines
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from mpl_toolkits.mplot3d import Axes3D   # must be imported for 3d vis

import nibabel as nib

from PyQt5 import QtGui, QtWidgets
from gui.mayavi_widgets import QVolumeSlicer
from gui.ui import QTPForm, QTPClass, QVPForm, QVPClass


class QTimePlotter(QtWidgets.QWidget, QTPForm):

    def __init__(self, parent=None):

        super(QTimePlotter, self).__init__(parent)
        QTPClass.__init__(self)
        self.setupUi(self)

        self.pointSlider.valueChanged.connect(self.pointSliderChanged)
        self.pointEdit.editingFinished.connect(self.pointEditChanged)

        self.draw = True

    def setInstance(self, Instance):

        self.Instance = Instance

    def pointEditChanged(self):

        T0 = float(self.pointEdit.text())
        idx = np.abs(self.T-T0).argmin()
        self.pointSlider.setValue(idx)      # triggers sliderChanged

    def pointSliderChanged(self):

        T0 = self.T[self.pointSlider.value()]
        self.pointEdit.setText(str(int(round(T0))))
        self.plotCurrentPoint()

    def sync(self, index, draw):

        self.draw = draw
        self.pointSlider.setValue(index)    # triggers sliderChanged


class QTimeSeriesPlotter(QTimePlotter):

    def __init__(self, parent=None):

        QTimePlotter.__init__(self)

        self.timeSeriesPlotter = timeSeriesPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.timeSeriesPlotter.canvas)
        self.frame.setLayout(layout)

        self.intervalEdit.editingFinished.connect(self.plotCurrentInterval)

        self.stackedWidget.setCurrentIndex(0)
        self.intervalWidget.setCurrentIndex(1)

    def update(self):

        self.pointSlider.setMaximum(len(self.T)-1)
        self.plotCurrentInterval()

    def plotCurrentInterval(self):

        self.timeSeriesPlotter.plotCurrentInterval(self.intervalEdit.text())
        self.plotCurrentPoint()

    def plotCurrentPoint(self):

        if not self.draw:
            return

        self.timeSeriesPlotter.plotCurrentPoint(self.pointSlider.value())

    def export(self, msg, path):

        # Not used yet.

        plot = self.timeSeriesPlotter.fig

        fname = str(QtWidgets.QFileDialog.getSaveFileName(self,
                                                      msg,
                                                      path,
                                                      '*.jpeg'))

        if fname is not '':
            plot.savefig(fname, bbox_inches='tight',
                         facecolor=plot.get_facecolor(), transparent=True)


class QSeqDiagPlotter(QTimeSeriesPlotter):

    def __init__(self, parent=None):

        QTimeSeriesPlotter.__init__(self)

        self.checkBox.stateChanged.connect(self.init)

        self.stackedWidget.setCurrentIndex(1)

    def update(self):

        self.A = self.Instance.data
        self.T = self.Instance.data['T']
        self.timeSeriesPlotter.activate()
        self.timeSeriesPlotter.T = self.T   # cheap workaround

        self.init()

    def init(self):

        xVars = ['Tadc', 'T', 'T', 'T', 'T', 'T']
        xLabels = [None, None, 't \, [msec]', None, None, 't \, [msec]']

        if self.checkBox.isChecked():
            yVars = ['Rec_Phs', 'TXM', 'TXP', 'KX', 'KY', 'KZ']
            yLabels = ['\phi(RX)', '|TX|', '\phi(TX)', 'K_X', 'K_Y', 'K_Z']
        else:
            yVars = ['Rec_Phs', 'TXM', 'TXP', 'GX', 'GY', 'GZ']
            yLabels = ['\phi(RX)', '|TX|', '\phi(TX)', 'G_X', 'G_Y', 'G_Z']

        xVars = [[self.A[xvar]] for xvar in xVars]
        yVars = [[self.A[yvar]] for yvar in yVars]

        lstyle = [['%s' % ls] for ls in ['.r', '-b', '-b', '-b', '-b', '-b']]

        order = [int('32%i' % x) for x in [1, 3, 5, 2, 4, 6]]  # subplot order

        self.timeSeriesPlotter.initPlot(order, xVars, yVars,
                                        xLabels, yLabels, lstyle)

        QTimeSeriesPlotter.update(self)


class QTimeSignalPlotter(QTimeSeriesPlotter):

    def __init__(self, parent=None):

        QTimeSeriesPlotter.__init__(self)

        self.channelWidget.setCurrentIndex(1)

        self.channelBox.activated[str].connect(self.init)

    def update(self):

        self.channelBox.clear()

        if self.Instance.status:
            return

        for channel in self.Instance.channels:
            self.channelBox.addItem(channel)
        self.channelBox.addItem('All')

        self.T = self.Instance.data['T']
        self.S = self.Instance.data['S']

        self.timeSeriesPlotter.activate()
        self.timeSeriesPlotter.T = self.T   # cheap workaround

        self.init()

    def init(self):

        channel = self.channelBox.currentIndex()

        if self.channelBox.currentText() in ['All']:

            Mx = self.S[:, 0, :]
            My = self.S[:, 1, :]
            Mz = self.S[:, 2, :]

        else:

            Mx = self.S[:, 0, channel]
            My = self.S[:, 1, channel]
            Mz = self.S[:, 2, channel]

        Mt = (Mx + 1j*My)

        order = [int('31%i' % x) for x in [1, 2, 3]]  # subplot order
        xVars = [[self.T, self.T, self.T], [self.T], [self.T]]
        yVars = [[Mx, My, Mz], [np.abs(Mt)], [np.angle(Mt)]]
        xLabels = [None, None, 't \, [msec]']
        yLabels = ['M', '|M_T|', '\phi(M_T)']
        lstyle = [['b', 'g', 'r'], ['k'], ['k']]
        legend = [['Mx', 'My', 'Mz'], None, None]

        self.timeSeriesPlotter.initPlot(order, xVars, yVars,
                                        xLabels, yLabels, lstyle, legend)

        QTimeSeriesPlotter.update(self)


class timeSeriesPlotter(object):

    # base class

    def __init__(self):

        self.fig = plt.figure()
        self.canvas = FigureCanvasQTAgg(self.fig)
        self.active = False

    def activate(self):

        self.active = True

    def initPlot(self, order, xVars, yVars,
                 xLabels=None, yLabels=None, lstyle=None, legend=None):

        if not self.active:
            return

        # Defaults
        if xLabels is None:
            xLabels = [None for pos in order]
        if yLabels is None:
            yLabels = [None for pos in order]
        if lstyle is None:
            lstyle = ['b' for pos in order]
        if legend is None:
            legend = [None for pos in order]

        def convert2tex(_list):
            return [r'$\mathrm{%s}$' % _str if _str is not None else None
                    for _str in _list]

        xLabels = convert2tex(xLabels)
        yLabels = convert2tex(yLabels)

        for i, pos in enumerate(order):
            if i == 0:
                axes = self.fig.add_subplot(pos)
            else:
                axes = self.fig.add_subplot(pos,
                                            sharex=self.fig.axes[0])
            axes.clear()
            axes.grid(True)
            for x, y, ls in zip(xVars[i], yVars[i], lstyle[i]):
                axes.plot(x, y, ls, linewidth=2)
            if xLabels[i] is not None:
                axes.set_xlabel(xLabels[i], fontsize=20, fontweight='bold')
                axes.get_xaxis().set_label_coords(0.5, -0.2)
            if yLabels[i] is not None:
                axes.set_ylabel(yLabels[i], fontsize=20, fontweight='bold')
                axes.get_yaxis().set_label_coords(-0.2, 0.5)
            if legend[i] is not None:
                axes.legend(legend[i])

        self.fig.set_facecolor('white')
        self.fig.tight_layout()

        self.canvas.draw()
        self.cP = [None, None, None, None, None, None]

    def plotCurrentInterval(self, interval):

        if not self.active:
            return

        try:
            self.interval = int(interval)
        except:
            self.interval = None

        for i, axes in enumerate(self.fig.axes):
            if self.cP[i] is not None:      # Delete old object, Line objects
                    self.cP[i].remove()     # can be removed directly
            if self.interval is not None:
                line = lines.Line2D([0.5, 0.5], [0, 1],
                                    transform=axes.transAxes,
                                    linestyle='--', linewidth=2, color='black')
                self.cP[i] = axes.add_line(line)
            else:
                self.cP[i] = None
                axes.set_xlim((self.T.min(), self.T.max()))

        self.canvas.draw()

    def plotCurrentPoint(self, point):

        if not self.active:
            return

        # If Interval is Valid
        if self.interval is not None:
            # Shift Window
            self.fig.axes[0].set_xlim((self.T[point] - self.interval/2,
                                       self.T[point] + self.interval/2))

        else:
            # Draw Line at Current Position
            for i, axes in enumerate(self.fig.axes):
                if self.cP[i] is not None:
                    self.cP[i].remove()
                self.cP[i] = axes.axvline(x=self.T[point],
                                          linestyle='--', linewidth=2,
                                          color='black')

        self.canvas.draw()


class QKSpacePlotter(QTimePlotter):

    def __init__(self, parent=None):

        QTimePlotter.__init__(self)

        self.kSpaceTrajPlotter = kSpaceTrajPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.kSpaceTrajPlotter.canvas)
        self.frame.setLayout(layout)

        self.addADCs.stateChanged.connect(self.plotADCs)
        self.view3d.stateChanged.connect(self.plotkSpaceTraj)

        self.stackedWidget.setCurrentIndex(2)

    def update(self):

        A = self.Instance.data
        self.T = A['T']
        self.pointSlider.setMaximum(len(self.T)-1)

        self.kSpaceTrajPlotter.update(A)
        self.plotkSpaceTraj()

    def plotkSpaceTraj(self):

        self.kSpaceTrajPlotter.plotkSpaceTraj(False,
                                              self.view3d.isChecked())
        self.plotADCs()
        self.plotCurrentPoint()

    def plotADCs(self):

        self.kSpaceTrajPlotter.plotADCs(self.addADCs.isChecked(),
                                        self.view3d.isChecked())

    def plotCurrentPoint(self):

        if not self.draw:
            return
        self.kSpaceTrajPlotter.plotCurrentPoint(self.pointSlider.value(),
                                                self.view3d.isChecked())


class kSpaceTrajPlotter(object):

    def __init__(self):

        self.fig = plt.figure()
        self.canvas = FigureCanvasQTAgg(self.fig)
        self.active = False

    def update(self, A):

        self.KX = A['KX']
        self.KY = A['KY']
        self.KZ = A['KZ']
        self.J = A['J']
        self.Iadc = A['Iadc']
        self.active = True

    def plotkSpaceTraj(self, contdraw, view3d):

        if not self.active:
            return

        fgc = 'white'   # Foreground color
        bgc = 'black'   # Background color

        self.fig.clear()
        plt.figure(self.fig.number)     # Workaround to set current figure
        gridspec = plt.GridSpec(1, 50)  # Ratio Figure Width / Colorbar Width

        if not view3d:
            ax = plt.subplot(gridspec[0, :-1])
        else:
            ax = plt.subplot(gridspec[0, :-1], projection='3d')

        C = cm.autumn(np.linspace(0, 1, np.size(self.J)))[::-1, :]

        # Draw Actual Trajectory
        for j in range(np.size(self.J)):
            if j < np.size(self.J) - 1:
                n_end = self.J[j+1] - 2 + contdraw
            else:
                n_end = np.size(self.KX) - 1

            n_all = np.arange(start=self.J[j]+1-contdraw, stop=n_end, step=1)

            if not view3d:
                ax.plot(self.KX[n_all], self.KY[n_all],
                        c=C[j, :], linewidth=0.5)
            else:
                ax.plot(self.KX[n_all], self.KY[n_all], self.KZ[n_all],
                        c=C[j, :], linewidth=0.5)
                ax.view_init(elev=90, azim=-90)

        if (self.KX.min() < self.KX.max()) and (self.KY.min() < self.KY.max()):
            ax.axis([1.1*self.KX.min(), 1.1*self.KX.max(),
                     1.1*self.KY.min(), 1.1*self.KY.max()])

        ax.grid(True, color=fgc)
        ax.set_aspect('equal')

        # Set Labels
        labels = ['%s [rad / mm]' % s for s in ['Kx', 'Ky', 'Kz']]
        ax.set_xlabel(labels[0], fontsize=15, fontweight='bold', color=fgc)
        ax.set_ylabel(labels[1], fontsize=15, fontweight='bold', color=fgc)
        if view3d:
            ax.set_zlabel(labels[2], fontsize=15, fontweight='bold', color=fgc)

        # Add colorbar
        if np.size(self.J) > 2:
            cax = plt.subplot(gridspec[0, -1])
            sm = plt.cm.ScalarMappable(cmap=cm.autumn)
            sm._A = []      # fake up the array of the scalar mappable.
            h = plt.colorbar(sm, orientation='vertical', cax=cax, ax=ax)
            h.set_ticks([])
            h.set_label('Late \t\t\t\t Early'.expandtabs(),
                        fontsize=15, color=fgc)

        # Color elements in foreground color
        for child in ax.get_children():
            if isinstance(child, spines.Spine):
                child.set_color(fgc)

        ax.tick_params(axis='x', colors=fgc)
        ax.tick_params(axis='y', colors=fgc)
        ax.tick_params(axis='z', colors=fgc)

        # Color background
        self.fig.set_facecolor(bgc)
        ax.set_facecolor(bgc)

        # Transparent background
        if view3d:
            ax.w_xaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))
            ax.w_yaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))
            ax.w_zaxis.set_pane_color((0.0, 0.0, 0.0, 0.0))

        self.canvas.draw()
        self.cP = None
        self.ADCs = None

    def plotADCs(self, adc_flag, view3d):

        if not self.active:
            return

        if self.ADCs is not None:
            self.ADCs.pop(0).remove()   # Delete old object

        if adc_flag:
            if not view3d:
                self.ADCs = self.fig.axes[0].plot(self.KX[self.Iadc],
                                                  self.KY[self.Iadc], '.g')
            else:
                self.ADCs = self.fig.axes[0].plot(self.KX[self.Iadc],
                                                  self.KY[self.Iadc],
                                                  self.KZ[self.Iadc], '.g')
        else:
            self.ADCs = None

        self.canvas.draw()

    def plotCurrentPoint(self, cP, view3d):

        if not self.active:
            return

        if self.cP is not None:
            self.cP.pop(0).remove()     # Delete old object

        if not view3d:
            self.cP = self.fig.axes[0].plot([self.KX[cP]],
                                            [self.KY[cP]],
                                            marker='o', markersize=5,
                                            color='white')
        else:                           # Note: mplot3d needs list input
            self.cP = self.fig.axes[0].plot([self.KX[cP]],
                                            [self.KY[cP]],
                                            [self.KZ[cP]],
                                            marker='o', markersize=5,
                                            color='white')

        self.canvas.draw()


class QVolumePlotter(QtWidgets.QWidget, QVPForm):

    def __init__(self, parent=None):

        super(QVolumePlotter, self).__init__(parent)
        QVPClass.__init__(self)
        self.setupUi(self)

        self.volumePlotter = QVolumeSlicer()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.volumePlotter)
        self.frame.setLayout(layout)

        self.exportButton.clicked.connect(self.export)

        self.comboBox.activated['QString'].connect(self.plot)
        self.horizontalSlider.valueChanged.connect(self.update_index)

        self.stackedWidget.setCurrentIndex(0)

    def setInstance(self, Instance):

        self.Instance = Instance

    def update(self, trunk=None):

        self.trunk = trunk
        self.updated = True

        self.setLabel()                         # Needs to be done only once
        self.plot()

    def plot(self):

        self.setData()

        if self.updated:
            self.volumePlotter.update(self.label, self.data, self.cmap)
            self.updated = False
        else:
            self.volumePlotter.update_data(self.data, self.cmap)

    def update_index(self):

        idx = int(self.horizontalSlider.value())
        self.volumeEdit.setText(str(idx))
        self.volumePlotter.update_index(idx)

    def export(self):

        fname = str(QtGui.QFileDialog.getSaveFileName(None,
                                                      'Export',
                                                      '',
                                                      '*.nii'))

        if fname is not '':

            nii = nib.Nifti1Image(self.data, np.eye(4))
            nib.save(nii, fname)


class QRecoPlotter(QVolumePlotter):

    def __init__(self, parent=None):

        QVolumePlotter.__init__(self)

        self.comboBox.addItems(['Magnitude', 'Phase', 'Real', 'Imag'])

    def setLabel(self):

        trunk = self.trunk
#        trunk = self.Instance.data

        if len(trunk.shape) > 3:
            self.label = np.abs(trunk[..., 0])
        else:
            self.label = np.abs(trunk)

        if len(self.trunk) > 3:
            self.horizontalSlider.setMaximum(trunk.shape[3]-1)
            self.stackedWidget.setCurrentIndex(1)

    def setData(self):

        trunk = self.trunk

        _mode = self.comboBox.currentText()

        if _mode == 'Magnitude':
            self.data = np.abs(trunk)
        elif _mode == 'Phase':
            self.data = np.angle(trunk)
        elif _mode == 'Real':
            self.data = np.real(trunk)
        elif _mode == 'Imag':
            self.data = np.imag(trunk)

        self.cmap = 'gray'


class QParameterMapsPlotter(QVolumePlotter):

    def __init__(self, parent=None):

        QVolumePlotter.__init__(self)

        self.comboBox.addItems(['M0', 'T1', 'T2', 'T2s', 'CS', 'Xi', 'DB'])

    def setLabel(self):

        trunk = self.Instance
        self.label = trunk.smaps['M0']

    def setData(self):

        trunk = self.Instance
        _str = self.comboBox.currentText()

        self.maps = trunk.smaps
        self.DB = trunk.DB

        _str = str(_str)

        if _str in self.maps.keys():
            self.data = self.maps[_str]
        elif _str in ['DB']:
            self.data = self.DB

        self.cmap = 'spectral' if _str in ['DB'] else 'gray'


class QLabelMapPlotter(QVolumePlotter):

    def __init__(self, parent=None):

        QVolumePlotter.__init__(self)

        self.comboBox.setHidden(True)

    def setLabel(self):

        self.label = self.Instance.labelPhantom['data']
#        self.label = self.trunk

    def setData(self):

        self.data = self.Instance.labelPhantom['data']
#        self.data = self.trunk
        self.cmap = 'spectral'


class QSensitivityMapsPlotter(QVolumePlotter):

    def __init__(self, parent=None):

        QVolumePlotter.__init__(self)

        self.comboBox.addItems(['Magnitude', 'Phase', 'Real', 'Imag'])

    def update(self):

        QVolumePlotter.update(self, self.Instance.data)

    def setLabel(self):

        trunk = self.Instance.data

        if len(trunk.shape) > 3:
            self.label = np.abs(trunk[..., 0])
        else:
            self.label = np.abs(trunk)

        if len(trunk) > 3:
            self.horizontalSlider.setMaximum(trunk.shape[3]-1)
            self.stackedWidget.setCurrentIndex(1)

    def setData(self):

        trunk = self.Instance.data

        _mode = self.comboBox.currentText()

        self.maps = trunk[0]

        if _mode == 'Magnitude':
            self.data = np.abs(trunk)
        elif _mode == 'Phase':
            self.data = np.angle(trunk)
        elif _mode == 'Real':
            self.data = np.real(trunk)
        elif _mode == 'Imag':
            self.data = np.imag(trunk)

        self.cmap = 'spectral'
