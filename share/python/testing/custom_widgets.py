# -*- coding: utf-8 -*-
"""
Created on Sat Oct 17 13:55:31 2015

@author: user
"""

from PyQt4 import QtGui, QtDesigner
from mayavi_widgets import QVolumeSlicer

class QVolumeSlicerPlugin(QtDesigner.QPyDesignerCustomWidgetPlugin):

    def __init__(self, parent = None):

        QtDesigner.QPyDesignerCustomWidgetPlugin.__init__(self)

        self.initialized = False

    def initialize(self, core):

        if self.initialized:
            return

        self.initialized = True

    def isInitialized(self):

        return self.initialized

    def createWidget(self, parent):
        return QVolumeSlicer(parent)

    def name(self):
        return "QVolumeSlicer"

    def group(self):
        return "PyQt Examples"

#    def icon(self):
#        return QtGui.QIcon(_logo_pixmap)
#
#     _logo_pixmap = QtGui.QPixmap(_logo_16x16_xpm)

    def toolTip(self):
        return ""

    def whatsThis(self):
        return ""

    def isContainer(self):
        return False

    def domXml(self):
        return (
               '<widget class="QVolumeSlicer" name=\"volumeSlicer\">\n'
               " <property name=\"toolTip\" >\n"
               "  <string>The current time</string>\n"
               " </property>\n"
               " <property name=\"whatsThis\" >\n"
               "  <string>QVolumeSlicer "
               "slices volumes.</string>\n"
               " </property>\n"
               "</widget>\n"
               )

    def includeFile(self):
        return "mayavi_widgets"

