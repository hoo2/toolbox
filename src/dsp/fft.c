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
static uint32_t _log2 (int32_t n) __optimize__ ;
static uint32_t _pow2 (uint32_t e) __optimize__ ;
static void _bit_reverse_c (complex_d_t *x, complex_d_t *r, uint32_t n) __optimize__;
static void _bit_reverse_cf (complex_f_t *x, complex_f_t *r, uint32_t n) __optimize__ ;

/*
 * Static toolbox
 */

/*!
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

/*!
 * \brief
 *    Calculate the 2^e power for integer e
 */
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


/*!
 * \brief
 *    The main body of bit reversal algorithm
 */
#define _bit_reverse_body()                     \
{                                               \
   uint32_t i, j, k, n_2;                       \
   n_2 = n>>1;                                  \
   for (i=1, j=n_2 ; i<n-1 ; ++i) {             \
      if (i<=j) {                               \
         /* complex exchange */                 \
         tmp = x[i]; r[i] = x[j]; r[j] = tmp;   \
      }                                         \
      for (k=n_2 ; k<=j ; k>>=1)                \
         j = j-k;                               \
      j = j+k;                                  \
   }                                            \
   /* Add the extra common nodes */             \
   r[0] = x[0]; r[n-1] = x[n-1];                \
}

/*!
 * \brief
 *    Bit reversal shorting algorithm using double precision
 *    complex numbers.
 *    This algorithm use an altered in place technique with two different
 *    pointers for input and output. Hence the user can use it both
 *    for in-place or not in-place situations.
 *
 * \param   x     Pointer to input signal
 * \param   r     Pointer to output signal
 * \param   n     Number of points
 */
static void _bit_reverse_c (complex_d_t *x, complex_d_t *r, uint32_t n) {
   complex_d_t tmp;
   _bit_reverse_body();
}

/*!
 * \brief
 *    Bit reversal shorting algorithm using single precision
 *    complex numbers.
 *    This algorithm use an altered in place technique with two different
 *    pointers for input and output. Hence the user can use it both
 *    for in-place or not in-place situations.
 *
 * \param   x     Pointer to input signal
 * \param   r     Pointer to output signal
 * \param   n     Number of points
 */
static void _bit_reverse_cf (complex_f_t *x, complex_f_t *r, uint32_t n) {
   complex_f_t tmp;
   _bit_reverse_body();
}


/*!
 * \brief
 *    The main body of fft frequency domain synthesis algorithm
 */
#define  _fft_loop_cmplx(_x, _n, _l)      \
{                                         \
   w = 1.0 + I*0.0;                       \
   le = _pow2 (_l);                       \
   le_2 = le>>1;                          \
   th = M_PI/le_2;                        \
   s = cos (th) - I*sin (th);             \
   /* Loop each sub-DFT  */               \
   for (j=0 ; j<le_2 ; ++j) {             \
      /* Loop each Butterfly */           \
      for (i=j ; i<_n-1 ; i+=le) {        \
         k = i+le_2;                      \
         t = _x[k]*w;                     \
         _x[k] = _x[i]-t;                 \
         _x[i] += t;                      \
      }                                   \
      w *= s;                             \
   }                                      \
}

