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
from PyQt5 import QtGui, QtCore, QtWidgets

from gui.caller import caller
from gui.widgets import QTreeWidget, QTableWidget, QToolBar

from gui.parser import parseXML
from gui.instances import Sequence, Phantom, Array, Simulation

from gui.plotter import QSeqDiagPlotter, QKSpacePlotter, QRecoPlotter, \
                        QSensitivityMapsPlotter, QTimeSignalPlotter, \
                        QParameterMapsPlotter, QLabelMapPlotter

from gui.settings import path as settings_path
from gui.settings import sequencePath, arrayPath, \
                         phantomPath, simulationPath, \
                         workspace

from gui.icon import ModuleIcons
from gui.logo import path as logo_path

from gui.recon import Reconstructor

from gui.ui import JMainForm, JMainClass, \
                   JSeqForm, JSeqClass, JPhaForm, JPhaClass, \
                   JSimForm, JSimClass, JArrForm, JArrClass, \
                   QSimOptForm, QSimOptClass, QRecOptForm, QRecOptClass, \
                   QXMLWidgetForm, QXMLWidgetClass, \
                   QPreferencesForm, QPreferencesClass, QAboutForm, QAboutClass


class JEMRIS(QtWidgets.QMainWindow, JMainForm):

    def __init__(self, parent=None):

        QtWidgets.QMainWindow.__init__(self, parent)
        JMainClass.__init__(self)
        self.setupUi(self)

        # TODO: Qt Designer + Resources File
        self.setWindowIcon(QtGui.QIcon(logo_path))

        self.preferences = QPreferences(self)
        self.about = QAbout(self)

        self.actionPreferences.triggered.connect(self.preferences.show)
        self.actionAbout.triggered.connect(self.about.show)
        self.actionConnect.triggered.connect(self.connectToJEMRIS)

        " CALLER ############################################################ "

        self.caller = caller()
        self.caller.setStatusBar(self.statusBar())
        self.versionLabel = QtWidgets.QLabel()
        self.statusBar().addPermanentWidget(self.versionLabel)

        " CENTRAL WIDGET #################################################### "

        icons = ModuleIcons()

        self.JEMRIS_seq_super = JEMRIS_seq_super(self.caller, icons)
        self.JEMRIS_pha_super = JEMRIS_pha_super(self.caller, icons)
        self.JEMRIS_arr_super = JEMRIS_arr_super(self.caller, icons)
        self.JEMRIS_sim_super = JEMRIS_sim_super(self.caller, icons)

        self.tabs = [self.JEMRIS_seq_super,
                     self.JEMRIS_pha_super,
                     self.JEMRIS_arr_super,
                     self.JEMRIS_sim_super]

        self.tab_names = ['Sequences',
                          'Phantoms',
                          'Arrays',
                          'Simulations']

        for tab, tab_name in zip(self.tabs, self.tab_names):
            self.tabWidget.addTab(tab, tab_name)

        # Workaround to reorder Actions
        # TODO: Find clean solution
        tmpBar = QtWidgets.QMenuBar()
        for action in self.menuBar().actions():
            tmpBar.addAction(action)

        for tab in self.tabs:
            tab.menuBar().setHidden(True)
        #    for action in tab.menuBar().actions():
        #        self.menuBar().addAction(action)

        for action in tmpBar.actions():
            self.menuBar().addAction(action)

        self.connectToJEMRIS()

    def connectToJEMRIS(self):

        p = self.preferences
        validSettings = p.checkWorkspace() and p.checkJpath()

        if validSettings:

            self.caller.connect()

            self.versionLabel.setText(self.caller.version)
            self.statusBar().showMessage('Connected to JEMRIS.', msecs=1e4)

            self.menuBar().actions()[0].setEnabled(True)

            self.JEMRIS_seq_super.getAttrib()
            self.JEMRIS_arr_super.getAttrib()
        else:
            p.show()

        for tab in self.tabs:
            tab.toolBarActionActive = validSettings


