/*
 * \file dft.h
 * \brief
 *    A target independent Discrete Fourier Transform functionality
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
#ifndef __dft_h__
#define __dft_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include <tbx_types.h>
#include <dsp/quick_trig.h>

#ifndef M_PI
#define M_PI     3.14159265358979323846
#endif

/*
 * ================== Public API ====================
 */
void rdft_i (int *re, int *im, int *x, int n);
void rdft_f (float *re, float *im, float *x, int n);
void rdft_d (double *re, double *im, double *x, int n);

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __dft_h__
