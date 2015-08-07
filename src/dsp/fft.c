/*
 * \file fft.c
 * \brief
 *    A target independent Fast Fourier Transfer implementation
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
#include <dsp/fft.h>
static uint32_t _log2 (int32_t n);
static void _bit_reverse_f (float * x, unsigned long n);
static void _bit_reverse_d (double *x, double *r, uint32_t n);
static void _bit_reverse_c (_Complex double *x, _Complex double *r, uint32_t n);



/*
 * \brief
 *    Calculate the Log2N
 * \note    N has to be power of 2
 */
static uint32_t _log2 (int32_t n)
{
   uint32_t r=1;

   while (n>1) {
      n >>= 1;
      ++r;
   }
   return r-1;
}


static void _bit_reverse_f (float * x, unsigned long n);
static void _bit_reverse_d (double *x, double *r, uint32_t n)
{
   double tmp;
   long i, j, k, n_2;

   n_2 = n>>1;
   for (i=1, j=n_2 ; i<n-1 ; ++i) {
      if (i<=j) {
         /* complex exchange */
         tmp = x[i];
         r[i] = x[j] ;
         r[j] = tmp ;
      }
      for (k=n_2 ; k<=j ; k>>=1) {
         j = j-k;
      }
      j = j+k;
   }
   // Add the extra common nodes
   r[0] = x[0];
   r[n-1] = x[n-1];
}

static void _bit_reverse_c (_Complex double *x, _Complex double *r, uint32_t n)
{
   _Complex double tmp;
   long i, j, k, n_2;

   n_2 = n>>1;
   for (i=1, j=n_2 ; i<n-1 ; ++i) {
      if (i<=j) {
         /* complex exchange */
         tmp = x[i];
         r[i] = x[j] ;
         r[j] = tmp ;
      }
      for (k=n_2 ; k<=j ; k>>=1) {
         j = j-k;
      }
      j = j+k;
   }
   // Add the extra common nodes
   r[0] = x[0];
   r[n-1] = x[n-1];
}


//-----------------------------------------------------------------------------
// name: rfft()
// desc: real value fft
//
//   these routines from the CARL software, spect.c
//   check out the CARL CMusic distribution for more source code
//
//   if forward is true, rfft replaces 2*N real data points in x with N complex
//   values representing the positive frequency half of their Fourier spectrum,
//   with x[1] replaced with the real part of the Nyquist frequency value.
//
//   if forward is false, rfft expects x to contain a positive frequency
//   spectrum arranged as before, and replaces it with 2*N real values.
//
//   N MUST be a power of 2.
//
//-----------------------------------------------------------------------------
void rfft_f (float *x, float *X, uint32_t n, uint32_t fwd)
{

}

void rfft_d (double *x, double *X, uint32_t n, uint32_t fwd)
{
   _bit_reverse_d (x, X, n);
}

#define _real(_z, _i)   ((_z)[2*(_i)])
#define _img(_z, _i)    ((_z)[2*(_i)+1])

void cfft_r (double *x, double *X, uint32_t n, uint32_t fwd)
{

}


void cfft_c (_Complex double *x, _Complex double *X, uint32_t n, uint32_t fwd)
{
   uint32_t i, j, to;    // Loop counters

   _bit_reverse_c (x, X, n);

}