class JEMRIS_super(QtWidgets.QMainWindow):

    def __init__(self, module, tag, parent=None):

        super(JEMRIS_super, self).__init__(parent)

        self.module = module

        self.filemenu = self.menuBar().addMenu('&File')

        self.openTag = QtWidgets.QAction('Open %s' % tag, self)
        self.openTag.setShortcut('Ctrl+O')
        self.openTag.triggered.connect(self.openInstance)

        self.newTag = QtWidgets.QAction('New %s' % tag, self)
        self.newTag.triggered.connect(self.newInstance)

        self.saveTag = QtWidgets.QAction('Save %s' % tag, self)
        self.saveTag.setShortcut('Ctrl+S')
        self.saveTag.triggered.connect(self.saveInstance)

        self.saveAsTag = QtWidgets.QAction('Save %s as' % tag, self)
        self.saveAsTag.triggered.connect(self.saveInstanceAs)

        self.toolBarActionActive = False

        """ CENTRAL WIDGET """

        self.tabWidget = QtWidgets.QTabWidget()
        self.setCentralWidget(self.tabWidget)
        self.tabWidget.setTabsClosable(True)
        self.tabWidget.setMovable(True)
        self.tabWidget.tabCloseRequested.connect(self.closeTab)

        self.Instances = list()

    def closeTab(self, tabToCloseIndex):

        self.tabWidget.widget(tabToCloseIndex).deleteLater()
        self.tabWidget.removeTab(tabToCloseIndex)
        del self.Instances[tabToCloseIndex]

    def openInstance(self):

        print('Open Instance')

        self.Instances.append(self.module())
        fname = self.Instances[-1].openInstance()
        if fname is not None:
            self.tabWidget.addTab(self.Instances[-1], fname)
            self.tabWidget.setCurrentWidget(self.Instances[-1])

    def newInstance(self):

        self.Instances.append(self.module())
        fname = self.Instances[-1].newInstance()
        if fname is not None:
            self.tabWidget.addTab(self.Instances[-1], fname)
            self.tabWidget.setCurrentWidget(self.Instances[-1])

    def saveInstance(self):

        fname = self.Instances[self.tabWidget.currentIndex()].saveInstance()
        if fname is not None:
            self.tabWidget.setTabText(self.tabWidget.currentIndex(), fname)

    def saveInstanceAs(self):

        fname = self.Instances[self.tabWidget.currentIndex()].saveInstanceAs()
        if fname is not None:
            self.tabWidget.setTabText(self.tabWidget.currentIndex(), fname)

    def toolBarAction(self):
        if not self.toolBarActionActive:
            return

        tag = str(self.sender().text())

        if tag == 'OPEN':
            self.openInstance()
        elif tag == 'NEW':
            self.newInstance()
        elif tag == 'SAVE':
            self.saveInstance()
        elif tag == 'SAVEAS':
            self.saveInstanceAs()
        else:
            self.treeItemInsert()

    def treeItemInsert():
        pass


class JEMRIS_seq_super(JEMRIS_super):

    def __init__(self, jemris_caller, icons, parent=None):

        self.jemris_caller = jemris_caller
        self.icons = icons

        def module():
            return JEMRIS_seq_sub(self.attrib, self.icons, self.jemris_caller)

        super(JEMRIS_seq_super, self).__init__(module, 'Sequence', parent)

        order = ['OPEN', 'NEW', 'SAVE', 'SAVEAS', 'SEPARATOR',
                 'CONCATSEQUENCE',      'ATOMICSEQUENCE',
                 'DELAYATOMICSEQUENCE', 'EMPTYPULSE', 'SEPARATOR',
                 'ANALYTICGRADPULSE',   'CONSTANTGRADPULSE',
                 'EXTERNALGRADPULSE',   'SPIRALGRADPULSE',
                 'TRAPGRADPULSE',       'TRIANGLEGRADPULSE',
                 'ANALYTICRFPULSE',     'EXTERNALRFPULSE',
                 'GAUSSIANRFPULSE',     'HARDRFPULSE',
                 'SECHRFPULSE',         'SINCRFPULSE']

        self.addToolBar(QToolBar(self.icons, order, self.toolBarAction))

    def getAttrib(self):
        self.attrib = parseXML(self.jemris_caller.loadmod())

    def treeItemInsert(self):
        if self.tabWidget.currentIndex() is not None:
            self.Instances[self.tabWidget.currentIndex()].treeItemInsert()


class JEMRIS_pha_super(JEMRIS_super):

    def __init__(self, jemris_caller, icons, parent=None):

        self.icons = icons

        def module():
            return JEMRIS_pha_sub()

        super(JEMRIS_pha_super, self).__init__(module, 'Phantom', parent)

        order = ['OPEN', 'NEW', 'SAVE', 'SAVEAS', 'SEPARATOR']

        self.addToolBar(QToolBar(self.icons, order, self.toolBarAction))


