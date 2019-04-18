# -*- coding: utf-8 -*-
"""
Created on Sun Sep 27 19:01:53 2015

@author: user
"""

import numpy as np

#import matplotlib.pyplot as plt
#import matplotlib.cm as cm
#from mpl_toolkits.mplot3d import Axes3D   # must be imported for 3d vis
#
#n = 256
#
#golden_angle = np.pi * (3 - np.sqrt(5))
#theta = golden_angle * np.arange(n)
#z = np.linspace(1 - 1.0 / n, 1.0 / n - 1, n)
#radius = np.sqrt(1 - z * z)
#
#points = np.zeros((n, 3))
#points[:,0] = radius * np.cos(theta)
#points[:,1] = radius * np.sin(theta)
#points[:,2] = z
#
#idx = 32
#ax = plt.subplot(111, projection='3d')
#ax.plot(points[:,0], points[:,1], points[:,2], '.r')
#
#ax.hold(True)
#for i in range(idx):
#    ax.plot([0]+points[i,0], [0]+points[i,1], [0]+points[i,2], '.r')
#
#np.sum(points**2,axis=1)


G = np.array((1,0,0))
Gx = 1
Gy = 0
Gz = 0
#Grot = np.array((0,0,0))
#Grot = [0, 0, 0]

theta = float(np.radians(45))
alpha = float(np.radians(180))
phi = float(np.radians(45))

##theta = float(np.radians(np.random.randn(1)*90))
##alpha = float(np.radians(np.random.randn(1)*90))
##phi =   float(np.radians(np.random.randn(1)*90))
#
#cos_theta   = np.cos(theta)
#sin_theta   = np.sin(theta)
#cos_2_theta = cos_theta**2
#sin_2_theta = sin_theta**2
#cos_alpha   = np.cos(alpha)
#sin_alpha   = np.sin(alpha)
#cos_phi     = np.cos(phi)
#sin_phi     = np.sin(phi)
#
#
#Grot[0] = \
#    ((cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)* cos_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*sin_phi)*Gx+ \
#   (-(cos_phi*(cos_2_theta*cos_alpha+sin_2_theta)-sin_phi*cos_theta*sin_alpha)* sin_phi+(cos_phi*cos_theta*sin_alpha+sin_phi*cos_alpha)*cos_phi)*Gy+ \
#    	(cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+sin_phi*sin_theta*sin_alpha)*Gz;
#
#Grot[1] = \
#    ((-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)* cos_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*sin_phi)*Gx+ \
#   (-(-sin_phi*(cos_2_theta*cos_alpha+sin_2_theta)-cos_phi*cos_theta*sin_alpha)* sin_phi+(-sin_phi*cos_theta*sin_alpha+cos_phi*cos_alpha)*cos_phi)*Gy+ \
#    	(-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)+cos_phi*sin_theta*sin_alpha)*Gz;
#
#Grot[2] = (cos_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-sin_phi*sin_theta*sin_alpha)*Gx+ \
#		(-sin_phi*(-cos_theta*cos_alpha*sin_theta+sin_theta*cos_theta)-cos_phi*sin_theta*sin_alpha)*Gy+ \
#		(sin_2_theta*cos_alpha+cos_2_theta)*Gz;
#
#Grot = np.array(Grot).T
#
#
#
#A = np.cos(theta)**2*np.cos(alpha) + np.sin(theta)**2
#B = np.cos(theta) * np.sin(alpha)
#C = np.cos(alpha)
#D = -np.cos(theta)*np.cos(alpha)*np.sin(theta) + np.sin(theta)*np.cos(theta)
#E = np.sin(theta)*np.sin(alpha)
#F = np.sin(theta)**2*np.cos(alpha) * np.cos(theta)**2
#
#M = np.cos(phi) * A - np.sin(phi)*B
#N = np.cos(phi) * B + np.sin(phi) * C
#P = -np.sin(phi) * A - np.cos(phi)*B
#Q = -np.sin(phi) * B + np.cos(phi)*C
#
#R = np.cos(phi) * D + np.sin(phi) * E
#S = -np.sin(phi)*D + np.cos(phi) * E
#
#mtx1 = np.array(((M, N, R), (P, Q, S), (D, -E, F)))
#mtx2 = np.array(((np.cos(phi), -np.sin(phi), 0),
#                 (+np.sin(phi), np.cos(phi), 0),
#                 (0, 0, 1)))
#
#mtx3 = np.array(((A, B, D),
#                 (-B, C, E),
#                 (D, -E, F)))

mtx_phi = np.array(((np.cos(phi), -np.sin(phi), 0),
                 (+np.sin(phi), np.cos(phi), 0),
                 (0, 0, 1)))

mtx_theta = np.array(((np.sin(theta), 0, np.cos(theta)),
                      (np.cos(theta), 0, -np.sin(theta)),
                      (0, 1, 0)))

mtx_alpha = np.array(((1, 0, 0),
                      (0, np.cos(alpha), np.sin(alpha)),
                      (0, -np.sin(alpha), np.cos(alpha))))

Grot_new = np.dot(mtx_phi.T, np.dot(mtx_theta.T, np.dot(mtx_alpha, np.dot(mtx_theta, np.dot(mtx_phi, G)))))

#Grot_new = np.dot(mtx2.T, np.dot(mtx3, np.dot(mtx2, G)))

#Grot_new = np.dot(mtx1, np.dot(mtx2, G))

print Grot_new