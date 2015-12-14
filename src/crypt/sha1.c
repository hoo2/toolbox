/*!
 * \file sha1.c
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
#include <crypt/sha1.h>

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                          \
(                                                     \
   temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^    \
          W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],     \
        ( W[t & 0x0F] = S(temp,1) )                   \
)

#define P(a,b,c,d,e,x,k)                              \
{                                                     \
   e += S(a,5) + F(b,c,d) + k + x;                    \
   b = S(b,30);                                       \
}

static const uint8_t sha1_padding[64] =
{
   0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


// Static functions
static void     sha1_pre (sha1_t *ctx);
static void sha1_process (sha1_t* ctx, const uint8_t data[64]);
static void  sha1_update (sha1_t *ctx, const uint8_t *input, size_t ilen);
static void  sha1_finish (sha1_t *ctx, uint8_t output[32]);

/*!
 * \brief
 *    SHA-1 context setup
 *
 * \param ctx  context to be initialised
 */
static void sha1_pre (sha1_t *ctx)
{
   ctx->total[0] = 0;
   ctx->total[1] = 0;

   ctx->state[0] = 0x67452301;
   ctx->state[1] = 0xEFCDAB89;
   ctx->state[2] = 0x98BADCFE;
   ctx->state[3] = 0x10325476;
   ctx->state[4] = 0xC3D2E1F0;
}

/*!
 * \brief
 *  Main SHA-1 process
 * \param ctx      context to be initialised
 * \param data     data to process
 */
