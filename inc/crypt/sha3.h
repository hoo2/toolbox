/*!
 * \file sha3.h
 * \brief
 *    FIPS-180-2 compliant SHA-384/512 implementation
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2006-2010, Brainspark B.V.
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
 * The SHA-512 Secure Hash Standard was published by NIST in 2002.
 * http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 *
 */

#ifndef __sha3_h__
#define __sha3_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <crypt/cryptint.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>

#if defined(_MSC_VER) || defined(__WATCOMC__)
 #define UL64(x) x##ui64
#else
 #define UL64(x) x##ULL
#endif

typedef enum {SHA3_384=0, SHA3_512} sha3_size;

/*!
 * \brief  SHA-3 context structure
 */
typedef struct
{
   uint64_t total[2];      /*!< number of bytes processed  */
   uint64_t state[8];      /*!< intermediate digest state  */
   uint8_t buffer[128];    /*!< data block being processed */

   sha3_size  sz;          /*!< SHA3_384 or SHA3_512 */
}
sha3_t;


void sha384 (uint8_t *input, size_t ilen, uint8_t output[48]);
void sha512 (uint8_t *input, size_t ilen, uint8_t output[64]);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __sha3_h__
