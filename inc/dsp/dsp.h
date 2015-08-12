/*
 * \file dsp.h
 * \brief
 *    Common dsp header file
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

#ifndef __dsp_h__
#define __dsp_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <complex.h>
#include <tbx_types.h>
#include <math.h>


#ifdef __GNUC__
#define __optimize__    __attribute__ ((optimize("O3")))
#else
#define __optimize__
#endif

#ifndef M_E
#define M_E    2.7182818284590452354
#endif

#ifndef M_LOG2E
#define M_LOG2E      1.4426950408889634074
#endif

#ifndef M_LOG10E
#define M_LOG10E  0.43429448190325182765
#endif

#ifndef M_LN2
#define M_LN2     _M_LN2
#endif

#ifndef M_LN10
#define M_LN10    2.30258509299404568402
#endif

#ifndef M_PI
#define M_PI      3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2    1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4    0.78539816339744830962
#endif

#ifndef M_1_PI
#define M_1_PI    0.31830988618379067154
#endif

#ifndef M_2_PI
#define M_2_PI    0.63661977236758134308
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI   1.12837916709551257390
#endif

#ifndef M_SQRT2
#define M_SQRT2      1.41421356237309504880
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __dsp_h__
