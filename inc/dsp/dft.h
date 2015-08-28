/*
 * \file dft.h
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
#ifndef __dft_h__
#define __dft_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>
#include <math/math.h>

/*
 * ================== Public API ====================
 */
// Forward DFT
#if __STDC_VERSION__ >= 201112L

#ifndef fft
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void dft (T *x, T *X, uint32_t n);
 *
 * \note We still have to implement all the functions
 *
 * \brief
 *    Calculate the complex DFT using a not in-place DFT matrix algorithm.
 *
 * \param   x     Pointer to size n time domain array
 * \param   X     Pointer to size n frequency domain complex array
 * \param   n     Number of points
 * \return        None
 */
#define dft(x, X, n)       _Generic((x),  \
       complex_d_t*: dft_c,               \
       complex_f_t*: dft_cf,              \
       complex_i_t*: dft_cf,              \
            double*: dft_r,               \
             float*: dft_rf,              \
               int*: dft_rf,              \
            default: dft_r)(x, X, n)
#endif   // #ifndef dft
#endif   // #if __STDC_VERSION__ >= 201112L

void dft_c (complex_d_t *x, complex_d_t *X, uint32_t n) __O3__ ;
void dft_cf (complex_f_t *x, complex_f_t *X, uint32_t n) __O3__ ;
void dft_r (double *x, complex_d_t *X, uint32_t n) __O3__ ;
void dft_rf (float *x, complex_f_t *X, uint32_t n) __O3__ ;


// Inverse DFT
#if __STDC_VERSION__ >= 201112L

#ifndef idft
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void idft (T *X, T *x, uint32_t n);
 *
 * \note We still have to implement all the functions
 *
 * \brief
 *    Calculate the double precision inverse DFT using a
 *    not in-place DFT matrix algorithm.
 *
 * \note
 *    Due to symmetrical property of real DFT the algorithm doesn't use the
 *    negative frequencies of frequency spectra. So in the case of real inverse DFT
 *    the frequency array can have size n/2+1.
 *
 * \param   X     Pointer to size n frequency domain array
 * \param   x     Pointer to size n time domain array
 * \param   n     Number of points
 * \return        None
 */
#define idft(X, x, n)      _Generic((x),  \
       complex_d_t*: idft_c,              \
       complex_f_t*: idft_cf,             \
       complex_i_t*: idft_cf,             \
            double*: idft_r,              \
             float*: idft_rf,             \
               int*: idft_rf,             \
            default: idft_r)(X, x, n)
#endif   // #ifndef idft
#endif   // #if __STDC_VERSION__ >= 201112L

void idft_c (complex_d_t *X, complex_d_t *x, uint32_t n) __O3__ ;
void idft_cf (complex_f_t *X, complex_f_t *x, uint32_t n) __O3__ ;
void idft_r (complex_d_t *X, double *x, uint32_t n) __O3__ ;
void idft_rf (complex_f_t *X, float *x, uint32_t n) __O3__ ;

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __dft_h__
