/**
 * \file sha2.h
 *
 * \brief SHA-224 and SHA-256 cryptographic hash function
 *
 * Copyright (C) 2006-2010, Brainspark B.V.
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 *
 * This file is part of PolarSSL (http://www.polarssl.org)
 * Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __sha2_h__
#define __sha2_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <inttypes.h>

/*!
 * \brief          SHA-256 context structure
 */
typedef struct
{
    uint32_t total[2];    /*!< number of bytes processed  */
    uint32_t state[8];    /*!< intermediate digest state  */
    uint8_t buffer[64];   /*!< data block being processed */

    int is224;            /*!< 0 => SHA-256, else SHA-224 */
}
sha2_context;


/*!
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha2( uint8_t *input, size_t ilen, uint8_t output[32], int is224 );


#ifdef __cplusplus
}
#endif

#endif // #ifndef __sha2_h__

