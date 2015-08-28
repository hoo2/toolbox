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

static void _bit_reverse_c (complex_d_t *x, complex_d_t *r, uint32_t n) __O3__;
static void _bit_reverse_cf (complex_f_t *x, complex_f_t *r, uint32_t n) __O3__ ;
static void _bit_reverse_ci (complex_i_t *x, complex_f_t *r, uint32_t n) __O3__ ;


/*!
 * \brief
 *    The main body of bit reversal algorithm
 * \param   _t    The type for the conversion
 */
#define _bit_reverse_body(_t)                   \
{                                               \
   uint32_t i, j, k, n_2;                       \
   n_2 = n>>1;                                  \
   for (i=1, j=n_2 ; i<n-1 ; ++i) {             \
      /* point exchange and type conversion */  \
      if (i<=j) {                               \
         tmp = (_t)x[i];                        \
         r[i] = (_t)x[j];                       \
         r[j] = tmp;                            \
      }                                         \
      for (k=n_2 ; k<=j ; k>>=1)                \
         j = j-k;                               \
      j = j+k;                                  \
   }                                            \
   /* Add the extra common nodes */             \
   r[0] = (_t)x[0]; r[n-1] = (_t)x[n-1];        \
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
   _bit_reverse_body(complex_d_t);
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
   _bit_reverse_body(complex_f_t);
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
static void _bit_reverse_ci (complex_i_t *x, complex_f_t *r, uint32_t n) {
   complex_f_t tmp;
   _bit_reverse_body(complex_f_t);
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
 *    The main body of fft
 */
#define _fft_body(_type, _reverse) {            \
   uint32_t i, j, l, m;    /* Loop counters */  \
   uint32_t k, le, le_2;   /* butterfly loop */ \
   _type w, s, t;                         \
   double th;  /* Always double like sin/cos */ \
                                                \
   /* Bit reversal */                           \
   _reverse (x, X, n);                    \
                                                \
   /* Loop for each stage */                    \
   m = _log2(n);                                \
   for (l=1 ; l<=m ; ++l)                       \
      _fft_loop_cmplx (X, n, l);                \
}

/*!
 * \brief
 *    The main body of fft for real signals
 */
#define _fft_r_body(_intype, _outtype, _fft, _r, _i) {   \
   uint32_t i, j;    /* Loop counters */                 \
   uint32_t k, le, le_2; /* butterfly loop */            \
   uint32_t n_2, n_4, _3n_4, im, ip2, ipm;               \
   _outtype w, s, t;                                  \
   double th;  /* Always double like sin/cos */          \
                                                         \
   /* Calculate helpers */                               \
   n_2 = n>>1;                                           \
   n_4 = n_2>>1;                                         \
   _3n_4 = 3*n_4;                                        \
                                                         \
   /* Cast real signal as complex, so even */            \
   /* points became real part and odd points */          \
   /* became imaginary. Then do FFT to n/2 */            \
   _fft ((_intype*)x, X, n_2);                    \
                                                         \
   /* Even/odd frequency domain decomposition */         \
   for (i=1 ; i<n_4 ; ++i) {                             \
      im = n_2 - i;                                      \
      ip2 = n_2 + i;                                     \
      ipm = n_2 + im;                                    \
      _r(X[ip2]) = (_i(X[i]) + _i(X[im])) / 2;           \
      _i(X[ip2]) = -(_r(X[i]) - _r(X[im])) / 2;          \
      _r(X[ipm]) = _r(X[ip2]);                           \
      _i(X[ipm]) = -_i(X[ip2]);                          \
      _r(X[i])   = (_r(X[i]) + _r(X[im])) / 2;           \
      _i(X[i])   = (_i(X[i]) - _i(X[im])) / 2;           \
      _r(X[im])  = _r(X[i]);                             \
      _i(X[im])  = -_i(X[i]);                            \
   }                                                     \
   _r(X[_3n_4]) = _i(X[n_4]);                            \
   _r(X[n_2]) = _i(X[0]);                                \
   _i(X[0]) = _i(X[n_4]) = _i(X[n_2]) = _i(X[_3n_4]) = 0; \
                                                         \
   /* Do the last frequency domain synthesis loop */     \
   _fft_loop_cmplx (X, n, _log2(n));                     \
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
void fft_c (complex_d_t *x, complex_d_t *X, uint32_t n) {
   _fft_body (complex_d_t, _bit_reverse_c);
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
void fft_cf (complex_f_t *x, complex_f_t *X, uint32_t n) {
   _fft_body (complex_f_t, _bit_reverse_cf);
}

/*!
 * \brief
 *    Calculate the single precision complex FFT for complex integer input,
 *    using an in-place decimation in time algorithm.
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
void fft_ci (complex_i_t *x, complex_f_t *X, uint32_t n) {
   _fft_body (complex_f_t, _bit_reverse_ci);
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
void fft_r (double *x, complex_d_t *X, uint32_t n) {
   _fft_r_body (complex_d_t, complex_d_t, fft_c, real, imag);
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
void fft_rf (float *x, complex_f_t *X, uint32_t n) {
   _fft_r_body (complex_f_t, complex_f_t, fft_cf, realf, imagf);
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
void fft_ri (int *x, complex_f_t *X, uint32_t n) {
   _fft_r_body (complex_i_t, complex_f_t, fft_ci, realf, imagf);
}

/*!
 * \brief
 *    Inverse fft main body
 */
#define _ifft_body(_type, _reverse, _i, _c) {  \
   uint32_t i, j, l, m;    /* Loop counters */  \
   uint32_t k, le, le_2;   /* butterfly loop */ \
   _type w, s, t;                         \
   double th;  /* Always double like sin/cos*/  \
                                                \
   _reverse (X, x, n);  /* Bit reversal */      \
                                                \
   /* Convert to the conjugate */               \
   for (i=0 ; i<n ; ++i)                        \
   _i(x[i]) = -_i(x[i]);                 \
                                                \
   /* Loop for each stage */                    \
   m = _log2(n);                                \
   for (l=1 ; l<=m ; ++l)                       \
      _fft_loop_cmplx (x, n, l);                \
                                                \
   /* Take the conjugate and scale by n */      \
   for (i=0 ; i<n ; ++i)                        \
      x[i] = _c (x[i])/n;                    \
}

/*!
 * \brief
 *    Inverse fft main body for real signals
 */
#define _ifft_r_body(_type, _fft, _r, _i) { \
   uint32_t i, _2n;                             \
   _type *xx = (_type*)x;                       \
                                                \
   /* Add real and imaginary part */            \
   for (i=0 ; i<n ; ++i)                        \
      x[i] = _r(X[i]) + _i(X[i]);               \
                                                \
   /* Calculate the real FFT from x */          \
   _fft (x, xx, n);                             \
                                                \
   /* place the signal to the first half of the array */ \
   for (i=0 ; i<n ; ++i)                        \
      x[i] = (_r(xx[i]) + _i(xx[i])) / n;       \
   for (i=n, _2n = 2*n; i<_2n ; ++i)            \
      x[i] = 0;                                 \
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
void ifft_c (complex_d_t *X, complex_d_t *x, uint32_t n) {
   _ifft_body (complex_d_t, _bit_reverse_c, imag, conj);
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
void ifft_cf (complex_f_t *X, complex_f_t *x, uint32_t n) {
   _ifft_body (complex_f_t, _bit_reverse_cf, imagf, conjf);
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
void ifft_r (complex_d_t *X, double *x, uint32_t n) {
   _ifft_r_body (complex_d_t, fft_r, real, imag);
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
void ifft_rf (complex_f_t *X, float *x, uint32_t n) {
   _ifft_r_body (complex_f_t, fft_rf, realf, imagf);
}