/*!
 * \brief
 *    Calculate the double precision complex FFT using an in-place decimation
 *    in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *
 * \param   x     Pointer to size n time domain complex array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void fft_c (complex_d_t *x, complex_d_t *X, uint32_t n)
{
   uint32_t i, j, l, m;    // Loop counters
   uint32_t k, le, le_2;
   complex_d_t w, s, t;
   double th;

   // Bit reversal
   _bit_reverse_c (x, X, n);

   // Loop for each stage
   m = _log2(n);
   for (l=1 ; l<=m ; ++l)
      _fft_loop_cmplx (X, n, l);
}

/*!
 * \brief
 *    Calculate the single precision complex FFT using an in-place decimation
 *    in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *
 * \param   x     Pointer to size n time domain complex array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void fft_cf (complex_f_t *x, complex_f_t *X, uint32_t n)
{
   uint32_t i, j, l, m;    // Loop counters
   uint32_t k, le, le_2;
   complex_f_t w, s, t;
   float th;

   // Bit reversal
   _bit_reverse_cf (x, X, n);

   // Loop for each stage
   m = _log2(n);
   for (l=1 ; l<=m ; ++l)
      _fft_loop_cmplx (X, n, l);
}

/*!
 * \brief
 *    Calculate the double precision FFT for real signal, using complex FFT
 *
 *    The algorithm use the even/odd decomposition. The signal, placed in the real part
 *    of the time domain used as an complex stream. The even points as the real part and the
 *    odd points as imaginary part. After calculating the complex DFT (via the FFT, of course),
 *    the spectra are separated using the even/odd decomposition. When two or more signals
 *    need to be passed through the FFT, this technique reduces the execution time by about 35%.
 *    The improvement isn't a full factor of two because of the calculation time
 *    required for the even/odd decomposition.
 *
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *                      In this case time domain array must have 2*n size.
 *
 * \param   x     Pointer to size n time domain array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void fft_r (double *x, complex_d_t *X, uint32_t n)
{
   uint32_t i, j;    // Loop counters
   uint32_t k, le, le_2;
   uint32_t n_2, n_4, _3n_4, im, ip2, ipm;
   complex_d_t w, s, t;
   double th;

   // Calculate helpers
   n_2 = n>>1;
   n_4 = n_2>>1;
   _3n_4 = 3*n_4;

   /*
    * Cast real signal as complex, so even points
    * are real part and odd points are complex
    * Do the FFT to n/2 array
    */
   fft_c ((complex_d_t*)x, X, n_2);

   // Even/odd frequency domain decomposition
   for (i=1 ; i<n_4 ; ++i) {
      im = n_2 - i;
      ip2 = n_2 + i;
      ipm = n_2 + im;
      real(X[ip2]) = (imag(X[i]) + imag(X[im])) / 2;
      real(X[ipm]) = real(X[ip2]);
      imag(X[ip2]) = -(real(X[i]) - real(X[im])) / 2;
      imag(X[ipm]) = -imag(X[ip2]);
      real(X[i])   = (real(X[i]) + real(X[im])) / 2;
      real(X[im])  = real(X[i]);
      imag(X[i])   = (imag(X[i]) - imag(X[im])) / 2;
      imag(X[im])  = -imag(X[i]);
   }
   real(X[_3n_4]) = imag(X[n_4]);
   real(X[n_2]) = imag(X[0]);
   imag(X[0]) = imag(X[n_4]) = imag(X[n_2]) = imag(X[_3n_4]) = 0;

   // Do the last stage of fft
   _fft_loop_cmplx (X, n, _log2(n));
}

/*!
 * \brief
 *    Calculate the single precision FFT for real signal, using complex FFT
 *
 *    The algorithm use the even/odd decomposition. The signal, placed in the real part
 *    of the time domain used as an complex stream. The even points as the real part and the
 *    odd points as imaginary part. After calculating the complex DFT (via the FFT, of course),
 *    the spectra are separated using the even/odd decomposition. When two or more signals
 *    need to be passed through the FFT, this technique reduces the execution time by about 35%.
 *    The improvement isn't a full factor of two because of the calculation time
 *    required for the even/odd decomposition.
 *
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency.
 *                      In this case time domain array must have 2*n size.
 *
 * \param   x     Pointer to size n time domain array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
void fft_rf (float *x, complex_f_t *X, uint32_t n)
{
   uint32_t i, j;    // Loop counters
   uint32_t k, le, le_2;
   uint32_t n_2, n_4, _3n_4, im, ip2, ipm;
   complex_f_t w, s, t;
   float th;

   // Calculate helpers
   n_2 = n>>1;
   n_4 = n_2>>1;
   _3n_4 = 3*n_4;

   /*
    * Cast real signal as complex, so even points
    * are real part and odd points are complex
    * Do the FFT to n/2 array
    */
   fft_cf ((complex_f_t*)x, X, n_2);

   // Even/odd frequency domain decomposition
   for (i=1 ; i<n_4 ; ++i) {
      im = n_2 - i;
      ip2 = n_2 + i;
      ipm = n_2 + im;
      realf(X[ip2]) = (imagf(X[i]) + imagf(X[im])) / 2;
      realf(X[ipm]) = realf(X[ip2]);
      imagf(X[ip2]) = -(realf(X[i]) - realf(X[im])) / 2;
      imagf(X[ipm]) = -imagf(X[ip2]);
      realf(X[i])   = (realf(X[i]) + realf(X[im])) / 2;
      realf(X[im])  = realf(X[i]);
      imagf(X[i])   = (imagf(X[i]) - imagf(X[im])) / 2;
      imagf(X[im])  = -imagf(X[i]);
   }
   realf(X[_3n_4]) = imagf(X[n_4]);
   realf(X[n_2]) = imagf(X[0]);
   imagf(X[0]) = imagf(X[n_4]) = imagf(X[n_2]) = imagf(X[_3n_4]) = 0;

   // Do the last stage of fft
   _fft_loop_cmplx (X, n, _log2(n));
}


