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

#include <math/math.h>

int32_t isin_S4 (int32_t x)  ;
int32_t icos_S4 (int32_t x)  ;


#ifdef __cplusplus
}
#endif

#endif   // #ifndef __quick_trig_h__

