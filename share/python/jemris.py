"""
JEMRIS Copyright (C)
                      2006-2025  Tony Stoecker
                      2007-2019  Kaveh Vahedipour
                      2009-2019  Daniel Pflugfelder
                      2018-2019  Markus Zimmermann
                      2018-2019  Dominik Ridder


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

import sys
from PyQt5 import QtWidgets

from gui.jemris_main import JEMRIS

"""
Call GUI
"""

app = QtWidgets.QApplication.instance()
if app is None:
    app = QtWidgets.QApplication(sys.argv)

myWindow = JEMRIS(None)
myWindow.show()
app.exec_()