class JEMRIS_arr_super(JEMRIS_super):

    def __init__(self, jemris_caller, icons, parent=None):

        self.jemris_caller = jemris_caller
        self.icons = icons

        def module():
            return JEMRIS_arr_sub(self.attrib, self.icons, self.jemris_caller)

        super(JEMRIS_arr_super, self).__init__(module, 'Array', parent)

        order = ['OPEN', 'NEW', 'SAVE', 'SAVEAS', 'SEPARATOR',
                 'BIOTSAVARTLOOP', 'ANALYTICCOIL', 'EXTERNALCOIL']

        self.addToolBar(QToolBar(self.icons, order, self.toolBarAction))

    def getAttrib(self):
        self.attrib = parseXML(self.jemris_caller.loadmod())

    def treeItemInsert(self):
        if self.tabWidget.currentIndex() is not None:
            self.Instances[self.tabWidget.currentIndex()].treeItemInsert()


class JEMRIS_sim_super(JEMRIS_super):

    def __init__(self, jemris_caller, icons, parent=None):

        self.jemris_caller = jemris_caller
        self.icons = icons

        def module():
            return JEMRIS_sim_sub(self.jemris_caller)

        super(JEMRIS_sim_super, self).__init__(module, 'Simulation', parent)

        order = ['OPEN', 'NEW', 'SAVE', 'SAVEAS', 'SEPARATOR']

        self.addToolBar(QToolBar(self.icons, order, self.toolBarAction))


class JEMRIS_sub(QtWidgets.QMainWindow):

    def __init__(self, module, parent=None):

        super(JEMRIS_sub, self).__init__(parent)

        self.module = module
        self.module['new'] = 'New %s' % self.module['type']
        self.module['open'] = 'Open %s %s file' % (self.module['type'],
                                                   self.module['ext'])
        self.module['saveAs'] = 'Save %s %s file' % (self.module['type'],
                                                     self.module['ext'])

    def openInstance(self):

        _open = QtWidgets.QFileDialog.getOpenFileName
        fname = _open(self, self.module['open'],
                      self.module['path'], ('*.%s' % self.module['ext']))[0]

        print(fname, type(fname))

        if not (fname == ''):
            self.Instance = self.module['instance'](fname)
            self.readXML()

            return self.Instance.FileName

    def newInstance(self):

        self.Instance = self.module['instance'](None)
        self.Instance.save()
        self.readXML()

        return self.module['new']

    def saveInstance(self):

        if not self.Instance.tmpname:
            self.Instance.save()
        else:
            return self.saveInstanceAs()

    def saveInstanceAs(self):

        _save = QtWidgets.QFileDialog.getSaveFileName
        fname, filt = _save(
                self, self.module['saveAs'], self.module['path'],
                ('*.%s' % self.module['ext']))

        if not (fname == ''):
            self.Instance.rename(fname)
            self.Instance.save()
            return self.Instance.FileName


class JEMRIS_seq_sub(JEMRIS_sub, JSeqForm):

    def __init__(self, attrib, icons, jemris_caller, parent=None):

        def instance(fname):
            return Sequence(attrib, fname)

        module = dict()
        module['type'] = 'Sequence'
        module['instance'] = instance
        module['path'] = sequencePath()
        module['ext'] = 'xml'

        super(JEMRIS_seq_sub, self).__init__(module, parent)
        JSeqClass.__init__(self)
        self.setupUi(self)

        self.jemris_caller = jemris_caller
        self.icons = icons

        self.qXMLWidget = QXMLWidget(module, self.jemris_caller, self.icons,
                                     self.seq_dump, self.update)
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.qXMLWidget)
        self.tab_seqtree.setLayout(layout)

        self.seqDiagPlotter = QSeqDiagPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.seqDiagPlotter)
        self.tab_seqdiag.setLayout(layout)

        self.kSpaceTrajPlotter = QKSpacePlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.kSpaceTrajPlotter)
        self.tab_kspace.setLayout(layout)

        self.seqDiagPlotter.pointSlider.valueChanged.connect(self.Sync)
        self.kSpaceTrajPlotter.pointSlider.valueChanged.connect(self.Sync)
        self.tabWidget.currentChanged.connect(self.Sync)

    def Sync(self):

        if self.tabWidget.currentIndex() == 2:

            source = self.seqDiagPlotter
            target = self.kSpaceTrajPlotter

        elif self.tabWidget.currentIndex() == 3:

            source = self.kSpaceTrajPlotter
            target = self.seqDiagPlotter

        else:

            return

        if not source.draw:
            source.draw = True
            source.plotCurrentPoint()

        target.sync(source.pointSlider.value(), False)

    def update(self):

        self.seqDiagPlotter.update()
        self.kSpaceTrajPlotter.update()

    def treeItemInsert(self):
        # Emergency solution
        self.qXMLWidget.treeItemInsert()

    def readXML(self):

        self.qXMLWidget.setInstance(self.Instance)
        self.seqDiagPlotter.setInstance(self.Instance)
        self.kSpaceTrajPlotter.setInstance(self.Instance)
        self.qXMLWidget.readXML()


