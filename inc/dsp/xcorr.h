/*
 * \file xcorr.h
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

#ifndef __xcorr_h__
#define __xcorr_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>

/*
 * ================== Public API ====================
 */
void xcorr_i (int *y, int *t, int32_t st, int *x, int32_t sx) __O3__ ;
void xcorr_f (float *y, float *t, int32_t st, float *x, int32_t sx) __O3__ ;
void xcorr_d (double *y, double *t, int32_t st, double *x, int32_t sx) __O3__ ;
void xcorr_ci (complex_i_t *y, complex_i_t *t, int32_t st, complex_i_t *x, int32_t sx) __O3__ ;
void xcorr_cf (complex_f_t *y, complex_f_t *t, int32_t st, complex_f_t *x, int32_t sx) __O3__ ;
void xcorr_cd (complex_d_t *y, complex_d_t *t, int32_t st, complex_d_t *x, int32_t sx) __O3__ ;


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
                                      complex_i_t*: xcorr_ci,  \
                                      complex_f_t*: xcorr_cf,  \
                                      complex_d_t*: xcorr_cd,  \
                                           default: xcorr_d)(y, t, st, x, sx)
#endif   // #ifndef xcorr
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __xcorr_h__
