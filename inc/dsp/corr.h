/*
 * \file corr.h
 * \brief
 *    A target independent cross-correlation functionality
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

#ifndef __corr_h__
#define __corr_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <complex.h>

/*
 * ================== Public API ====================
 */
void xcorr_i (int *y, int *t, int st, int *x, int sx);
void xcorr_f (float *y, float *t, int st, float *x, int sx);
void xcorr_d (double *y, double *t, int st, double *x, int sx);
void cxcorr_i (_Complex int *y, _Complex int *t, int st, _Complex int *x, int sx);
void cxcorr_d (_Complex double *y, _Complex double *t, int st, _Complex double *x, int sx);


#if __STDC_VERSION__ >= 201112L

#ifndef xcorr
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void xcorr (T *y, T *t, int st, T *x, int sx);
 *
 * \brief
 *    Calculates the cross-correlation of t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t'[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 *
 * \note We still have to implement all the functions
 */
#define xcorr(y, t, st, x, sx) _Generic((y),  int*: xcorr_i,   \
                                            float*: xcorr_f,   \
                                           double*: xcorr_d,   \
                                     _Complex int*: cxcorr_i,  \
                                  _Complex double*: cxcorr_d,  \
                                           default: xcorr_i)(y, t, st, x, sx)
#endif   // #ifndef conv
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __conv_h__
