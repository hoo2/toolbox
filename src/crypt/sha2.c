/*!
 * \file sha2.c
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
#include <crypt/sha2.h>

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                       \
{                                                  \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )        \
        | ( (uint32_t) (b)[(i) + 1] << 16 )        \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )        \
        | ( (uint32_t) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                       \
{                                                  \
    (b)[(i)    ] = (uint8_t) ( (n) >> 24 );        \
    (b)[(i) + 1] = (uint8_t) ( (n) >> 16 );        \
    (b)[(i) + 2] = (uint8_t) ( (n) >>  8 );        \
    (b)[(i) + 3] = (uint8_t) ( (n)       );        \
}
#endif

#define SHR(x,n)  ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
(                                               \
   W[t] = S1(W[t -  2]) + W[t -  7] +           \
          S0(W[t - 15]) + W[t - 16]             \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
{                                               \
   temp1 = h + S3(e) + F1(e,f,g) + K + x;       \
   temp2 = S2(a) + F0(a,b,c);                   \
   d += temp1;                                  \
   h = temp1 + temp2;                           \
}

static const unsigned char sha2_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define A      (ctx->state[0])
#define B      (ctx->state[1])
#define C      (ctx->state[2])
#define D      (ctx->state[3])
#define E      (ctx->state[4])
#define F      (ctx->state[5])
#define G      (ctx->state[6])
#define H      (ctx->state[7])


// Static functions
static void sha2_pre (sha2_t *ctx, sha2_size sz);
static void sha2_core (sha2_t* ctx, const uint8_t data[64]);
static void sha2_update (sha2_t *ctx, const uint8_t *input, size_t ilen);
static void sha2_post (sha2_t *ctx, uint8_t output[32]);


static void sha2_core (sha2_t* ctx, const uint8_t data[64])
{
   uint32_t temp1, temp2, W[64];

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

   P( A, B, C, D, E, F, G, H, W[ 0], 0x428A2F98 );
   P( H, A, B, C, D, E, F, G, W[ 1], 0x71374491 );
   P( G, H, A, B, C, D, E, F, W[ 2], 0xB5C0FBCF );
   P( F, G, H, A, B, C, D, E, W[ 3], 0xE9B5DBA5 );
   P( E, F, G, H, A, B, C, D, W[ 4], 0x3956C25B );
   P( D, E, F, G, H, A, B, C, W[ 5], 0x59F111F1 );
   P( C, D, E, F, G, H, A, B, W[ 6], 0x923F82A4 );
   P( B, C, D, E, F, G, H, A, W[ 7], 0xAB1C5ED5 );
   P( A, B, C, D, E, F, G, H, W[ 8], 0xD807AA98 );
   P( H, A, B, C, D, E, F, G, W[ 9], 0x12835B01 );
   P( G, H, A, B, C, D, E, F, W[10], 0x243185BE );
   P( F, G, H, A, B, C, D, E, W[11], 0x550C7DC3 );
   P( E, F, G, H, A, B, C, D, W[12], 0x72BE5D74 );
   P( D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE );
   P( C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7 );
   P( B, C, D, E, F, G, H, A, W[15], 0xC19BF174 );
   P( A, B, C, D, E, F, G, H, R(16), 0xE49B69C1 );
   P( H, A, B, C, D, E, F, G, R(17), 0xEFBE4786 );
   P( G, H, A, B, C, D, E, F, R(18), 0x0FC19DC6 );
   P( F, G, H, A, B, C, D, E, R(19), 0x240CA1CC );
   P( E, F, G, H, A, B, C, D, R(20), 0x2DE92C6F );
   P( D, E, F, G, H, A, B, C, R(21), 0x4A7484AA );
   P( C, D, E, F, G, H, A, B, R(22), 0x5CB0A9DC );
   P( B, C, D, E, F, G, H, A, R(23), 0x76F988DA );
   P( A, B, C, D, E, F, G, H, R(24), 0x983E5152 );
   P( H, A, B, C, D, E, F, G, R(25), 0xA831C66D );
   P( G, H, A, B, C, D, E, F, R(26), 0xB00327C8 );
   P( F, G, H, A, B, C, D, E, R(27), 0xBF597FC7 );
   P( E, F, G, H, A, B, C, D, R(28), 0xC6E00BF3 );
   P( D, E, F, G, H, A, B, C, R(29), 0xD5A79147 );
   P( C, D, E, F, G, H, A, B, R(30), 0x06CA6351 );
   P( B, C, D, E, F, G, H, A, R(31), 0x14292967 );
   P( A, B, C, D, E, F, G, H, R(32), 0x27B70A85 );
   P( H, A, B, C, D, E, F, G, R(33), 0x2E1B2138 );
   P( G, H, A, B, C, D, E, F, R(34), 0x4D2C6DFC );
   P( F, G, H, A, B, C, D, E, R(35), 0x53380D13 );
   P( E, F, G, H, A, B, C, D, R(36), 0x650A7354 );
   P( D, E, F, G, H, A, B, C, R(37), 0x766A0ABB );
   P( C, D, E, F, G, H, A, B, R(38), 0x81C2C92E );
   P( B, C, D, E, F, G, H, A, R(39), 0x92722C85 );
   P( A, B, C, D, E, F, G, H, R(40), 0xA2BFE8A1 );
   P( H, A, B, C, D, E, F, G, R(41), 0xA81A664B );
   P( G, H, A, B, C, D, E, F, R(42), 0xC24B8B70 );
   P( F, G, H, A, B, C, D, E, R(43), 0xC76C51A3 );
   P( E, F, G, H, A, B, C, D, R(44), 0xD192E819 );
   P( D, E, F, G, H, A, B, C, R(45), 0xD6990624 );
   P( C, D, E, F, G, H, A, B, R(46), 0xF40E3585 );
   P( B, C, D, E, F, G, H, A, R(47), 0x106AA070 );
   P( A, B, C, D, E, F, G, H, R(48), 0x19A4C116 );
   P( H, A, B, C, D, E, F, G, R(49), 0x1E376C08 );
   P( G, H, A, B, C, D, E, F, R(50), 0x2748774C );
   P( F, G, H, A, B, C, D, E, R(51), 0x34B0BCB5 );
   P( E, F, G, H, A, B, C, D, R(52), 0x391C0CB3 );
   P( D, E, F, G, H, A, B, C, R(53), 0x4ED8AA4A );
   P( C, D, E, F, G, H, A, B, R(54), 0x5B9CCA4F );
   P( B, C, D, E, F, G, H, A, R(55), 0x682E6FF3 );
   P( A, B, C, D, E, F, G, H, R(56), 0x748F82EE );
   P( H, A, B, C, D, E, F, G, R(57), 0x78A5636F );
   P( G, H, A, B, C, D, E, F, R(58), 0x84C87814 );
   P( F, G, H, A, B, C, D, E, R(59), 0x8CC70208 );
   P( E, F, G, H, A, B, C, D, R(60), 0x90BEFFFA );
   P( D, E, F, G, H, A, B, C, R(61), 0xA4506CEB );
   P( C, D, E, F, G, H, A, B, R(62), 0xBEF9A3F7 );
   P( B, C, D, E, F, G, H, A, R(63), 0xC67178F2 );

   A += A;
   B += B;
   C += C;
   D += D;
   E += E;
   F += F;
   G += G;
   H += H;
}


/*!
 * \brief
 *    SHA-256 context setup
 *
 * \param ctx  context to be initialized
 * \param sz   The lengh of the hash.
 *    \arg     SHA2_224
 *    \arg     SHA2_256
 */