static void sha1_process (sha1_t* ctx, const uint8_t data[64])
{
   uint32_t temp, W[16], A, B, C, D, E;

   GET_UINT32_BE (W[ 0], data,  0);
   GET_UINT32_BE (W[ 1], data,  4);
   GET_UINT32_BE (W[ 2], data,  8);
   GET_UINT32_BE (W[ 3], data, 12);
   GET_UINT32_BE (W[ 4], data, 16);
   GET_UINT32_BE (W[ 5], data, 20);
   GET_UINT32_BE (W[ 6], data, 24);
   GET_UINT32_BE (W[ 7], data, 28);
   GET_UINT32_BE (W[ 8], data, 32);
   GET_UINT32_BE (W[ 9], data, 36);
   GET_UINT32_BE (W[10], data, 40);
   GET_UINT32_BE (W[11], data, 44);
   GET_UINT32_BE (W[12], data, 48);
   GET_UINT32_BE (W[13], data, 52);
   GET_UINT32_BE (W[14], data, 56);
   GET_UINT32_BE (W[15], data, 60);

   A = ctx->state[0];
   B = ctx->state[1];
   C = ctx->state[2];
   D = ctx->state[3];
   E = ctx->state[4];

   #define F(x,y,z) (z ^ (x & (y ^ z)))
   P (A, B, C, D, E, W[ 0] ,0x5A827999);
   P (E, A, B, C, D, W[ 1] ,0x5A827999);
   P (D, E, A, B, C, W[ 2] ,0x5A827999);
   P (C, D, E, A, B, W[ 3] ,0x5A827999);
   P (B, C, D, E, A, W[ 4] ,0x5A827999);
   P (A, B, C, D, E, W[ 5] ,0x5A827999);
   P (E, A, B, C, D, W[ 6] ,0x5A827999);
   P (D, E, A, B, C, W[ 7] ,0x5A827999);
   P (C, D, E, A, B, W[ 8] ,0x5A827999);
   P (B, C, D, E, A, W[ 9] ,0x5A827999);
   P (A, B, C, D, E, W[10] ,0x5A827999);
   P (E, A, B, C, D, W[11] ,0x5A827999);
   P (D, E, A, B, C, W[12] ,0x5A827999);
   P (C, D, E, A, B, W[13] ,0x5A827999);
   P (B, C, D, E, A, W[14] ,0x5A827999);
   P (A, B, C, D, E, W[15] ,0x5A827999);
   P (E, A, B, C, D, R(16) ,0x5A827999);
   P (D, E, A, B, C, R(17) ,0x5A827999);
   P (C, D, E, A, B, R(18) ,0x5A827999);
   P (B, C, D, E, A, R(19) ,0x5A827999);
   #undef F

   #define F(x,y,z) (x ^ y ^ z)
   P (A, B, C, D, E, R(20) ,0x6ED9EBA1);
   P (E, A, B, C, D, R(21) ,0x6ED9EBA1);
   P (D, E, A, B, C, R(22) ,0x6ED9EBA1);
   P (C, D, E, A, B, R(23) ,0x6ED9EBA1);
   P (B, C, D, E, A, R(24) ,0x6ED9EBA1);
   P (A, B, C, D, E, R(25) ,0x6ED9EBA1);
   P (E, A, B, C, D, R(26) ,0x6ED9EBA1);
   P (D, E, A, B, C, R(27) ,0x6ED9EBA1);
   P (C, D, E, A, B, R(28) ,0x6ED9EBA1);
   P (B, C, D, E, A, R(29) ,0x6ED9EBA1);
   P (A, B, C, D, E, R(30) ,0x6ED9EBA1);
   P (E, A, B, C, D, R(31) ,0x6ED9EBA1);
   P (D, E, A, B, C, R(32) ,0x6ED9EBA1);
   P (C, D, E, A, B, R(33) ,0x6ED9EBA1);
   P (B, C, D, E, A, R(34) ,0x6ED9EBA1);
   P (A, B, C, D, E, R(35) ,0x6ED9EBA1);
   P (E, A, B, C, D, R(36) ,0x6ED9EBA1);
   P (D, E, A, B, C, R(37) ,0x6ED9EBA1);
   P (C, D, E, A, B, R(38) ,0x6ED9EBA1);
   P (B, C, D, E, A, R(39) ,0x6ED9EBA1);
   #undef F

   #define F(x,y,z) ((x & y) | (z & (x | y)))
   P (A, B, C, D, E, R(40) ,0x8F1BBCDC);
   P (E, A, B, C, D, R(41) ,0x8F1BBCDC);
   P (D, E, A, B, C, R(42) ,0x8F1BBCDC);
   P (C, D, E, A, B, R(43) ,0x8F1BBCDC);
   P (B, C, D, E, A, R(44) ,0x8F1BBCDC);
   P (A, B, C, D, E, R(45) ,0x8F1BBCDC);
   P (E, A, B, C, D, R(46) ,0x8F1BBCDC);
   P (D, E, A, B, C, R(47) ,0x8F1BBCDC);
   P (C, D, E, A, B, R(48) ,0x8F1BBCDC);
   P (B, C, D, E, A, R(49) ,0x8F1BBCDC);
   P (A, B, C, D, E, R(50) ,0x8F1BBCDC);
   P (E, A, B, C, D, R(51) ,0x8F1BBCDC);
   P (D, E, A, B, C, R(52) ,0x8F1BBCDC);
   P (C, D, E, A, B, R(53) ,0x8F1BBCDC);
   P (B, C, D, E, A, R(54) ,0x8F1BBCDC);
   P (A, B, C, D, E, R(55) ,0x8F1BBCDC);
   P (E, A, B, C, D, R(56) ,0x8F1BBCDC);
   P (D, E, A, B, C, R(57) ,0x8F1BBCDC);
   P (C, D, E, A, B, R(58) ,0x8F1BBCDC);
   P (B, C, D, E, A, R(59) ,0x8F1BBCDC);
   #undef F

   #define F(x,y,z) (x ^ y ^ z)
   P (A, B, C, D, E, R(60) ,0xCA62C1D6);
   P (E, A, B, C, D, R(61) ,0xCA62C1D6);
   P (D, E, A, B, C, R(62) ,0xCA62C1D6);
   P (C, D, E, A, B, R(63) ,0xCA62C1D6);
   P (B, C, D, E, A, R(64) ,0xCA62C1D6);
   P (A, B, C, D, E, R(65) ,0xCA62C1D6);
   P (E, A, B, C, D, R(66) ,0xCA62C1D6);
   P (D, E, A, B, C, R(67) ,0xCA62C1D6);
   P (C, D, E, A, B, R(68) ,0xCA62C1D6);
   P (B, C, D, E, A, R(69) ,0xCA62C1D6);
   P (A, B, C, D, E, R(70) ,0xCA62C1D6);
   P (E, A, B, C, D, R(71) ,0xCA62C1D6);
   P (D, E, A, B, C, R(72) ,0xCA62C1D6);
   P (C, D, E, A, B, R(73) ,0xCA62C1D6);
   P (B, C, D, E, A, R(74) ,0xCA62C1D6);
   P (A, B, C, D, E, R(75) ,0xCA62C1D6);
   P (E, A, B, C, D, R(76) ,0xCA62C1D6);
   P (D, E, A, B, C, R(77) ,0xCA62C1D6);
   P (C, D, E, A, B, R(78) ,0xCA62C1D6);
   P (B, C, D, E, A, R(79) ,0xCA62C1D6);
   #undef F

   ctx->state[0] += A;
   ctx->state[1] += B;
   ctx->state[2] += C;
   ctx->state[3] += D;
   ctx->state[4] += E;
}

