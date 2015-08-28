/*
 * \file fft.f
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

#ifndef __fft_h__
#define __fft_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>
#include <math/math.h>
/*
 * General Defines
 */

/*
 * ========= Public API ============
 */
// Forward FFT
#if __STDC_VERSION__ >= 201112L

#ifndef fft
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void fft (T *x, T *X, uint32_t n);
 *
 * \note We still have to implement all the functions
 *
 * \brief
 *    Calculate the forward FFT for complex and real signals
 *    using an in-place decimation in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and frequency
 *    - In-place        Use the same pointer for time and frequency
 *
 * \param   x     Pointer to size n time domain array
 * \param   X     Pointer to size n frequency domain array
 * \param   n     Number of points
 * \return        None
 */
#define fft(x, X, n)       _Generic((x),  \
       complex_d_t*: fft_c,               \
       complex_f_t*: fft_cf,              \
       complex_i_t*: fft_ci,              \
            double*: fft_r,               \
             float*: fft_rf,              \
               int*: fft_ri,              \
            default: fft_r)(x, X, n)
#endif   // #ifndef fft
#endif   // #if __STDC_VERSION__ >= 201112L

void fft_c (complex_d_t *x, complex_d_t *X, uint32_t n) __O3__ ;
void fft_cf (complex_f_t *x, complex_f_t *X, uint32_t n) __O3__ ;
void fft_ci (complex_i_t *x, complex_f_t *X, uint32_t n) __O3__ ;
void fft_r (double *x, complex_d_t *X, uint32_t n) __O3__ ;
void fft_rf (float *x, complex_f_t *X, uint32_t n) __O3__ ;
void fft_ri (int *x, complex_f_t *X, uint32_t n) __O3__ ;


// Inverse FFT
#if __STDC_VERSION__ >= 201112L

#ifndef ifft
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void ifft (T *X, T *x, uint32_t n);
 *
 * \note We still have to implement all the functions
 *
 * \brief
 *    Calculate the inverse FFT for complex and real signals using an
 *    in-place decimation in time algorithm.
 *    This algorithm use an altered in place technique with two different
 *    pointers for time and frequency. Hence the user can use it both
 *    for in-place or not in-place situations.
 *    - Not in-place.   Use pointers to different arrays for time and freq
 *    - In-place        Use the same pointer for time and frequency
 *
 * \warning
 *    Due to inner calculations based on duality property of the DFT, the time domain
 *    and frequency domain signals MUST have the same actual size in bytes. So in the
 *    case or real inverse fft the real time domain pointers MUST point to arrays
 *    with size 2*n
 *
 * \param   X     Pointer to size n frequency domain array
 * \param   x     Pointer to time domain array. size n for complex, size 2*n for real signals
 * \param   n     Number of points
 * \return        None
 */
#define ifft(X, x, n)      _Generic((x),  \
       complex_d_t*: ifft_c,              \
       complex_f_t*: ifft_cf,             \
            double*: ifft_r,              \
             float*: ifft_rf,             \
            default: ifft_r)(X, x, n)
#endif   // #ifndef ifft
#endif   // #if __STDC_VERSION__ >= 201112L

void ifft_c (complex_d_t *X, complex_d_t *x, uint32_t n) __O3__ ;
void ifft_cf (complex_f_t *X, complex_f_t *x, uint32_t n) __O3__ ;
void ifft_r (complex_d_t *X, double *x, uint32_t n) __O3__ ;
void ifft_rf (complex_f_t *X, float *x, uint32_t n) __O3__ ;

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __fft_h__