static void sha2_pre (sha2_t *ctx, sha2_size sz)
{
   ctx->sz = sz;
   ctx->total[0] = ctx->total[1] = 0;

   switch (sz)
   {
      case SHA2_224:
         A = 0xC1059ED8;
         B = 0x367CD507;
         C = 0x3070DD17;
         D = 0xF70E5939;
         E = 0xFFC00B31;
         F = 0x68581511;
         G = 0x64F98FA7;
         H = 0xBEFA4FA4;
         break;
      case SHA2_256:
         A = 0x6A09E667;
         B = 0xBB67AE85;
         C = 0x3C6EF372;
         D = 0xA54FF53A;
         E = 0x510E527F;
         F = 0x9B05688C;
         G = 0x1F83D9AB;
         H = 0x5BE0CD19;
         break;
      default:
         break;
   }
}

/*!
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
static void sha2_update (sha2_t *ctx, const uint8_t *in, size_t ilen)
{
   size_t fill;
   uint32_t left;

   if (ilen<=0)
      return;

   left = ctx->total[0] & 0x3F;
   fill = 64 - left;

   ctx->total[0] += (uint32_t) ilen;
   ctx->total[0] &= 0xFFFFFFFF;

   if (ctx->total[0] < (uint32_t) ilen)
      ctx->total[1]++;

   if (left && ilen>=fill)
   {
      memcpy ((void*)(ctx->buffer + left), (void*)in, fill );
      sha2_core (ctx, ctx->buffer);
      in += fill;
      ilen  -= fill;
      left = 0;
   }

   while (ilen>=64)
   {
      sha2_core (ctx, in);
      in += 64;
      ilen  -= 64;
   }

   if (ilen>0)
      memcpy ((void*)(ctx->buffer + left), (void*)in, ilen);
}

/*!
 * \brief
 *    SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
static void sha2_post (sha2_t *ctx, uint8_t out[32])
{
   uint32_t last, padn;
   uint32_t high, low;
   uint8_t  msglen[8];

   high = ( ctx->total[0] >> 29 ) | ( ctx->total[1] <<  3 );
   low  = ( ctx->total[0] <<  3 );

   PUT_UINT32_BE (high, msglen, 0);
   PUT_UINT32_BE (low,  msglen, 4);

   last = ctx->total[0] & 0x3F;
   padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

   sha2_update (ctx, (uint8_t *) sha2_padding, padn);
   sha2_update (ctx, msglen, 8);

   PUT_UINT32_BE (ctx->state[0], out,  0);
   PUT_UINT32_BE (ctx->state[1], out,  4);
   PUT_UINT32_BE (ctx->state[2], out,  8);
   PUT_UINT32_BE (ctx->state[3], out, 12);
   PUT_UINT32_BE (ctx->state[4], out, 16);
   PUT_UINT32_BE (ctx->state[5], out, 20);
   PUT_UINT32_BE (ctx->state[6], out, 24);

   if (ctx->sz == SHA2_256)
      PUT_UINT32_BE (ctx->state[7], out, 28);
}

/*!
 * \brief
 *    Output = SHA2 (input buffer)
 *
 * \param input   buffer holding the  data
 * \param ilen    length of the input data
 * \param output  SHA-224/256 checksum result
 * \param sz      The length of the hash.
 *    \arg        SHA2_224
 *    \arg        SHA2_256
 * \return        zero on success, non zero on error.
 */
int sha2 (uint8_t *in, size_t ilen, uint8_t out[32], sha2_size sz)
{
   sha2_t ctx;

   if ( !(sz == SHA2_224 || sz == SHA2_256) )
      return 1;
   sha2_pre (&ctx, sz);
   sha2_update (&ctx, in, ilen);
   sha2_post (&ctx, out);

   // Clear memory for security
   memset (&ctx, 0, sizeof (sha2_t));

   return 0;
}
