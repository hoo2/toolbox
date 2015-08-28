/*
 * \file   toolbox_defs.h
 * \brief  Contains defines for internal and public use.
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

#ifndef __toolbox_defs_h__
#define __toolbox_defs_h__

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Saturation macro
 */
#ifndef _SATURATE
#define _SATURATE(_x, _M, _m)    \
   do {                          \
      if (_x<_m)  _x=_m;         \
      if (_x>_M)  _x=_M;         \
   } while (0)
#endif

/*!
 * This assert macro is used for the toolbox
 */
#define tbx_assert(x)  ((void)sizeof(x))



/*!
 * Optimisations
 */

#ifdef __GNUC0__
#define __O0__    __attribute__ ((optimize("O0")))
#else
#define __O0__
#endif

#ifdef __GNUC0__
#define __O3__    __attribute__ ((optimize("O3")))
#else
#define __O3__
#endif

#ifdef __GNUC0__
#define __Os__    __attribute__ ((optimize("Os")))
#else
#define __Os__
#endif




#ifdef __cplusplus
}
#endif

#endif // #ifndef __toolbox_defs_h__
