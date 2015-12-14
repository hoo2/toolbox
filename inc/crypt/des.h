/*!
 * \file des.c
 * \brief
 *    FIPS-46-3 compliant Triple-DES implementation
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
 * DES, on which TDES is based, was originally designed by Horst Feistel
 * at IBM in 1974, and was adopted as a standard by NIST (formerly NBS).
 * http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
 *
 */

#ifndef __des_h__
#define __des_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <crypt/cryptint.h>
#include <string.h>
#include <inttypes.h>

#define DES_KEY_SIZE    8

/*!
 * \brief
 *    DES context structure
 */
typedef struct
{
    int      mode;      /*!<  encrypt/decrypt   */
    uint32_t sk[32];    /*!<  DES subkeys       */
}
des_t;

/*!
 * \brief
 *    Triple-DES context structure
 */
typedef struct
{
    int      mode;      /*!<  encrypt/decrypt   */
    uint32_t sk[96];    /*!<  3DES subkeys      */
}
des3_t;


/*
 * =================== Public Functions ===================
 */
void  des_key_set_parity (uint8_t key[DES_KEY_SIZE]);
int des_key_check_parity (const uint8_t key[DES_KEY_SIZE]);
int   des_key_check_weak (const uint8_t key[DES_KEY_SIZE]);

void   des_setkey_enc (des_t *ctx, const uint8_t key[8]);
void   des_setkey_dec (des_t *ctx, const uint8_t key[8]);

void des3_set2key_enc (des3_t *ctx, const uint8_t key[16]);
void des3_set2key_dec (des3_t *ctx, const uint8_t key[16]);

void des3_set3key_enc (des3_t *ctx, const uint8_t key[24]);
void des3_set3key_dec (des3_t *ctx, const uint8_t key[24]);

void  des_crypt_ecb (des_t *ctx, const uint8_t input[8], uint8_t output[8]);
void des3_crypt_ecb (des3_t *ctx, const uint8_t input[8], uint8_t output[8]);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __des_h__
