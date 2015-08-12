/*
 * \file dft.c
 * \brief
 *    A target independent Discrete Fourier Transform implementation
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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

/*!
 * \brief
 *    Calculate the double precision complex DFT using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   x     Pointer to size n time domain complex array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void dft_c (complex_d_t *x, complex_d_t *X, uint32_t n)
{
   uint32_t i, j;
   double th, _2pi_n, _2pii_n;
   complex_d_t w;

   _2pi_n = 2*M_PI/n;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      X[i] = 0;               // empty acc
      for (j=0 ; j<n ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) - I*sin(th);
         X[i] += w * x[j];
      }
   }
}

/*!
 * \brief
 *    Calculate the single precision complex DFT using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   x     Pointer to size n time domain complex array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void dft_cf (complex_f_t *x, complex_f_t *X, uint32_t n)
{
   uint32_t i, j;
   float th, _2pi_n, _2pii_n;
   complex_f_t w;

   _2pi_n = 2*M_PI/n;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      X[i] = 0;               // empty acc
      for (j=0 ; j<n ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) - I*sin(th);
         X[i] += w * x[j];
      }
   }
}

/*!
 * \brief
 *    Calculate the double precision DFT for real signals using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   x     Pointer to size n time domain real array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void dft_r (double *x, complex_d_t *X, uint32_t n)
{
   uint32_t i, j, n_2;
   double th, _2pi_n, _2pii_n;
   complex_d_t w;

   _2pi_n = 2*M_PI/n;
   n_2 = n>>1;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      X[i] = 0;               // empty acc
      for (j=0 ; j<=n_2 ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) - I*sin(th);
         X[i] += w * x[j];
      }
   }
   /*
    *  For real DFT the Frequency spectra is symmetrical.
    *  Hence we can copy the negative frequencies from positives
    */
   for (i=n_2+1 ; i<n ; ++i)
      X[i] = conj (X[n-i]);
}

/*!
 * \brief
 *    Calculate the single precision DFT for real signals using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   x     Pointer to size n time domain real array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void dft_rf (float *x, complex_f_t *X, uint32_t n)
{
   uint32_t i, j, n_2;
   float th, _2pi_n, _2pii_n;
   complex_f_t w;

   _2pi_n = 2*M_PI/n;
   n_2 = n>>1;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      X[i] = 0;               // empty acc
      for (j=0 ; j<=n_2 ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) - I*sin(th);
         X[i] += w * x[j];
      }
   }
   /*
    *  For real DFT the Frequency spectra is symmetrical.
    *  Hence we can copy the negative frequencies from positives
    */
   for (i=n_2+1 ; i<n ; ++i)
      X[i] = conjf (X[n-i]);
}

/*!
 * \brief
 *    Calculate the double precision inverse DFT using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size n time domain complex array
 * \param   n     Number of points
 * \return        None
 */
void idft_c (complex_d_t *X, complex_d_t *x, uint32_t n)
{
   uint32_t i, j;
   double th, _2pi_n, _2pii_n, _1_n;
   complex_d_t w;

   _2pi_n = 2*M_PI/n;
   _1_n = 1./n;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      x[i] = 0;               // empty acc
      for (j=0 ; j<n ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) + I*sin(th);
         x[i] += w * X[j];
      }
      x[i] *= _1_n;           // scale output
   }
}

/*!
 * \brief
 *    Calculate the single precision inverse DFT using a
 *    not in-place DFT matrix algorithm.
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size n time domain complex array
 * \param   n     Number of points
 * \return        None
 */
void idft_cf (complex_f_t *X, complex_f_t *x, uint32_t n)
{
   uint32_t i, j;
   float th, _2pi_n, _2pii_n, _1_n;
   complex_f_t w;

   _2pi_n = 2*M_PI/n;
   _1_n = 1./n;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      x[i] = 0;               // empty acc
      for (j=0 ; j<n ; ++j) {
         th = _2pii_n * j;    // calculate Omega * j
         w = cos(th) + I*sin(th);
         x[i] += w * X[j];
      }
      x[i] *= _1_n;           // scale output
   }
}

/*!
 * brief
 *    inverse dft contribution loop body
 */
#define _idft_r_loop(_j, _div)   \
{                                \
   th = _2pii_n * _j;            \
   w = cos(th) + I*sin(th);      \
   x[i] += w * (X[_j] * _div);   \
}

/*!
 * \brief
 *    Calculate the double precision inverse DFT for real signals
 *    using a not in-place DFT matrix algorithm.
 * \note
 *    Due to symmetrical property of real DFT the algorithm doesn't use the
 *    negative frequencies of frequency spectra. So the frequency array can
 *    also be smaller.
 *
 * \param   X     Pointer to size n or n/2+1 frequency domain complex array
 * \param   x     Pointer to size n time domain real array
 * \param   n     Number of points
 * \return        None
 */
void idft_r (complex_d_t *X, double *x, uint32_t n)
{
   uint32_t i, j, n_2;
   double th, _2pi_n, _2pii_n, _1_n, _2_n;
   complex_d_t w;

   _2pi_n = 2*M_PI/n;
   _1_n = 1./n;
   _2_n = 2./n;
   n_2 = n>>1;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      x[i] = 0;               // empty acc
      _idft_r_loop(0, _1_n);
      for (j=1 ; j<n_2 ; ++j)
         _idft_r_loop(j, _2_n);
      _idft_r_loop(n_2, _1_n);
   }
}

/*!
 * \brief
 *    Calculate the single precision inverse DFT for real signals
 *    using a not in-place DFT matrix algorithm.
 * \note
 *    Due to symmetrical property of real DFT the algorithm doesn't use the
 *    negative frequencies of frequency spectra. So the frequency array can
 *    also be smaller.
 *
 * \param   X     Pointer to size n or n/2+1 frequency domain complex array
 * \param   x     Pointer to size n time domain real array
 * \param   n     Number of points
 * \return        None
 */
void idft_rf (complex_f_t *X, float *x, uint32_t n)
{
   uint32_t i, j, n_2;
   float th, _2pi_n, _2pii_n, _1_n, _2_n;
   complex_f_t w;

   _2pi_n = 2*M_PI/n;
   _1_n = 1./n;
   _2_n = 2./n;
   n_2 = n>>1;
   for (i=0 ; i<n ; ++i) {
      _2pii_n = _2pi_n * i;   // calculate omega
      x[i] = 0;               // empty acc
      _idft_r_loop(0, _1_n);
      for (j=1 ; j<n_2 ; ++j)
         _idft_r_loop(j, _2_n);
      _idft_r_loop(n_2, _1_n);
   }
}

