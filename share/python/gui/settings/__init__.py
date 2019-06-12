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

path = os.path.split(__file__)[0]


def getSettings():

    with open(os.path.join(path, 'settings.dat'), 'r') as f:
        lines = f.readlines()

    settings = dict()

    for line in lines:
        if ' = ' in line:
            cStr = line.split(' = ')
            settings[cStr[0]] = cStr[1].split('\n')[0]

    return settings


def workspace():

    settings = getSettings()

    return settings['workspace']


def sequencePath():

    settings = getSettings()

    return os.path.join(settings['workspace'], 'sequences')


def phantomPath():

    settings = getSettings()

    return os.path.join(settings['workspace'], 'phantoms')


def arrayPath():

    settings = getSettings()

    return os.path.join(settings['workspace'], 'arrays')


def simulationPath():

    settings = getSettings()

    return os.path.join(settings['workspace'], 'simulations')
