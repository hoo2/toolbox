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

#include <stdint.h>
#include <complex.h>
#include <math.h>

/*
 * General Defines
 */

#ifndef M_INVLN2
#define M_INVLN2 1.4426950408889633870E0
#endif

void rfft_f (float *x, float *X, uint32_t n, uint32_t fwd);
void rfft_d (double *x, double *X, uint32_t n, uint32_t fwd);

void cfft_r (double *x, double *X, uint32_t n, uint32_t fwd);
void cfft_c (_Complex double *x, _Complex double *X, uint32_t n, uint32_t fwd);

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __fft_h__


