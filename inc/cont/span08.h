/*!
 * \file span08.h
 * \brief
 *    This file provides span capability for 8bit objects
 *
 * Copyright (C) 2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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
#ifndef  __span08_h__
#define  __span08_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <toolbox_defs.h>

#include <stdarg.h>
#include <string.h>

/*!
 * The main span type. This is a non-owning object to a collection of bytes
 */
typedef struct {
   size_t   size;
   byte_t   *data;
}span08_t;

span08_t span08_init (byte_t* data);
span08_t span08_mk (byte_t* data, size_t num, ...);
span08_t span08_add (span08_t* span, byte_t it);
span08_t span08_set (span08_t* span, size_t num, ...);
span08_t span08_cpy (span08_t* span, byte_t* data, size_t num);
span08_t span08_cat (span08_t* span, span08_t src);
byte_t* span08_get (span08_t* span);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef  __span08_h__ */
