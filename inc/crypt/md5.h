/*!
 * \file md5.c
 * \brief
 *    RFC 1321 compliant MD5 implementation
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2006-2014, Brainspark B.V.
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
 *  The MD5 algorithm was designed by Ron Rivest in 1991.
 *  http://www.ietf.org/rfc/rfc1321.txt
 *
 */

#ifndef __md5_h__
#define __md5_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <crypt/cryptint.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>

/*!
 * \brief MD5 context structure
 */
typedef struct
{
   uint32_t  total[2];     /*!< number of bytes processed  */
   uint32_t  state[4];     /*!< intermediate digest state  */
   uint8_t  buffer[64];    /*!< data block being processed */
}
md5_t;


void md5 (const uint8_t *input, size_t ilen, uint8_t output[16]);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __md5_h__