class JEMRIS_pha_sub(JEMRIS_sub, JPhaForm):

    # TODO: Complete overhaul needed to make it more simple and similar to
    # Sequence, Array and Simulation.

    def __init__(self, parent=None):

        def instance(fname):
            return Phantom(fname)

        module = dict()
        module['type'] = 'Phantom'
        module['instance'] = instance
        module['path'] = phantomPath()
        module['ext'] = 'h5'

        super(JEMRIS_pha_sub, self).__init__(module, parent)
        JPhaClass.__init__(self)
        self.setupUi(self)

        self.labelMapsPlotter = QLabelMapPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.labelMapsPlotter)
        self.lMapsPlotter.setLayout(layout)

        self.parameterMapsPlotter = QParameterMapsPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.parameterMapsPlotter)
        self.pMapsPlotter.setLayout(layout)

        self.combo_sample.activated[str].connect(self.selectGeometricPhantom)

        # TODO: This should come from Phantom Instance
        self.MAP = ['M0', 'T1', 'T2', 'T2s', 'CS', 'Xi']
        self.RES = ['RESX', 'RESY', 'RESZ']
        self.OFF = ['OFFX', 'OFFY', 'OFFZ']
        self.MIN = ['MINX', 'MINY', 'MINZ']
        self.MAX = ['MAXX', 'MAXY', 'MAXZ']

        self.tedits = {'M0':    self.editM0,    'T1':   self.editT1,
                       'T2':    self.editT2,    'T2s':  self.editT2s,
                       'CS':    self.editCS,    'Xi':   self.editXi,
                       'RESX':  self.editResX,  'RESY': self.editResY,
                       'RESZ':  self.editResZ,  'OFFX': self.editOffX,
                       'OFFY':  self.editOffY,  'OFFZ': self.editOffZ,
                       'MINX':  self.editMinX,  'MAXX': self.editMaxX,
                       'MINY':  self.editMinY,  'MAXY': self.editMaxY,
                       'MINZ':  self.editMinZ,  'MAXZ': self.editMaxZ}
        for key in self.tedits.keys():
            self.tedits[key].setObjectName(key)
            self.tedits[key].editingFinished.connect(self.updateMaps)

        self.buttonMaps.clicked.connect(self.computeMaps)

        # self.tab_parametermaps.setEnabled(False)
        # self.scalingFrame.setEnabled(False)

        self.buttonLabelPhantom.clicked.connect(self.selectLabelPhantom)
        self.buttonLabelMatrix.clicked.connect(self.selectLabelMatrix)

    def selectLabelPhantom(self):

        msg = 'Select Label Phantom Nifti File'
        fname = str(QtWidgets.QFileDialog.getOpenFileName(
                self, msg, phantomPath(), ("*.nii"))[0])
        if not (fname == ''):

            self.editLabelPhantom.setText(fname)
            # Create new Sequence instance
            self.Instance.setLabelPhantom(fname, isfile=True)
            self.buttonGeometricPhantom.setChecked(False)
            self.initPhantom()

    def selectGeometricPhantom(self):

        self.buttonGeometricPhantom.setChecked(True)
        _str = self.combo_sample.currentText()
        self.Instance.setLabelPhantom(_str, isfile=False)
        self.initPhantom()

    def selectLabelMatrix(self):

        msg = 'Select Label Matrix Excel File'
        fname = str(QtWidgets.QFileDialog.getOpenFileName(
                self, msg, phantomPath(), ("*.xlsx"))[0])

        if not (fname == ''):

            self.editLabelMatrix.setText(fname)
            # Create new Sequence instance
            self.Instance.setLabelMatrix(fname)

            materials = self.Instance.labelMatrix['materials']
            labels = self.Instance.labelMatrix['labels']
            matrix = self.Instance.labelMatrix['matrix']

            self.tableWidget.setRowCount(len(materials))
            self.tableWidget.setColumnCount(len(labels))
            self.tableWidget.setHorizontalHeaderLabels(labels)
            self.tableWidget.setVerticalHeaderLabels(materials)

            for row in range(matrix.shape[0]):
                for col in range(matrix.shape[1]):
                    Qitem = QtWidgets.QTableWidgetItem(str(matrix[row, col]))
                    Qitem.setFlags(QtCore.Qt.ItemIsEnabled)
                    self.tableWidget.setItem(row, col, Qitem)

    def initPhantom(self):

        self.labelMapsPlotter.update()

        for idx, key in enumerate(self.MAX):
            shape = self.Instance.labelPhantom['shape']
            self.tedits[key].setText(str(shape[idx]))

    def computeMaps(self):

        if not self.Instance.mapsComputed:
            self.Instance.computeParameterMaps()
        # Now check again
        if self.Instance.mapsComputed:
            # self.scalingFrame.setEnabled(True)
            self.Instance.computeScaledMaps()
            self.parameterMapsPlotter.update()

    def updateMaps(self):

        key = str(self.sender().objectName())
        text = self.sender().text()

        if key in self.MAP:
            self.Instance.sfacs[key] = float(text)
        elif key in self.RES:
            self.Instance.RES[self.RES.index(key)] = float(text)
        elif key in self.OFF:
            self.Instance.OFF[self.OFF.index(key)] = float(text)
        elif key in self.MIN:
            idx = self.MIN.index(key)
            _min = 0
            _max = self.Instance.range[idx][1]
            try:
                newmin = int(text)
            except:
                newmin = _min
            if (newmin < _min) or (newmin > _max):
                self.sender().setText(str(_min))
            else:
                self.Instance.range[idx][0] = newmin
        elif key in self.MAX:
            idx = self.MAX.index(key)
            _min = self.Instance.range[idx][0]
            _max = self.Instance.labelPhantom['shape'][idx]
            try:
                newmax = int(text)
            except:
                newmax = _max
            if (newmax > _max) or (newmax < _min):
                self.sender().setText(str(_max))
            else:
                self.Instance.range[idx][1] = newmax

    def readXML(self):

        self.labelMapsPlotter.setInstance(self.Instance)
        self.parameterMapsPlotter.setInstance(self.Instance)

        self.Instance.readh5()
        self.computeMaps()
        self.scalingFrame.setEnabled(False)

    def newInstance(self):

        self.Instance = Phantom(None)

        self.labelMapsPlotter.setInstance(self.Instance)
        self.parameterMapsPlotter.setInstance(self.Instance)

        # TODO: Move this to a proper place.
        self.combo_sample.clear()
        for item in self.Instance.geometricPhantoms:
            self.combo_sample.addItem(item)

        return 'New Phantom'