/*!
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
static void sha1_update (sha1_t *ctx, const uint8_t *in, size_t ilen)
{
   size_t fill;
   uint32_t left;

   if (ilen <= 0)
       return;

   left = ctx->total[0] & 0x3F;
   fill = 64 - left;

   ctx->total[0] += (uint32_t) ilen;
   ctx->total[0] &= 0xFFFFFFFF;

   if (ctx->total[0] < (uint32_t) ilen)
      ctx->total[1]++;

   if (left && ilen >= fill) {
      memcpy ((void *) (ctx->buffer + left), in, fill);
      sha1_process (ctx, ctx->buffer);
      in += fill;
      ilen  -= fill;
      left = 0;
   }

   while (ilen >= 64) {
      sha1_process (ctx, in);
      in += 64;
      ilen  -= 64;
   }

   if (ilen > 0)
      memcpy ((void *) (ctx->buffer + left), in, ilen);
}

/*!
 * \brief
 *    SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
static void sha1_finish (sha1_t *ctx, uint8_t out[20])
{
   uint32_t last, padn;
   uint32_t high, low;
   uint8_t  msglen[8];

   high = (ctx->total[0] >> 29) | (ctx->total[1] <<  3);
   low  = (ctx->total[0] <<  3);

   PUT_UINT32_BE (high, msglen, 0);
   PUT_UINT32_BE (low,  msglen, 4);

   last = ctx->total[0] & 0x3F;
   padn = (last < 56) ? (56 - last) : (120 - last);

   sha1_update (ctx, sha1_padding, padn);
   sha1_update (ctx, msglen, 8);

   PUT_UINT32_BE (ctx->state[0], out,  0);
   PUT_UINT32_BE (ctx->state[1], out,  4);
   PUT_UINT32_BE (ctx->state[2], out,  8);
   PUT_UINT32_BE (ctx->state[3], out, 12);
   PUT_UINT32_BE (ctx->state[4], out, 16);
}

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Output = SHA1 (input buffer)
 *
 * \param input   buffer holding the  data
 * \param ilen    length of the input data
 * \param output  SHA-1 checksum result
 * \return        none
 */
void sha1 (const uint8_t *input, size_t ilen, uint8_t output[20])
{
   sha1_t ctx;

   sha1_pre (&ctx);
   sha1_update (&ctx, input, ilen);
   sha1_finish (&ctx, output);

   memset (&ctx, 0, sizeof (sha1_t));
}
