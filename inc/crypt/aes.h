/*!
 * \file aes.h
 * \brief
 *    FIPS-197 compliant AES implementation
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2001-2004  Christophe Devine
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

#ifndef __aes_h__
#define __aes_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <crypt/cryptint.h>
#include <string.h>
#include <inttypes.h>

typedef struct
{
    uint32_t erk[64];   /* encryption round keys */
    uint32_t drk[64];   /* decryption round keys */
    int nr;             /* number of rounds */
}
aes_t;

typedef enum {AES_128=128, AES_192=192, AES_256=256} aes_size;

void  aes_key_deinit (aes_t *ctx);
void aes128_key_init (aes_t *ctx, uint8_t *key);
void aes192_key_init (aes_t *ctx, uint8_t *key);
void aes256_key_init (aes_t *ctx, uint8_t *key);

void aes_encrypt (aes_t *ctx, uint8_t in[16], uint8_t out[16]);
void aes_decrypt (aes_t *ctx, uint8_t in[16], uint8_t out[16]);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __aes_h__

