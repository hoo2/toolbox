/*
 * \file conv.h
 * \brief
 *    A target independent convolution and cross-correlation functionality
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

#ifndef __conv_h__
#define __conv_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <tbx_types.h>

/*
 * ================== Public API ====================
 */
void conv_i (int *y, int *h, int sh, int *x, int sx);
void conv_f (float *y, float *h, int sh, float *x, int sx);
void conv_d (double *y, double *h, int sh, double *x, int sx);
void cconv_i (complex_i_t *y, complex_i_t *h, int sh, complex_i_t *x, int sx);
void cconv_d (complex_d_t *y, complex_d_t *h, int sh, complex_d_t *x, int sx);


#if __STDC_VERSION__ >= 201112L

#ifndef conv
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void conv (T *y, T *h, int sh, T *x, int sx);
 *
 * \brief
 *    Calculates the convolution of h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 * \note We still have to implement all the functions
 */
#define conv(y, h, sh, x, sx) _Generic((y),  int*: conv_i,   \
                                           float*: conv_f,   \
                                          double*: conv_d,   \
                                    _Complex int*: cconv_i,  \
                                 _Complex double*: cconv_d,  \
                                          default: conv_i)(y, h, sh, x, sx)
#endif   // #ifndef conv
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __conv_h__