class JEMRIS_arr_sub(JEMRIS_sub, JArrForm):

    def __init__(self, attrib, icons, jemris_caller, parent=None):

        def instance(fname):
            return Array(attrib, fname)

        module = dict()
        module['type'] = 'Array'
        module['instance'] = instance
        module['path'] = arrayPath()
        module['ext'] = 'xml'

        super(JEMRIS_arr_sub, self).__init__(module, parent)
        JArrClass.__init__(self)
        self.setupUi(self)

        self.attrib = attrib
        self.jemris_caller = jemris_caller
        self.icons = icons

        self.qXMLWidget = QXMLWidget(module, self.jemris_caller, self.icons,
                                     self.textEdit, self.update)
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.qXMLWidget)
        self.tab_arrtree.setLayout(layout)

        self.sensitivityMapsPlotter = QSensitivityMapsPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.sensitivityMapsPlotter)
        self.tab_sensmaps.setLayout(layout)

    def update(self):

        self.sensitivityMapsPlotter.update()

    def treeItemInsert(self):
        # Emergency solution
        self.qXMLWidget.treeItemInsert()

    def readXML(self):

        self.qXMLWidget.setInstance(self.Instance)
        self.sensitivityMapsPlotter.setInstance(self.Instance)
        self.qXMLWidget.readXML()


