/*!
 * \file sha2.h
 * \brief
 *    SHA-224 and SHA-256 cryptographic hash function
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
 * The SHA-256 Secure Hash Standard was published by NIST in 2002.
 * http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 *
 */

#ifndef __sha2_h__
#define __sha2_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <inttypes.h>

typedef enum {SHA2_224=0, SHA2_256} sha2_size;

/*!
 * \brief  SHA-256 context structure
 */
typedef struct
{
    uint32_t total[2];    /*!< number of bytes processed  */
    uint32_t state[8];    /*!< intermediate digest state  */
    uint8_t buffer[64];   /*!< data block being processed */

    sha2_size  sz;        /*!< SHA2_224 or SHA2_256 */
}
sha2_t;


/*!
 * \brief
 *    Output = SHA2 (input buffer)
 *
 * \param input   buffer holding the  data
 * \param ilen    length of the input data
 * \param output  SHA-224/256 checksum result
 * \param sz      The lengh of the hash.
 *    \arg        SHA2_224
 *    \arg        SHA2_256
 * \return        zero on success, non zero on error.
 */
int sha2 (uint8_t *input, size_t ilen, uint8_t output[32], sha2_size sz);


#ifdef __cplusplus
}
#endif

#endif // #ifndef __sha2_h__

