/*
 * \file quick_trig.h
 * \brief
 *    A target independent fast trigonometric functions, using
 *    parabolic approximation.
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
#ifndef __quick_trig_h__
#define __quick_trig_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI      3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2    1.57079632679489661923
#endif

#ifndef M_1_PI
#define M_1_PI    0.31830988618379067154
#endif

#ifndef M_2PI
#define M_2PI     6.28318530717958647692
#endif

#define QTR_A    -0.40528473456935108577
#define QTR_B     1.27323954473516268615
#define QTR_P     0.225


double qsin (double th);
double qcos (double th);
double qtan (double th);
double qcot (double th);

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __quick_trig_h__

