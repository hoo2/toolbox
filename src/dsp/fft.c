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

/*
 * Static functions
 */
static uint32_t _log2 (int32_t n);
static uint32_t _pow2 (uint32_t e);
//static void _bit_reverse_f (float * x, unsigned long n);
static void _bit_reverse_d (double *x, double *r, uint32_t n);
static void _bit_reverse_c (_Complex double *x, _Complex double *r, uint32_t n);

void _fft_loop_pcd (double *X, uint32_t n, uint32_t l);

/*
 * Static toolbox
 */

#define _real(_z, _i)   ((_z)[2*(_i)])
#define _imag(_z, _i)    ((_z)[2*(_i)+1])

/*
 * \brief
 *    Calculate the Log2N
 * \note    N has to be power of 2
 */
static uint32_t _log2 (int32_t n) {
   uint32_t r=1;

   while (n>1) {
      n >>= 1;
      ++r;
   }
   return r-1;
}

static uint32_t _pow2 (uint32_t e) {
   uint32_t r = 1;

   if (e == 0)
      return 1;
   else {
      for ( ; e>0 ; --e)
         r <<= 1;
      return r;
   }
}

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
      for (k=n_2 ; k<=j ; k>>=1)
         j = j-k;
      j = j+k;
   }
   // Add the extra common nodes
   r[0] = x[0];
   r[n-1] = x[n-1];
}

static void _bit_reverse_pcd (double *x, double *r, uint32_t n)
{
   double tr, ti;
   uint32_t i, j, k, n_2;

   n_2 = n>>1;
   for (i=1, j=n_2 ; i<n-1 ; ++i) {
      if (i<=j) {
         /* complex exchange */
         tr = _real (x,i);
         ti = _imag (x,i);
         _real (r,i) = _real (x,j);
         _imag (r,i) = _imag (x,j);
         _real (r,j) = tr ;
         _imag (r,j) = ti;
      }
      for (k=n_2 ; k<=j ; k>>=1)
         j = j-k;
      j = j+k;
   }
   // Add the extra common nodes
   _real (r,0) = _real (x,0);
   _imag (r,0) = _imag (x,0);
   _real (r,n-1) = _real (x,n-1);
   _imag (r,n-1) = _imag (x,n-1);
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
      for (k=n_2 ; k<=j ; k>>=1)
         j = j-k;
      j = j+k;
   }
   // Add the extra common nodes
   r[0] = x[0];
   r[n-1] = x[n-1];
}


void _fft_loop_pcd (double *X, uint32_t n, uint32_t l)
{
   uint32_t i, j;    // Loop counters
   uint32_t k, le, le_2;
   double tr, ti, wr, wi, sr, si, th;

   wr = 1;
   wi = 0;
   le = _pow2 (l);
   le_2 = le>>1;
   th = M_PI/le_2;
   sr = cos (th);
   si = -sin (th);
   // Loop each sub-DFT
   for (j=1 ; j<=le_2 ; ++j) {
      // Loop each Butterfly
      for (i=j-1 ; i<n-1 ; i+=le) {
         k = i+le_2;
         tr = _real(X,k)*wr - _imag(X,k)*wi;
         ti = _real(X,k)*wi + _imag(X,k)*wr;
         _real(X,k) = _real(X,i) - tr;
         _imag(X,k) = _imag(X,i) - ti;
         _real(X,i) += tr;
         _imag(X,i) += ti;
      }
      tr = wr;
      wr = tr*sr - wi*si;
      wi = tr*si + wi*sr;
   }

}

