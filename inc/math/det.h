/*
 * \file det.h
 * \brief
 *    Small and quick math functions, that are not general cases
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

#ifndef __det__h__
#define __det__h__

#ifdef __cplusplus
extern "C" {
#endif

#include <math/math.h>
#include <math.h>

#define DET_MAX_SIZE          (5)

#if   DET_MAX_SIZE == 3
#define _DET_MINORS_SIZE      (12)  // 3 * (2x2)
#elif DET_MAX_SIZE == 4
#define _DET_MINORS_SIZE      (48)  // 4 * (3x3)
#elif DET_MAX_SIZE == 5
#define _DET_MINORS_SIZE      (240)  // 5 * (4x4)
#elif DET_MAX_SIZE == 6
#define _DET_MINORS_SIZE      (1440) // 6 * (5x5)
#else
#error "Determinant size out of limits"
#endif

float detf (float *A, int n);


#ifdef __cplusplus
}
#endif

#endif   // #ifndef __det__h__
