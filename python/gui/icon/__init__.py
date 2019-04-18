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
import scipy.io as io

import matplotlib.pyplot as plt
from PIL import Image

from PyQt5 import QtGui, QtCore

path = os.path.split(__file__)[0]


def ModuleIcons():

    # Create dict with bitmaps
    icons = dict()

    for f in os.listdir(path):
        if f.endswith('.png'):
            icon = QtGui.QIcon(os.path.join(path, f))
            pixmap = icon.pixmap(icon.actualSize(QtCore.QSize(28, 28)))
            icons[f.strip('.png')] = pixmap

    return icons


def Mat2Png():

    """
    Load Module Icons from ModuleIcons.mat and save them as *.png
    """

    fig = plt.figure(frameon=False)
    Icons = io.loadmat(os.path.join('ui/ModuleIcons.mat'))
    for key in Icons.keys():
        if key not in ['__globals__', '__header__', '__version__']:
            im = Icons[key]
            im[~np.isfinite(im)] = 1

            ax = plt.Axes(fig, [0., 0., 1., 1.])
            ax.set_axis_off()
            fig.add_axes(ax)
            ax.imshow(im, aspect='normal')
            fname = 'ui/icons/%s.png' % key
            fig.savefig(fname)

            threshold = 100
            dist = 5
            img = Image.open(fname).convert('RGBA')
            # np.asarray(img) is read only.
            # Wrap it in np.array to make it modifiable.
            arr = np.array(np.asarray(img))
            r, g, b, a = np.rollaxis(arr, axis=-1)
            mask = ((r > threshold) &
                    (g > threshold) &
                    (b > threshold) &
                    (np.abs(r-g) < dist) &
                    (np.abs(r-b) < dist) &
                    (np.abs(g-b) < dist))
            arr[mask, 3] = 0
            img = Image.fromarray(arr, mode='RGBA')
            img.save(fname)
