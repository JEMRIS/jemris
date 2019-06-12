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
from PyQt5 import uic

path = os.path.split(__file__)[0]

JMainForm, JMainClass = \
    uic.loadUiType(os.path.join(path, 'jemris_main.ui'))

JSeqForm, JSeqClass = \
    uic.loadUiType(os.path.join(path, 'jemris_seq.ui'))

JPhaForm, JPhaClass = \
    uic.loadUiType(os.path.join(path, 'jemris_pha.ui'))

JSimForm, JSimClass = \
    uic.loadUiType(os.path.join(path, 'jemris_sim.ui'))

JArrForm, JArrClass = \
    uic.loadUiType(os.path.join(path, 'jemris_arr.ui'))

QSimOptForm, QSimOptClass = \
    uic.loadUiType(os.path.join(path, 'QSimulationOptions.ui'))

QRecOptForm, QRecOptClass = \
    uic.loadUiType(os.path.join(path, 'QReconstructionOptions.ui'))

QXMLWidgetForm, QXMLWidgetClass = \
    uic.loadUiType(os.path.join(path, 'QXMLWidget.ui'))

QPreferencesForm, QPreferencesClass = \
    uic.loadUiType(os.path.join(path, 'QPreferences.ui'))

QAboutForm, QAboutClass = \
    uic.loadUiType(os.path.join(path, 'QAbout.ui'))

QTPForm, QTPClass = \
    uic.loadUiType(os.path.join(path, 'QTimePlotter.ui'))

QVPForm, QVPClass = \
    uic.loadUiType(os.path.join(path, 'QVolumePlotter.ui'))