/*!
 * \brief
 *    Calculate the double precision inverse complex FFT using an
 *    in-place decimation in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and freq
 *    - In-place        Use the same pointer for time and frequency
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size n time domain complex array
 * \param   n     Number of points
 * \return        None
 */
void ifft_c (complex_d_t *X, complex_d_t *x, uint32_t n)
{
   uint32_t i, j, l, m;    // Loop counters
   uint32_t k, le, le_2;
   complex_d_t w, s, t;
   double th;

   // Bit reversal
   _bit_reverse_c (X, x, n);

   // perform a sign change to Im part first
   for (i=0 ; i<n ; ++i)
      imag(x[i]) = -imag(x[i]);

   // Loop for each stage
   m = _log2(n);
   for (l=1 ; l<=m ; ++l)
      _fft_loop_cmplx (x, n, l);

   // Take the conjugate and scale by n
   for (i=0 ; i<n ; ++i)
      x[i] = conj (x[i])/n;
}

/*
 * \brief
 *    Calculate the single precision inverse complex FFT using an
 *    in-place decimation in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and freq
 *    - In-place        Use the same pointer for time and frequency
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size n time domain complex array
 * \param   n     Number of points
 * \return        None
 */
void ifft_cf (complex_f_t *X, complex_f_t *x, uint32_t n)
{
   uint32_t i, j, l, m;    // Loop counters
   uint32_t k, le, le_2;
   complex_f_t w, s, t;
   float th;

   // Bit reversal
   _bit_reverse_cf (X, x, n);

   // perform a sign change to Im part first
   for (i=0 ; i<n ; ++i)
      imag(x[i]) = -imag(x[i]);

   // Loop for each stage
   m = _log2(n);
   for (l=1 ; l<=m ; ++l)
      _fft_loop_cmplx (x, n, l);

   // Take the conjugate and scale by n
   for (i=0 ; i<n ; ++i)
      x[i] = conjf (x[i])/n;
}

/*!
 * \brief
 *    Calculate the double precision inverse FFT for real signal, using complex FFT
 *
 *    The algorithm use the even/odd decomposition. The signal, placed in the frequency domain
 *    is used as an real stream by combining the real and imaginary parts. After calculating the
 *    real inverse DFT (via the iFFT, of course), the spectra must combine again and scaled by n.
 *    This technique reduces the execution time by about 35%.
 *
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *
 * \warning
 *    Due to inner calculations based on duality property of the DFT, the time domain
 *    and frequency domain signals MUST have the same actual size in bytes. So the real
 *    time domain pointers MUST point to arrays with size 2*n
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size 2*n time domain array
 * \param   n     Number of points
 * \return        None
 */
void ifft_r (complex_d_t *X, double *x, uint32_t n)
{
   uint32_t i, _2n;
   complex_d_t *xx = (complex_d_t*)x;

   // Add real and imaginary part
   for (i=0 ; i<n ; ++i)
      x[i] = real(X[i]) + imag(X[i]);

   // Calculate the real FFT from x
   fft_r (x, xx, n);

   // place the signal to the first half of the array
   for (i=0 ; i<n ; ++i)
      x[i] = (real(xx[i]) + imag(xx[i])) / n;
   for (i=n, _2n = 2*n; i<_2n ; ++i)
      x[i] = 0;
}

/*!
 * \brief
 *    Calculate the single precision inverse FFT for real signal, using complex FFT
 *
 *    The algorithm use the even/odd decomposition. The signal, placed in the frequency domain
 *    is used as an real stream by combining the real and imaginary parts. After calculating the
 *    real inverse DFT (via the iFFT, of course), the spectra must combine again and scaled by n.
 *    This technique reduces the execution time by about 35%.
 *
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *
 * \warning
 *    Due to inner calculations based on duality property of the DFT, the time domain
 *    and frequency domain signals MUST have the same actual size in bytes. So the real
 *    time domain pointers MUST point to arrays with size 2*n
 *
 * \param   X     Pointer to size n frequency domain complex array
 * \param   x     Pointer to size 2*n time domain array
 * \param   n     Number of points
 * \return        None
 */
void ifft_rf (complex_f_t *X, float *x, uint32_t n)
{
   uint32_t i, _2n;
   complex_f_t *xx = (complex_f_t*)x;

   // Add real and imaginary part
   for (i=0 ; i<n ; ++i)
      x[i] = real(X[i]) + imag(X[i]);

   // Calculate the real FFT from x
   fft_rf (x, xx, n);

   // place the signal to the first half of the array
   for (i=0 ; i<n ; ++i)
      x[i] = (realf(xx[i]) + imagf(xx[i])) / n;
   for (i=n, _2n = 2*n; i<_2n ; ++i)
      x[i] = 0;
}

