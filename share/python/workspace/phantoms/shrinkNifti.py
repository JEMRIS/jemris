# -*- coding: utf-8 -*-
"""
Created on Thu Sep  3 17:34:19 2015

@author: user
"""

import nibabel as nib
import numpy as np

fname = 'MNIbrain_hires.nii'
fname_new = 'phantom_new.nii'
dtype = np.uint8

nii = nib.load(fname)
nii.set_data_dtype(dtype)
nib.save(nii, fname_new)
