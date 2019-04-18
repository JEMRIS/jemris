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
import xml.etree.ElementTree as ET
import lxml.etree as etree


def parseXML(fname):
    
    try:
        tree = ET.parse(fname)
    except:
        raise ValueError('Failed to read XML file %s.' %  os.path.split(fname)[1])

    return tree.getroot()


def writeXML(fname, root):

    try:
        ET.ElementTree(root).write(fname)
        # TODO: fully switch from xml to lxml
        etree.parse(fname).write(fname, pretty_print=True)

    except:
        raise ValueError('Failed to write XML file %s.' % os.path.split(fname)[1])