class JEMRIS_sim_sub(JEMRIS_sub, JSimForm):

    def __init__(self, jemris_caller, parent=None):

        def instance(fname):
            return Simulation(fname)

        module = dict()
        module['type'] = 'Simulation'
        module['instance'] = Simulation
        module['path'] = simulationPath()
        module['ext'] = 'xml'

        super(JEMRIS_sim_sub, self).__init__(module, parent)
        JSimClass.__init__(self)
        self.setupUi(self)

        self.jemris_caller = jemris_caller

        self.simulationOptions = QSimulationOptions()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.simulationOptions)
        self.frame.setLayout(layout)

        self.sim_start.clicked.connect(self.makeSim)

        """
        TIME SIGNAL
        """

        self.timeSignalPlotter = QTimeSignalPlotter()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.timeSignalPlotter)
        self.tab_time.setLayout(layout)

        # """
        # RECONSTRUCTOR
        # """

        self.reconstructionOptions = QReconstructionOptions()
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.reconstructionOptions)
        self.tab_reco.setLayout(layout)

    def readXML(self):

        self.simulationOptions.setInstance(self.Instance)
        self.timeSignalPlotter.setInstance(self.Instance)
        self.reconstructionOptions.setInstance(self.Instance)

        self.simulationOptions.readXML()

        self.update()

    def makeSim(self):

        self.jemris_caller.setOutput(self.sim_dump)

        mode = ['sequential',
                'parallel',
                'cluster'][self.sim_mode.currentIndex()]

        self.Instance.make(self.jemris_caller, mode, self.update)

    def update(self):

        self.Instance.readh5()
        self.reconstructionOptions.update()
        self.timeSignalPlotter.update()


class QSimulationOptions(QtWidgets.QWidget, QSimOptForm):

    def __init__(self, parent=None):

        super(QSimulationOptions, self).__init__(parent)
        QSimOptClass.__init__(self)
        self.setupUi(self)

        self.sequenceButton.clicked.connect(self.setSequence)
        self.phantomButton.clicked.connect(self.setSample)
        self.txArrayButton.clicked.connect(self.setTxArray)
        self.rxArrayButton.clicked.connect(self.setRxArray)
        self.r2TrajectoryButton.clicked.connect(self.setR2Trajectory)
        self.motionTrajectoryButton.clicked.connect(self.setMotionTrajectory)

        self.activeCirclesEdit.editingFinished.connect(self.setActiveCircles)

    def setInstance(self, Instance):

        self.Instance = Instance

    def setSequence(self):

        dialog = {'msg':     'Select the Sequence.xml file',
                  'path':    sequencePath(),
                  'ext':     "*.xml"}

        setFunction = self.Instance.setSequence
        getFunction = self.Instance.getSequence
        lineEdit = self.sequenceEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setSample(self):

        dialog = {'msg':     'Select the Phantom.h5 file',
                  'path':    phantomPath(),
                  'ext':     "*.h5"}

        setFunction = self.Instance.setSample
        getFunction = self.Instance.getSample
        lineEdit = self.phantomEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setTxArray(self):

        dialog = {'msg':     'Select the TxArray.xml file',
                  'path':    arrayPath(),
                  'ext':     "*.xml"}

        setFunction = self.Instance.setTxCoilArray
        getFunction = self.Instance.getTxCoilArray
        lineEdit = self.txArrayEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setRxArray(self):

        dialog = {'msg':     'Select the RxArray.xml file',
                  'path':    arrayPath(),
                  'ext':     "*.xml"}

        setFunction = self.Instance.setRxCoilArray
        getFunction = self.Instance.getRxCoilArray
        lineEdit = self.rxArrayEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setR2Trajectory(self):

        dialog = {'msg':     'Select the R2Trajectory.dat file',
                  'path':    simulationPath(),
                  'ext':     "*.dat"}

        setFunction = self.Instance.setR2Trajectory
        getFunction = self.Instance.getR2Trajectory
        lineEdit = self.r2TrajectoryEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setMotionTrajectory(self):

        dialog = {'msg':     'Select the MotionTrajectory.dat file',
                  'path':    simulationPath(),
                  'ext':     "*.dat"}

        setFunction = self.Instance.setMotionTrajectory
        getFunction = self.Instance.getMotionTrajectory
        lineEdit = self.motionTrajectoryEdit

        self.setSimulation(dialog, setFunction, getFunction, lineEdit)

    def setActiveCircles(self):

        self.Instance.setActiveCircles(self.activeCirclesEdit.text())

    def setSimulation(self, dialog, setFunction, getFunction, lineEdit):

        fname = str(QtWidgets.QFileDialog.getOpenFileName(
                self, dialog['msg'], dialog['path'], (dialog['ext']))[0])

        if not (fname == ''):
            setFunction(fname)
            lineEdit.setText(getFunction())

    def readXML(self):

        self.Instance.readXML()

        self.sequenceEdit.setText(self.Instance.getSequence())
        self.phantomEdit.setText(self.Instance.getSample())
        self.txArrayEdit.setText(self.Instance.getTxCoilArray())
        self.rxArrayEdit.setText(self.Instance.getRxCoilArray())

        if 'ActiveCircles' in self.Instance.sim['sample'].keys():
            self.activeCirclesEdit.setText(
                self.Instance.getActiveCircles())

        if 'R2Trajectory' in self.Instance.sim['sample'].keys():
            self.r2TrajectoryEdit.setText(
                self.Instance.getR2Trajectory())

        if 'MotionTrajectory' in self.Instance.sim['sample'].keys():
            self.motionTrajectoryEdit.setText(
                self.Instance.getMotionTrajectory())

        if 'Diffusionfile' in self.Instance.sim['sample'].keys():
            self.diffusionFileEdit.setText(
                self.Instance.getDiffusionFile())