void rfft_pcd (double *x, double *X, uint32_t n, uint32_t fwd)
{
   uint32_t i;    // Loop counters
   uint32_t n_2, n_4, _3n_4, im, ip2, ipm;

   // Calculate helpers
   n_2 = n>>1;
   n_4 = n_2>>1;
   _3n_4 = 3*n_4;

   // Separate even and odd points from x to X
   for (i=0; i<n_2 ; ++i) {
      _real(X,i) = _real(x,2*i);
      _imag(X,i) = _real(x, 2*i+1);
   }

   // Do the FFT to X
   cfft_pcd (X, X, n_2, 1);

   // Even/odd frequency domain decomposition
   for (i=1 ; i<n_4 ; ++i) {
      im = n_2 - i;
      ip2 = n_2 + i;
      ipm = n_2 + im;
      _real(X, ip2) = (_imag(X, i) + _imag(X, im)) / 2;
      _real(X, ipm) = _real(X, ip2);
      _imag(X, ip2) = -(_real(X, i) - _real(X, im)) / 2;
      _imag(X, ipm) = -_imag(X, ip2);
      _real(X, i)   = (_real(X, i) + _real(X, im)) / 2;
      _real(X, im)  = _real(X, i);
      _imag(X, i)   = (_imag(X, i) - _imag(X, im)) / 2;
      _imag(X, im)  = -_imag(X, i);
   }
   _real(X, _3n_4) = _imag(X, n_4);
   _real(X, n_2) = _imag(X, 0);
   _imag(X, 0) = _imag(X, n_4) = _imag(X, n_2) = _imag(X, _3n_4) = 0;

   // Do the last stage of fft
   _fft_loop_pcd (X, n, _log2(n));

   // In inverse fft
}

/*
 * \brief
 *    If fwd is true, calculate the complex FFT using an in-place decimation
 *    in time algorithm.
 *    User can use the same or separate arrays to store input and output.
 *    The arrays must have size 2*n. The inputs and outputs are packed arrays
 *    of floating point numbers. In a packed array the real and imaginary parts
 *    of each complex number are placed in alternate neighboring elements.
 *    For ex:
 *       x[0] = Re(z[0])
 *       x[1] = Im(z[0])
 *       x[2] = Re(z[1])
 *       x[3] = Im(z[1])
 *    If fwd is false, calculate the inverse complex FFT
 *
 * \param   x     Pointer to input complex packed array
 * \param   X     Pointer to output complex packed array
 * \param   n     Number of complex numbers, half the size of
 *                input and output arrays
 * \param   fwd   Select the forward or inverse FFT
 *    \arg  0     Inverse FFT
 *    \arg  1     Forward FFT
 * \return        None
 */
void cfft_pcf (float *x, float *X, uint32_t n, uint32_t fwd)
{

}

/*
 * \brief
 *    If fwd is true, calculate the complex FFT using an in-place decimation
 *    in time algorithm.
 *    User can use the same or separate arrays to store input and output.
 *    The arrays must have size 2*n. The inputs and outputs are packed arrays
 *    of floating point numbers. In a packed array the real and imaginary parts
 *    of each complex number are placed in alternate neighboring elements.
 *    For ex:
 *       x[0] = Re(z[0])
 *       x[1] = Im(z[0])
 *       x[2] = Re(z[1])
 *       x[3] = Im(z[1])
 *    If fwd is false, calculate the inverse complex FFT
 *
 * \param   x     Pointer to input complex packed array
 * \param   X     Pointer to output complex packed array
 * \param   n     Number of complex numbers, half the size of
 *                input and output arrays
 * \param   fwd   Select the forward or inverse FFT
 *    \arg  0     Inverse FFT
 *    \arg  1     Forward FFT
 * \return        None
 */
void cfft_pcd (double *x, double *X, uint32_t n, uint32_t fwd)
{
   uint32_t i, l, m;

   // Bit reversal
   _bit_reverse_pcd (x, X, n);

   // In inverse FFT perform a sign change to Im part
   for (i=0 ; !fwd && i<n ; ++i)
      _imag (X,i) = -_imag (X,i);

   // Loop for each stage
   m = _log2(n);
   for (l=1 ; l<=m ; ++l)
      _fft_loop_pcd (X, n, l);

   // In inverse FFT perform a scale by n and a sign change to Im part
   for (i=0 ; !fwd && i<n ; ++i) {
      _real(X,i) = _real(X,i)/n;
      _imag(X,i) = -_imag(X,i)/n;
   }
}





