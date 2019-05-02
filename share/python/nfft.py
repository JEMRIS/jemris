"""
Non-uniform Fast Fourier Transform

Markus Zimmermann
"""

from __future__ import division

import numpy as np

#from pynfft import NFFT, Solver

class nufft(object):

    """
    Non-uniform Fourier Transform
    """

    def __init__(self, N, K):

        self.nfft = NFFT(N, K[0].size)
        self.nfft.x = np.array(K).T
        self.nfft.precompute()

        self.infft = Solver(self.nfft)

    def fft(self, data):

        M = self.nfft.M
        N = self.nfft.N
        shape = data.shape[len(N):]
        I = int(np.prod(shape))

        data = data.reshape(N + (I,))

        output = np.zeros((M, I), dtype=complex)

        for i in range(I):

            self.nfft.f_hat = data[..., i]
            output[..., i] = self.nfft.trafo()

        output = output.reshape((M,) + tuple(shape))

        return output

    def ifft(self, data, flag=False):

        M = self.nfft.M
        N = self.nfft.N
        shape = data.shape[1:]
        I = int(np.prod(shape))

        data = data.reshape(M, I)

        output = np.zeros(N + (I,), dtype=complex)
        for i in range(I):

            if flag:
                self.infft.y = data[:, i]
                self.infft.before_loop()

                niter = 10  # set number of iterations to 10
                for iiter in range(niter):
                    self.infft.loop_one_step()
                output[..., i] = self.infft.f_hat_iter

#                threshold = 1e-3
#                while True:
#                    self.infft.loop_one_step()
#                    if(np.all(self.infft.r_iter < threshold)):
#                        continue

            else:

                self.nfft.f = data[:, i]
                output[..., i] = self.nfft.adjoint()

        output = output.reshape(N + tuple(shape))

        return output