class QReconstructionOptions(QtWidgets.QWidget, QRecOptForm):

    def __init__(self, parent=None):

        super(QReconstructionOptions, self).__init__(parent)
        QRecOptClass.__init__(self)
        self.setupUi(self)

        self.reconstructionButton.clicked.connect(self.doReco)
        self.reconstructionBox.activated[str].connect(self.selRecoPage)

        self.recoPlotter = QRecoPlotter()
        layout = QtWidgets.QGridLayout()
        layout.addWidget(self.recoPlotter)
        self.frame.setLayout(layout)

    def setInstance(self, Instance):

        self.Instance = Instance

    def selRecoPage(self):

        self.stackedWidget.setCurrentIndex(
            self.reconstructionBox.currentIndex())

    def doReco(self):

        reco = dict()

        text = self.reconstructionBox.currentText()

        if text == 'Cartesian Reconstruction':

            reco['type'] = 'cartesian'

        self.Reconstructor = Reconstructor(self.Instance, reco,
                                           self.recoFinished, self)

        self.Reconstructor.start()

    def recoFinished(self, I):

        self.recoPlotter.update(I)


class QXMLWidget(QtWidgets.QWidget, QXMLWidgetForm):

    def __init__(self, module, caller, icons,
                 textEdit, updateParent, parent=None):

        super(QXMLWidget, self).__init__(parent)
        QXMLWidgetClass.__init__(self)
        self.setupUi(self)

        self.caller = caller
        self.icons = icons
        self.textEdit = textEdit
        self.updateParent = updateParent

        self.updateButton.clicked.connect(self.updateXML)

        self.tableWidget = QTableWidget()
        self.treeWidget = QTreeWidget(self.tableWidget,
                                      self.icons,
                                      self.showHidden)

        layout = QtWidgets.QGridLayout()
        layout.addWidget(self.treeWidget, 0, 0)
        layout.addWidget(self.tableWidget, 1, 0)
        self.frame.setLayout(layout)

        if module['type'] in ['Sequence']:
            self.stackedWidget.setCurrentIndex(0)
        elif module['type'] in ['Array']:
            self.stackedWidget.setCurrentIndex(1)

    def setInstance(self, Instance):

        self.Instance = Instance

    def treeItemInsert(self):
        # Emergency solution
        self.treeWidget.treeItemInsert()

    def readXML(self):

        self.caller.setOutput(self.textEdit)

        self.Instance.readXML()
        self.treeWidget.update(self.Instance.xml)

        self.Instance.make(self.caller, self.updateGUI)

    def updateGUI(self):

        # Update GUI
        if self.Instance.status == 0:
            self.updateParent()
            self.updateButton.setStyleSheet("background-color: \
                qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, \
                stop:0 rgba(255, 0, 250, 0), stop:1 rgba(0, 150, 0, 255));")
        else:
            self.updateButton.setStyleSheet("background-color: \
                qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, \
                stop:0 rgba(255, 250, 0, 0), stop:1 rgba(150, 0, 0, 255));")

    def updateXML(self):

        Instance = self.Instance

        tmp = Instance.fname
        Instance.rename(os.path.join(workspace(), 'tmp.xml'))
        Instance.save()
        self.readXML()
        Instance.rename(tmp)


