# -*- coding: utf-8 -*-
"""
Created on Thu Sep 17 11:42:39 2015

@author: user
"""

import numpy as np
from mayavi import mlab
from tvtk.tools import visual
from tvtk.api import tvtk

f = open('/home/user/Downloads/JEMRIS_kt_blast/data/traj_smooth.dat')

c = f.readlines()

traj = list()

for line in c:

    out = line.split("  ")
    traj.append(np.array([float(out[i]) for i in range(1,len(out))]))

init = np.array(traj[0])
traj = np.array(traj[1:])

time = traj[:, 0]
displacement = traj[:, 1:4]/10
rotation = traj[:, 4:]

# Create a figure
f = mlab.figure(size=(500,500))
# Tell visual to use this as the viewer.
visual.set_viewer(f)

#a = mlab.test_plot3d()

# Even sillier animation.
b1 = visual.box()

#mov = np.random.randn(1000, 3)*0.01
#tim = np.arange(0, 1000, 1)

axis = np.array((0, 0, 1))

#sg = tvtk.ImageData(spacing=(1, 1, 1), origin=(0, 0, 0))
#d = mlab.pipeline.add_dataset(sg)
#gx = mlab.pipeline.grid_plane(d)
#gy = mlab.pipeline.grid_plane(d)
#gy.grid_plane.axis = 'y'
#gz = mlab.pipeline.grid_plane(d)
#gz.grid_plane.axis = 'z'
#
#iso = mlab.pipeline.iso_surface(d)
#iso.contour.maximum_contour = 75.0

@mlab.show
@mlab.animate(delay=250)
def anim():
    """Animate the b1 box."""
    for idx, t in enumerate(time):
        print t
        b1.x = displacement[idx, 0]
        b1.y = displacement[idx, 1]
        b1.z = displacement[idx, 2]

#        b1.rotate(rotation[idx, 0])

        yield

# Run the animation.
anim()

