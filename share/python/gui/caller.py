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
import platform
import subprocess
import multiprocessing

import copy
import xml.etree.ElementTree as ET

from PyQt5 import QtGui, QtWidgets

from gui.parser import parseXML, writeXML
from gui.logs import logger
from gui.settings import getSettings, workspace
from gui.instances import Sequence, Array, Simulation
from gui.widgets import QProcess, QProgressBar


class caller(object):

    def __init__(self):

        self.workers = multiprocessing.cpu_count()

        self.edit = None
        self.statusbar = None
        self.multiThreaded = True
        self.processes = list()

    def connect(self):

        settings = getSettings()

        self.win = (platform.system() == 'Windows')

        if self.win:
            " Windows "
            if platform.architecture()[0] == '64bit':
                pass  # TODO: port from MATLAB to Python
                # handles.JemrisPath=['"',winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Wow6432Node\Research Centre Juelich\jemris', 'Path')];
            else:
                pass  # TODO: port from MATLAB to Python
#                handles.JemrisPath=['"',winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Research Centre Juelich\jemris', 'Path')];
            self.Jshare = ['']
            self.Jcall = os.path.join(self.Jshare, 'jemris.exe"')
            # TODO: PJcall
        else:
            " OS X and Linux "
            self.Jpath = settings['Jpath']
            self.Jshare = settings['Jshare']

            self.Jcall = os.path.join(self.Jpath, 'jemris')
            self.PJcall = os.path.join(self.Jpath, 'pjemris')
            self.MPIcall = '%s -np %s' % (settings['MPIcall'], '%s')

            " Set libpath "
            self.shell = os.environ['SHELL']
            if self.shell == '/bin/bash':               # a BASH
                self.libpath = 'LD_LIBRARY_PATH="";'
            else:                                       # a TCSH
                self.libpath = 'setenv LD_LIBRARY_PATH "";'

        # Get connected JEMRIS version.
        cmd = self.Jcall
        proc = subprocess.Popen('%s %s' % (self.libpath, cmd),
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                shell=True)

        out, err = proc.communicate()
        
        out = out.decode(encoding='UTF-8')
        err = err.decode(encoding='UTF-8')
        
        status = proc.returncode

        logger.debug('Output: %s' % out)
        logger.debug('Error: %s' % err)
        logger.debug('Status: %s' % status)

        self.version = out.split('\n')[1]

    def setOutput(self, edit):

        self.edit = edit

    def setStatusBar(self, statusbar):

        self.statusbar = statusbar

    def setMultiThreaded(self, flag):

        self.multiThreaded = flag

    def loadmod(self):

        cmd = '%s modlist ; mv mod.xml %s' % (self.Jcall, workspace())

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, shell=True)
        out, err = proc.communicate()

        fname = os.path.join(workspace(), 'mod.xml')

        mod(fname)

        return fname

    def make(self, instance, fcn=None, mode='sequential'):

        fname = '%s.xml' % instance.fname           # Full File Name
        iname = os.path.split(instance.fname)[1]    # Instance Name

        if isinstance(instance, Sequence):

            containerFile = ['seq.h5', '%s.h5']
            outputFile = ['.seq.out', '%s.seq.out']
            progressRange = [0, 0]

            cmd = '%s %s' % (self.Jcall, fname)

        if isinstance(instance, Array):

            containerFile = ['sensmaps.h5', '%s.h5']
            outputFile = ['.arr.out', '%s.arr.out']
            progressRange = [0, 0]

            cmd = '%s %s' % (self.Jcall, fname)

        if isinstance(instance, Simulation):

            containerFile = ['signals.h5', '%s.h5']
            outputFile = ['.sim.out', '%s.sim.out']
            progressRange = [0, 100]

            if mode in ['sequential']:
                cmd = '%s %s' % (self.Jcall, fname)
            elif mode in ['parallel']:
                cmd = '%s %s %s' % (self.MPIcall % self.workers,
                                    self.PJcall, fname)
            elif mode in ['cluster']:
                pass    # TODO

        def updateFunction(output):

            if isinstance(instance, Simulation):

                with open('.jemris_progress.out', 'r') as f:
                    progress = int(f.readline())

                if self.statusbar is not None:
                    progressbar.setValue(progress)
                    progressbar.setText('%s - %s%%' % (iname, progress))

            with open(outputFile[0], 'a') as f:
                f.write(output)

            if edit is not None:
                edit.append(output)

        def exitFunction():

            if self.statusbar is not None:
                self.statusbar.removeWidget(frame)

            if self.multiThreaded:
                instance.status = process.exitStatus()
                process.deleteLater()
                self.processes.remove(process)

            os.rename(outputFile[0], outputFile[1] % instance.fname)
            if instance.status == 0:
                os.rename(containerFile[0], containerFile[1] % instance.fname)

            return fcn()

        if self.multiThreaded:

            process = QProcess()
            process.setUpdateFunction(updateFunction)
            process.setExitFunction(exitFunction)

            self.processes.append(process)

        if self.statusbar is not None:

            progressbar = QProgressBar()
            progressbar.setText(iname)

            pushbutton = QtWidgets.QPushButton('x')
            pushbutton.setMaximumWidth(22)
            pushbutton.clicked.connect(process.kill)

            layout = QtWidgets.QGridLayout()
            layout.addWidget(progressbar, 0, 0)
            layout.addWidget(pushbutton, 0, 1)
            # TODO
#            layout.setMargin(0)
            layout.setSpacing(0)
            frame = QtWidgets.QFrame()
            frame.setLayout(layout)
            frame.setFrameShadow(frame.Sunken)
            frame.setFrameShape(frame.Box)
            frame.setMaximumHeight(22)

            progressbar.setMinimum(progressRange[0])
            progressbar.setMaximum(progressRange[1])

            self.statusbar.addWidget(frame)

        edit = self.edit

        if self.multiThreaded:

            process.start(self.libpath)
            process.waitForFinished()
            process.start(cmd)

            return

        else:

            proc = subprocess.Popen('%s %s' % (self.libpath, cmd),
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, shell=True)

            out, err = proc.communicate()
            
            out = out.decode(encoding='UTF-8')
            err = err.decode(encoding='UTF-8')
            
            instance.status = proc.returncode

            updateFunction(out)
            exitFunction()

            return out, err


def mod(fname):

    """
    Modifies mod.xml file to incorporate coil array defaults.
    """

    coils = ET.Element('COILS')

    base = dict()

    base['Name'] = ''
    base['XPos'] = ''
    base['YPos'] = ''
    base['ZPos'] = ''

    base['Azimuth'] = ''
    base['Polar'] = ''
    base['Scale'] = ''
    base['Phase'] = ''

    base['Conj'] = ''
    base['Dim'] = ''
    base['Extent'] = ''
    base['Points'] = ''

    biotsavartloop = copy.copy(base)
    biotsavartloop['Name'] = 'BIOTSAVARTLOOP'
    biotsavartloop['Mask'] = ''
    biotsavartloop['Radius'] = ''

    analyticcoil = copy.copy(base)
    analyticcoil['Name'] = 'ANALYTICCOIL'
    analyticcoil['Sensitivity'] = ''

    externalcoil = copy.copy(base)
    externalcoil['Name'] = 'EXTERNALCOIL'
    externalcoil['Filename'] = ''

    coils.append(ET.Element('BIOTSAVARTLOOP', attrib=biotsavartloop))
    coils.append(ET.Element('ANALYTICCOIL', attrib=analyticcoil))
    coils.append(ET.Element('EXTERNALCOIL', attrib=externalcoil))

    root = parseXML(fname)

    root.append(coils)

    writeXML(fname, root)
