/*!
 * \file sha1.h
 * \brief
 *    SHA-1 cryptographic hash function
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
 * The SHA-1 standard was published by NIST in 1993.
 * http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 */

#ifndef __sha1_h__
#define __sha1_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <crypt/cryptint.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>

/*!
 * \brief
 *    SHA-1 context structure
 */
typedef struct
{
    uint32_t total[2];     /*!< number of bytes processed  */
    uint32_t state[5];     /*!< intermediate digest state  */
    uint8_t  buffer[64];   /*!< data block being processed */
}
sha1_t;


/*!
 * \brief
 *    Output = SHA1 (input buffer)
 *
 * \param input   buffer holding the  data
 * \param ilen    length of the input data
 * \param output  SHA-1 checksum result
 * \return        none
 */
void sha1 (const uint8_t *input, size_t ilen, uint8_t output[20]);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __sha1_h__

