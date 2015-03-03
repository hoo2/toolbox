/*
 * \file dft.c
 * \brief
 *    A target independent Discrete Fourier Transform functionality
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <dsp/dft.h>

void rdft_i (int *re, int *im, int *x, int n)
{
   int N=n, k, K=n/2+1;
   float wn, _2pik_N, _2pi_N = 2*M_PI/N;

   for (k=0 ; k<K ; ++k) {
      _2pik_N = _2pi_N * k;   // calculate omega
      re[k] = im[k] = 0;      // empty acc
      for (n=0 ; n<N ; ++n) {
         wn = _2pik_N * n;    // calculate Omega * n
         re[k] += x[n] * qcos (wn);
         im[k] -= x[n] * qsin (wn);
      }
   }
}

void rdft_f (float *re, float *im, float *x, int n)
{
   int N=n, k, K=n/2+1;
   float wn, _2pik_N, _2pi_N = 2*M_PI/N;

   for (k=0 ; k<K ; ++k) {
      _2pik_N = _2pi_N * k;   // calculate omega
      re[k] = im[k] = 0;      // empty acc
      for (n=0 ; n<N ; ++n) {
         wn = _2pik_N * n;    // calculate Omega * n
         re[k] += x[n] * qcos (wn);
         im[k] -= x[n] * qsin (wn);
      }
   }
}
void rdft_d (double *re, double *im, double *x, int n);