class QPreferences(QtWidgets.QDialog, QPreferencesForm):

    def __init__(self, parent=None):

        super(QPreferences, self).__init__(parent)
        QPreferencesClass.__init__(self)
        self.setupUi(self)

        self.parent = parent

        self.pushButtonApply.clicked.connect(self.save)
        self.pushButtonOK.clicked.connect(self.save)
        self.pushButtonOK.clicked.connect(self.close)
        self.pushButtonCancel.clicked.connect(self.close)
        self.pushButtonDefaults.clicked.connect(self.loadDefaults)

        self.toolButtonWorkspace.clicked.connect(self.setWorkspace)
        self.toolButtonJpath.clicked.connect(self.setJpath)
        self.toolButtonJshare.clicked.connect(self.setJshare)
        self.toolButtonMPIcall.clicked.connect(self.setMPIcall)

        for key in QtWidgets.QStyleFactory.keys():
            self.styleBox.addItem(key)

        self.styleBox.activated[str].connect(self.setQStyle)

        self.loadSettings()
        self.update()

    def loadSettings(self):

        self.load('settings.dat')

    def loadDefaults(self):

        self.load('defaults.dat')
        self.update()

    def load(self, fname):

        with open(os.path.join(settings_path, fname), 'r') as f:
            lines = f.readlines()

        self.settings = dict()

        for line in lines:
            if ' = ' in line:
                cStr = line.split(' = ')
                self.settings[cStr[0]] = cStr[1].split('\n')[0]

        self.applySettings()

        idx = self.styleBox.findText(self.settings['QStyle'])
        self.styleBox.setCurrentIndex(idx)

    def save(self):

        separator = '# ' + '-'*75 + ' #\n'

        keys = ['workspace', 'Jpath', 'Jshare', 'MPIcall', 'QStyle']

        with open(os.path.join(settings_path, 'settings.dat'), 'w') as f:
            f.write(separator)
            f.write('JEMRIS Settings File\n')
            f.write(separator)
            f.write('\n')
            for key in keys:
                f.write('%s = %s\n' % (key, self.settings[key]))

        self.applySettings()

    def applySettings(self):

        style = QtWidgets.QStyleFactory.create(self.settings['QStyle'])
        self.parent.setStyle(style)

    def update(self):

        self.lineEditWorkspace.setText(self.settings['workspace'])
        self.lineEditJpath.setText(self.settings['Jpath'])
        self.lineEditJshare.setText(self.settings['Jshare'])
        self.lineEditMPIcall.setText(self.settings['MPIcall'])

    def setQStyle(self):

        self.settings['QStyle'] = self.styleBox.currentText()

    def setParameter(self, key, dialog):

        _open = QtWidgets.QFileDialog.getExistingDirectory
        fname = str(_open(self, dialog, self.settings[key]))
        if not (fname == ''):
            self.settings[key] = fname
            self.update()

    def setWorkspace(self):

        self.setParameter('workspace', 'Set Workspace Directory')

        ws = self.settings['workspace']
        subdirs = ['sequences', 'phantoms', 'arrays', 'simulations']
        for subdir in subdirs:
            if not os.path.exists(os.path.join(ws, subdir)):
                os.makedirs(os.path.join(ws, subdir))

        self.checkWorkspace()

    def setJpath(self):

        self.setParameter('Jpath', 'Set JEMRIS Path Directory')

        self.checkJpath()

    def setJshare(self):

        self.setParameter('Jshare', 'Set JEMRIS Share Directory')

    def setMPIcall(self):

        self.setParameter('MPIcall', 'Set MPI Call Directory')

    def checkWorkspace(self):

        status = os.path.exists(self.settings['workspace'])

        if not status:

            self.statusWorkspace.setText('Setting incorrect.')

        else:

            self.statusWorkspace.setText('')

        return status

    def checkJpath(self):

        status = os.path.exists(os.path.join(self.settings['Jpath'], 'jemris'))

        if not status:

            self.statusJpath.setText('Setting incorrect.')

        else:

            self.statusJpath.setText('')

        return status


class QAbout(QtWidgets.QDialog, QAboutForm):

    def __init__(self, parent=None):

        super(QAbout, self).__init__(parent)
        QAboutClass.__init__(self)
        self.setupUi(self)
