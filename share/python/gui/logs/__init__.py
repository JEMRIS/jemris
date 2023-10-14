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
import logging

path = os.path.split(__file__)[0]


" Name of Logger "
LOGNAME = 'jemris'
LOGFILE = 'jemris.log'


"""
Initialize logger. Specify name of logger, format, level, stream handlers,
file handlers, name of log file etc.

Inputs
------
**path**: Path of log file
"""

FORMAT = "%(levelname)s:%(funcName)s:%(message)s"
#    FORMAT = "%(levelname)s:%(name)s:%(message)s"

logger = logging.getLogger(LOGNAME)
logger.handlers = []        # Delete old handlers
logger.propagate = False    # Messages should NOT also go to root logger

# logger.setLevel(logging.DEBUG)
logger.setLevel(logging.DEBUG)

" Stream handler, writes to console "
sh = logging.StreamHandler()
sh.setFormatter(logging.Formatter(FORMAT))
logger.addHandler(sh)

" File handler, writes to file "
fh = logging.FileHandler(os.path.join(path, LOGFILE), mode='w')
fh.setFormatter(logging.Formatter(FORMAT))
logger.addHandler(fh)
