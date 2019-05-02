# -*- coding: utf-8 -*-
"""
Created on Wed Sep  9 21:08:02 2015

@author: user
"""


import matplotlib.pyplot as plt
import numpy as np

im = np.random.randn(100,100)

fig = plt.figure()

ax = fig.add_subplot(111)
ax.imshow(im)

ax.set_xticklabels(ax.get_xticks()*2)


f = open('/home/user/Downloads/JEMRIS_kt_blast/data/traj_step.dat', 'r')