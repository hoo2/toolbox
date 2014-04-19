/*!
 * \file aes.c
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

#include <crypt/aes.h>


//#define RAM_TABLES

/*
 * Platform-independent 32-bit integer manipulation macros
 */
#define GET_UINT32(n,b,i)                      \
{                                              \
   (n) = ( (uint32_t) (b)[(i)    ] << 24 )     \
       | ( (uint32_t) (b)[(i) + 1] << 16 )     \
       | ( (uint32_t) (b)[(i) + 2] <<  8 )     \
       | ( (uint32_t) (b)[(i) + 3]       );    \
}  /*!< Get an unsigned integer 32 value from array \b b */

#define PUT_UINT32(n,b,i)                      \
{                                              \
   (b)[(i)    ] = (uint8_t) ( (n) >> 24 );     \
   (b)[(i) + 1] = (uint8_t) ( (n) >> 16 );     \
   (b)[(i) + 2] = (uint8_t) ( (n) >>  8 );     \
   (b)[(i) + 3] = (uint8_t) ( (n)       );     \
}  /*!< Put an unsigned integer 32 value to array \b b */


#ifdef RAM_TABLES
/*
 * Tables generation defines
 */
#define ROTR8(x) ( ( ( x << 24 ) & 0xFFFFFFFF ) | ( ( x & 0xFFFFFFFF ) >>  8 ) )
#define XTIME(x) ( ( x <<  1 ) ^ ( ( x & 0x80 ) ? 0x1B : 0x00 ) )
#define MUL(x,y) ( ( x &&  y ) ? pow[(log[x] + log[y]) % 255] : 0 )

#endif


#ifndef RAM_TABLES
/*!<
 * \brief
 *    Forward Reverse S-box & Tables will placed in rom. This way
 *    we dont need to call \sa _aes_create_tables().
 */

/*!
 *  Forward S-box table
 */
static const uint32_t FSbox[256] =
{
   0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
   0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
   0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
   0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
   0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
   0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
   0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
   0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
   0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
   0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
   0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
   0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
   0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
   0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
   0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
   0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
   0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
   0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
   0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
   0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
   0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
   0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
   0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
   0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
   0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
   0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
   0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
   0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
   0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
   0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
   0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
   0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/*!
 * Define for Encrypt Forward tables
 */
#define FWDT \
   _EX(C6,63,63,A5), _EX(F8,7C,7C,84), _EX(EE,77,77,99), _EX(F6,7B,7B,8D), \
   _EX(FF,F2,F2,0D), _EX(D6,6B,6B,BD), _EX(DE,6F,6F,B1), _EX(91,C5,C5,54), \
   _EX(60,30,30,50), _EX(02,01,01,03), _EX(CE,67,67,A9), _EX(56,2B,2B,7D), \
   _EX(E7,FE,FE,19), _EX(B5,D7,D7,62), _EX(4D,AB,AB,E6), _EX(EC,76,76,9A), \
   _EX(8F,CA,CA,45), _EX(1F,82,82,9D), _EX(89,C9,C9,40), _EX(FA,7D,7D,87), \
   _EX(EF,FA,FA,15), _EX(B2,59,59,EB), _EX(8E,47,47,C9), _EX(FB,F0,F0,0B), \
   _EX(41,AD,AD,EC), _EX(B3,D4,D4,67), _EX(5F,A2,A2,FD), _EX(45,AF,AF,EA), \
   _EX(23,9C,9C,BF), _EX(53,A4,A4,F7), _EX(E4,72,72,96), _EX(9B,C0,C0,5B), \
   _EX(75,B7,B7,C2), _EX(E1,FD,FD,1C), _EX(3D,93,93,AE), _EX(4C,26,26,6A), \
   _EX(6C,36,36,5A), _EX(7E,3F,3F,41), _EX(F5,F7,F7,02), _EX(83,CC,CC,4F), \
   _EX(68,34,34,5C), _EX(51,A5,A5,F4), _EX(D1,E5,E5,34), _EX(F9,F1,F1,08), \
   _EX(E2,71,71,93), _EX(AB,D8,D8,73), _EX(62,31,31,53), _EX(2A,15,15,3F), \
   _EX(08,04,04,0C), _EX(95,C7,C7,52), _EX(46,23,23,65), _EX(9D,C3,C3,5E), \
   _EX(30,18,18,28), _EX(37,96,96,A1), _EX(0A,05,05,0F), _EX(2F,9A,9A,B5), \
   _EX(0E,07,07,09), _EX(24,12,12,36), _EX(1B,80,80,9B), _EX(DF,E2,E2,3D), \
   _EX(CD,EB,EB,26), _EX(4E,27,27,69), _EX(7F,B2,B2,CD), _EX(EA,75,75,9F), \
   _EX(12,09,09,1B), _EX(1D,83,83,9E), _EX(58,2C,2C,74), _EX(34,1A,1A,2E), \
   _EX(36,1B,1B,2D), _EX(DC,6E,6E,B2), _EX(B4,5A,5A,EE), _EX(5B,A0,A0,FB), \
   _EX(A4,52,52,F6), _EX(76,3B,3B,4D), _EX(B7,D6,D6,61), _EX(7D,B3,B3,CE), \
   _EX(52,29,29,7B), _EX(DD,E3,E3,3E), _EX(5E,2F,2F,71), _EX(13,84,84,97), \
   _EX(A6,53,53,F5), _EX(B9,D1,D1,68), _EX(00,00,00,00), _EX(C1,ED,ED,2C), \
   _EX(40,20,20,60), _EX(E3,FC,FC,1F), _EX(79,B1,B1,C8), _EX(B6,5B,5B,ED), \
   _EX(D4,6A,6A,BE), _EX(8D,CB,CB,46), _EX(67,BE,BE,D9), _EX(72,39,39,4B), \
   _EX(94,4A,4A,DE), _EX(98,4C,4C,D4), _EX(B0,58,58,E8), _EX(85,CF,CF,4A), \
   _EX(BB,D0,D0,6B), _EX(C5,EF,EF,2A), _EX(4F,AA,AA,E5), _EX(ED,FB,FB,16), \
   _EX(86,43,43,C5), _EX(9A,4D,4D,D7), _EX(66,33,33,55), _EX(11,85,85,94), \
   _EX(8A,45,45,CF), _EX(E9,F9,F9,10), _EX(04,02,02,06), _EX(FE,7F,7F,81), \
   _EX(A0,50,50,F0), _EX(78,3C,3C,44), _EX(25,9F,9F,BA), _EX(4B,A8,A8,E3), \
   _EX(A2,51,51,F3), _EX(5D,A3,A3,FE), _EX(80,40,40,C0), _EX(05,8F,8F,8A), \
   _EX(3F,92,92,AD), _EX(21,9D,9D,BC), _EX(70,38,38,48), _EX(F1,F5,F5,04), \
   _EX(63,BC,BC,DF), _EX(77,B6,B6,C1), _EX(AF,DA,DA,75), _EX(42,21,21,63), \
   _EX(20,10,10,30), _EX(E5,FF,FF,1A), _EX(FD,F3,F3,0E), _EX(BF,D2,D2,6D), \
   _EX(81,CD,CD,4C), _EX(18,0C,0C,14), _EX(26,13,13,35), _EX(C3,EC,EC,2F), \
   _EX(BE,5F,5F,E1), _EX(35,97,97,A2), _EX(88,44,44,CC), _EX(2E,17,17,39), \
   _EX(93,C4,C4,57), _EX(55,A7,A7,F2), _EX(FC,7E,7E,82), _EX(7A,3D,3D,47), \
   _EX(C8,64,64,AC), _EX(BA,5D,5D,E7), _EX(32,19,19,2B), _EX(E6,73,73,95), \
   _EX(C0,60,60,A0), _EX(19,81,81,98), _EX(9E,4F,4F,D1), _EX(A3,DC,DC,7F), \
   _EX(44,22,22,66), _EX(54,2A,2A,7E), _EX(3B,90,90,AB), _EX(0B,88,88,83), \
   _EX(8C,46,46,CA), _EX(C7,EE,EE,29), _EX(6B,B8,B8,D3), _EX(28,14,14,3C), \
   _EX(A7,DE,DE,79), _EX(BC,5E,5E,E2), _EX(16,0B,0B,1D), _EX(AD,DB,DB,76), \
   _EX(DB,E0,E0,3B), _EX(64,32,32,56), _EX(74,3A,3A,4E), _EX(14,0A,0A,1E), \
   _EX(92,49,49,DB), _EX(0C,06,06,0A), _EX(48,24,24,6C), _EX(B8,5C,5C,E4), \
   _EX(9F,C2,C2,5D), _EX(BD,D3,D3,6E), _EX(43,AC,AC,EF), _EX(C4,62,62,A6), \
   _EX(39,91,91,A8), _EX(31,95,95,A4), _EX(D3,E4,E4,37), _EX(F2,79,79,8B), \
   _EX(D5,E7,E7,32), _EX(8B,C8,C8,43), _EX(6E,37,37,59), _EX(DA,6D,6D,B7), \
   _EX(01,8D,8D,8C), _EX(B1,D5,D5,64), _EX(9C,4E,4E,D2), _EX(49,A9,A9,E0), \
   _EX(D8,6C,6C,B4), _EX(AC,56,56,FA), _EX(F3,F4,F4,07), _EX(CF,EA,EA,25), \
   _EX(CA,65,65,AF), _EX(F4,7A,7A,8E), _EX(47,AE,AE,E9), _EX(10,08,08,18), \
   _EX(6F,BA,BA,D5), _EX(F0,78,78,88), _EX(4A,25,25,6F), _EX(5C,2E,2E,72), \
   _EX(38,1C,1C,24), _EX(57,A6,A6,F1), _EX(73,B4,B4,C7), _EX(97,C6,C6,51), \
   _EX(CB,E8,E8,23), _EX(A1,DD,DD,7C), _EX(E8,74,74,9C), _EX(3E,1F,1F,21), \
   _EX(96,4B,4B,DD), _EX(61,BD,BD,DC), _EX(0D,8B,8B,86), _EX(0F,8A,8A,85), \
   _EX(E0,70,70,90), _EX(7C,3E,3E,42), _EX(71,B5,B5,C4), _EX(CC,66,66,AA), \
   _EX(90,48,48,D8), _EX(06,03,03,05), _EX(F7,F6,F6,01), _EX(1C,0E,0E,12), \
   _EX(C2,61,61,A3), _EX(6A,35,35,5F), _EX(AE,57,57,F9), _EX(69,B9,B9,D0), \
   _EX(17,86,86,91), _EX(99,C1,C1,58), _EX(3A,1D,1D,27), _EX(27,9E,9E,B9), \
   _EX(D9,E1,E1,38), _EX(EB,F8,F8,13), _EX(2B,98,98,B3), _EX(22,11,11,33), \
   _EX(D2,69,69,BB), _EX(A9,D9,D9,70), _EX(07,8E,8E,89), _EX(33,94,94,A7), \
   _EX(2D,9B,9B,B6), _EX(3C,1E,1E,22), _EX(15,87,87,92), _EX(C9,E9,E9,20), \
   _EX(87,CE,CE,49), _EX(AA,55,55,FF), _EX(50,28,28,78), _EX(A5,DF,DF,7A), \
   _EX(03,8C,8C,8F), _EX(59,A1,A1,F8), _EX(09,89,89,80), _EX(1A,0D,0D,17), \
   _EX(65,BF,BF,DA), _EX(D7,E6,E6,31), _EX(84,42,42,C6), _EX(D0,68,68,B8), \
   _EX(82,41,41,C3), _EX(29,99,99,B0), _EX(5A,2D,2D,77), _EX(1E,0F,0F,11), \
   _EX(7B,B0,B0,CB), _EX(A8,54,54,FC), _EX(6D,BB,BB,D6), _EX(2C,16,16,3A)

#define _EX(a,b,c,d) 0x##a##b##c##d
static const uint32_t FT0[256] = { FWDT };  /*!< Forward table 0 a->b->c->d */
#undef _EX

#define _EX(a,b,c,d) 0x##d##a##b##c
static const uint32_t FT1[256] = { FWDT };  /*!< Forward table 1 d->a->b->c */
#undef _EX

#define _EX(a,b,c,d) 0x##c##d##a##b
static const uint32_t FT2[256] = { FWDT };  /*!< Forward table 2 c->d->a->b */
#undef _EX

#define _EX(a,b,c,d) 0x##b##c##d##a
static const uint32_t FT3[256] = { FWDT };  /*!< Forward table 3 b->c->d->a */
#undef _EX

#undef FWDT

/*!
 * Reverse S-box table
 */
static const uint32_t RSbox[256] =
{
   0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
   0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
   0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
   0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
   0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
   0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
   0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
   0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
   0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
   0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
   0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
   0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
   0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
   0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
   0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
   0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
   0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
   0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
   0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
   0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
   0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
   0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
   0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
   0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
   0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
   0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
   0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
   0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
   0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
   0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
   0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
   0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/*!
 *  Define for Reverse decrypt tables
 */
#define RVST \
   _EX(51,F4,A7,50), _EX(7E,41,65,53), _EX(1A,17,A4,C3), _EX(3A,27,5E,96), \
   _EX(3B,AB,6B,CB), _EX(1F,9D,45,F1), _EX(AC,FA,58,AB), _EX(4B,E3,03,93), \
   _EX(20,30,FA,55), _EX(AD,76,6D,F6), _EX(88,CC,76,91), _EX(F5,02,4C,25), \
   _EX(4F,E5,D7,FC), _EX(C5,2A,CB,D7), _EX(26,35,44,80), _EX(B5,62,A3,8F), \
   _EX(DE,B1,5A,49), _EX(25,BA,1B,67), _EX(45,EA,0E,98), _EX(5D,FE,C0,E1), \
   _EX(C3,2F,75,02), _EX(81,4C,F0,12), _EX(8D,46,97,A3), _EX(6B,D3,F9,C6), \
   _EX(03,8F,5F,E7), _EX(15,92,9C,95), _EX(BF,6D,7A,EB), _EX(95,52,59,DA), \
   _EX(D4,BE,83,2D), _EX(58,74,21,D3), _EX(49,E0,69,29), _EX(8E,C9,C8,44), \
   _EX(75,C2,89,6A), _EX(F4,8E,79,78), _EX(99,58,3E,6B), _EX(27,B9,71,DD), \
   _EX(BE,E1,4F,B6), _EX(F0,88,AD,17), _EX(C9,20,AC,66), _EX(7D,CE,3A,B4), \
   _EX(63,DF,4A,18), _EX(E5,1A,31,82), _EX(97,51,33,60), _EX(62,53,7F,45), \
   _EX(B1,64,77,E0), _EX(BB,6B,AE,84), _EX(FE,81,A0,1C), _EX(F9,08,2B,94), \
   _EX(70,48,68,58), _EX(8F,45,FD,19), _EX(94,DE,6C,87), _EX(52,7B,F8,B7), \
   _EX(AB,73,D3,23), _EX(72,4B,02,E2), _EX(E3,1F,8F,57), _EX(66,55,AB,2A), \
   _EX(B2,EB,28,07), _EX(2F,B5,C2,03), _EX(86,C5,7B,9A), _EX(D3,37,08,A5), \
   _EX(30,28,87,F2), _EX(23,BF,A5,B2), _EX(02,03,6A,BA), _EX(ED,16,82,5C), \
   _EX(8A,CF,1C,2B), _EX(A7,79,B4,92), _EX(F3,07,F2,F0), _EX(4E,69,E2,A1), \
   _EX(65,DA,F4,CD), _EX(06,05,BE,D5), _EX(D1,34,62,1F), _EX(C4,A6,FE,8A), \
   _EX(34,2E,53,9D), _EX(A2,F3,55,A0), _EX(05,8A,E1,32), _EX(A4,F6,EB,75), \
   _EX(0B,83,EC,39), _EX(40,60,EF,AA), _EX(5E,71,9F,06), _EX(BD,6E,10,51), \
   _EX(3E,21,8A,F9), _EX(96,DD,06,3D), _EX(DD,3E,05,AE), _EX(4D,E6,BD,46), \
   _EX(91,54,8D,B5), _EX(71,C4,5D,05), _EX(04,06,D4,6F), _EX(60,50,15,FF), \
   _EX(19,98,FB,24), _EX(D6,BD,E9,97), _EX(89,40,43,CC), _EX(67,D9,9E,77), \
   _EX(B0,E8,42,BD), _EX(07,89,8B,88), _EX(E7,19,5B,38), _EX(79,C8,EE,DB), \
   _EX(A1,7C,0A,47), _EX(7C,42,0F,E9), _EX(F8,84,1E,C9), _EX(00,00,00,00), \
   _EX(09,80,86,83), _EX(32,2B,ED,48), _EX(1E,11,70,AC), _EX(6C,5A,72,4E), \
   _EX(FD,0E,FF,FB), _EX(0F,85,38,56), _EX(3D,AE,D5,1E), _EX(36,2D,39,27), \
   _EX(0A,0F,D9,64), _EX(68,5C,A6,21), _EX(9B,5B,54,D1), _EX(24,36,2E,3A), \
   _EX(0C,0A,67,B1), _EX(93,57,E7,0F), _EX(B4,EE,96,D2), _EX(1B,9B,91,9E), \
   _EX(80,C0,C5,4F), _EX(61,DC,20,A2), _EX(5A,77,4B,69), _EX(1C,12,1A,16), \
   _EX(E2,93,BA,0A), _EX(C0,A0,2A,E5), _EX(3C,22,E0,43), _EX(12,1B,17,1D), \
   _EX(0E,09,0D,0B), _EX(F2,8B,C7,AD), _EX(2D,B6,A8,B9), _EX(14,1E,A9,C8), \
   _EX(57,F1,19,85), _EX(AF,75,07,4C), _EX(EE,99,DD,BB), _EX(A3,7F,60,FD), \
   _EX(F7,01,26,9F), _EX(5C,72,F5,BC), _EX(44,66,3B,C5), _EX(5B,FB,7E,34), \
   _EX(8B,43,29,76), _EX(CB,23,C6,DC), _EX(B6,ED,FC,68), _EX(B8,E4,F1,63), \
   _EX(D7,31,DC,CA), _EX(42,63,85,10), _EX(13,97,22,40), _EX(84,C6,11,20), \
   _EX(85,4A,24,7D), _EX(D2,BB,3D,F8), _EX(AE,F9,32,11), _EX(C7,29,A1,6D), \
   _EX(1D,9E,2F,4B), _EX(DC,B2,30,F3), _EX(0D,86,52,EC), _EX(77,C1,E3,D0), \
   _EX(2B,B3,16,6C), _EX(A9,70,B9,99), _EX(11,94,48,FA), _EX(47,E9,64,22), \
   _EX(A8,FC,8C,C4), _EX(A0,F0,3F,1A), _EX(56,7D,2C,D8), _EX(22,33,90,EF), \
   _EX(87,49,4E,C7), _EX(D9,38,D1,C1), _EX(8C,CA,A2,FE), _EX(98,D4,0B,36), \
   _EX(A6,F5,81,CF), _EX(A5,7A,DE,28), _EX(DA,B7,8E,26), _EX(3F,AD,BF,A4), \
   _EX(2C,3A,9D,E4), _EX(50,78,92,0D), _EX(6A,5F,CC,9B), _EX(54,7E,46,62), \
   _EX(F6,8D,13,C2), _EX(90,D8,B8,E8), _EX(2E,39,F7,5E), _EX(82,C3,AF,F5), \
   _EX(9F,5D,80,BE), _EX(69,D0,93,7C), _EX(6F,D5,2D,A9), _EX(CF,25,12,B3), \
   _EX(C8,AC,99,3B), _EX(10,18,7D,A7), _EX(E8,9C,63,6E), _EX(DB,3B,BB,7B), \
   _EX(CD,26,78,09), _EX(6E,59,18,F4), _EX(EC,9A,B7,01), _EX(83,4F,9A,A8), \
   _EX(E6,95,6E,65), _EX(AA,FF,E6,7E), _EX(21,BC,CF,08), _EX(EF,15,E8,E6), \
   _EX(BA,E7,9B,D9), _EX(4A,6F,36,CE), _EX(EA,9F,09,D4), _EX(29,B0,7C,D6), \
   _EX(31,A4,B2,AF), _EX(2A,3F,23,31), _EX(C6,A5,94,30), _EX(35,A2,66,C0), \
   _EX(74,4E,BC,37), _EX(FC,82,CA,A6), _EX(E0,90,D0,B0), _EX(33,A7,D8,15), \
   _EX(F1,04,98,4A), _EX(41,EC,DA,F7), _EX(7F,CD,50,0E), _EX(17,91,F6,2F), \
   _EX(76,4D,D6,8D), _EX(43,EF,B0,4D), _EX(CC,AA,4D,54), _EX(E4,96,04,DF), \
   _EX(9E,D1,B5,E3), _EX(4C,6A,88,1B), _EX(C1,2C,1F,B8), _EX(46,65,51,7F), \
   _EX(9D,5E,EA,04), _EX(01,8C,35,5D), _EX(FA,87,74,73), _EX(FB,0B,41,2E), \
   _EX(B3,67,1D,5A), _EX(92,DB,D2,52), _EX(E9,10,56,33), _EX(6D,D6,47,13), \
   _EX(9A,D7,61,8C), _EX(37,A1,0C,7A), _EX(59,F8,14,8E), _EX(EB,13,3C,89), \
   _EX(CE,A9,27,EE), _EX(B7,61,C9,35), _EX(E1,1C,E5,ED), _EX(7A,47,B1,3C), \
   _EX(9C,D2,DF,59), _EX(55,F2,73,3F), _EX(18,14,CE,79), _EX(73,C7,37,BF), \
   _EX(53,F7,CD,EA), _EX(5F,FD,AA,5B), _EX(DF,3D,6F,14), _EX(78,44,DB,86), \
   _EX(CA,AF,F3,81), _EX(B9,68,C4,3E), _EX(38,24,34,2C), _EX(C2,A3,40,5F), \
   _EX(16,1D,C3,72), _EX(BC,E2,25,0C), _EX(28,3C,49,8B), _EX(FF,0D,95,41), \
   _EX(39,A8,01,71), _EX(08,0C,B3,DE), _EX(D8,B4,E4,9C), _EX(64,56,C1,90), \
   _EX(7B,CB,84,61), _EX(D5,32,B6,70), _EX(48,6C,5C,74), _EX(D0,B8,57,42)

#define _EX(a,b,c,d) 0x##a##b##c##d
static const uint32_t RT0[256] = { RVST };  /*!< Reverse table 0 a->b->c->d */
#undef _EX

#define _EX(a,b,c,d) 0x##d##a##b##c
static const uint32_t RT1[256] = { RVST };  /*!< Reverse table 1 d->a->b->c */
#undef _EX

#define _EX(a,b,c,d) 0x##c##d##a##b
static const uint32_t RT2[256] = { RVST };  /*!< Reverse table 2 c->d->a->b */
#undef _EX

#define _EX(a,b,c,d) 0x##b##c##d##a
static const uint32_t RT3[256] = { RVST };  /*!< Reverse table 1 b->c->d->a */
#undef _EX

#undef RVST

/*!
 * Round constants table
 */
static const uint32_t RCON[10] =
{
    0x01000000,
    0x02000000,
    0x04000000,
    0x08000000,
    0x10000000,
    0x20000000,
    0x40000000,
    0x80000000,
    0x1B000000,
    0x36000000
};

#else
/*!<
 * \brief
 *    Forward Reverse S-box & Tables will placed in ram
 *    and calculated the first time we call \sa aes_set_key()
 *    the init routine is \sa _aes_create_tables()
 */

uint32_t FSbox[256];   /*!< Forward S-box table */
uint32_t FT0[256];     /*!< Forward table 0 */
uint32_t FT1[256];     /*!< Forward table 1 */
uint32_t FT2[256];     /*!< Forward table 2 */
uint32_t FT3[256];     /*!< Forward table 3 */

/*
 * Reverse S-box & tables
 */
uint32_t RSbox[256];   /*!< Reverse S-box table */
uint32_t RT0[256];     /*!< Reverse table 0 */
uint32_t RT1[256];     /*!< Reverse table 1 */
uint32_t RT2[256];     /*!< Reverse table 2 */
uint32_t RT3[256];     /*!< Reverse table 3 */

uint32_t RCON[10];     /*!< Round constants */

static int _init_flag = 1;    /*!< RAM-Tables initialization flag */

#endif


#ifdef RAM_TABLES

static void _aes_create_tables (void)
{
   int i;
   uint8_t x, y;
   uint8_t pow[256];
   uint8_t log[256];

   /* compute pow and log tables over GF(2^8) */
   for( i=0, x=1 ; i<256 ; ++i, x^=XTIME(x) )
   {
      pow[i] = x;
      log[x] = i;
   }

   /* calculate the round constants */
   for( i=0, x=1 ; i<10 ; ++i, x=XTIME(x) )
      RCON[i] = (uint32) x << 24;

   /* generate the forward and reverse S-boxes */
   FSbox[0x00] = 0x63;
   RSbox[0x63] = 0x00;

   for( i=1 ; i<256; ++i )
   {
      x = pow[255 - log[i]];

      y = x;  y = ( y << 1 ) | ( y >> 7 );
      x ^= y; y = ( y << 1 ) | ( y >> 7 );
      x ^= y; y = ( y << 1 ) | ( y >> 7 );
      x ^= y; y = ( y << 1 ) | ( y >> 7 );
      x ^= y ^ 0x63;

      FSbox[i] = x;
      RSbox[x] = i;
   }

   /* generate the forward and reverse tables */
   for( i=0 ; i<256 ; ++i )
   {
      x = (unsigned char) FSbox[i]; y = XTIME( x );

      FT0[i] = (uint32) ( x ^ y ) ^
               ( (uint32) x <<  8 ) ^
               ( (uint32) x << 16 ) ^
               ( (uint32) y << 24 );

      FT0[i] &= 0xFFFFFFFF;

      FT1[i] = ROTR8( FT0[i] );
      FT2[i] = ROTR8( FT1[i] );
      FT3[i] = ROTR8( FT2[i] );

      y = (unsigned char) RSbox[i];

      RT0[i] = ( (uint32) MUL( 0x0B, y )       ) ^
               ( (uint32) MUL( 0x0D, y ) <<  8 ) ^
               ( (uint32) MUL( 0x09, y ) << 16 ) ^
               ( (uint32) MUL( 0x0E, y ) << 24 );

      RT0[i] &= 0xFFFFFFFF;

      RT1[i] = ROTR8( RT0[i] );
      RT2[i] = ROTR8( RT1[i] );
      RT3[i] = ROTR8( RT2[i] );
   }
}
#endif

/*!
 * \brief
 *    Clears the AES context memory for security
 * \return  none
 */
void aes_key_deinit (aes_t *ctx)
{
   memset (&ctx, 0, sizeof (aes_t));
}

/*!
 * \brief
 *    AES key scheduling routine.
 *
 * \param ctx     the active aes context data to fill.
 * \param key     pointer to key
 * \param size    AES size
 *    \arg        AES_128
 *    \arg        AES_192
 *    \arg        AES_256
 *
 * \return        zero on success, non zero on error.
 */
int aes_key_init (aes_t *ctx, uint8_t *key, aes_size size)
{
   int i;
   uint32_t *RK, *DK;  // Pointer to round and decryption key tables

   #ifdef RAM_TABLES
   if (init_flag)
   {
      _aes_create_tables();
      _init_flag = 0;
   }
   #endif

   // Update number of rounds
   switch (size)
   {
      case AES_128:  ctx->nr = 10; break;
      case AES_192:  ctx->nr = 12; break;
      case AES_256:  ctx->nr = 14; break;
      default :      return 1;
   }
   RK = ctx->erk;

   for(i=0; i<(size>>5); ++i)
      GET_UINT32 (RK[i], key, i*4);

   // setup encryption round keys
   switch (size)
   {
      case AES_128:
         for( i=0 ; i<10 ; ++i, RK+=4 )
         {
            RK[4]  = RK[0] ^ RCON[i] ^
               ( FSbox[ (uint8_t) ( RK[3] >> 16 ) ] << 24 ) ^
               ( FSbox[ (uint8_t) ( RK[3] >>  8 ) ] << 16 ) ^
               ( FSbox[ (uint8_t) ( RK[3]       ) ] <<  8 ) ^
               ( FSbox[ (uint8_t) ( RK[3] >> 24 ) ]       );

            RK[5]  = RK[1] ^ RK[4];
            RK[6]  = RK[2] ^ RK[5];
            RK[7]  = RK[3] ^ RK[6];
         }
         break;

      case AES_192:
         for( i=0 ; i<8 ; ++i, RK+=6 )
         {
            RK[6]  = RK[0] ^ RCON[i] ^
               ( FSbox[ (uint8_t) ( RK[5] >> 16 ) ] << 24 ) ^
               ( FSbox[ (uint8_t) ( RK[5] >>  8 ) ] << 16 ) ^
               ( FSbox[ (uint8_t) ( RK[5]       ) ] <<  8 ) ^
               ( FSbox[ (uint8_t) ( RK[5] >> 24 ) ]       );

            RK[7]  = RK[1] ^ RK[6];
            RK[8]  = RK[2] ^ RK[7];
            RK[9]  = RK[3] ^ RK[8];
            RK[10] = RK[4] ^ RK[9];
            RK[11] = RK[5] ^ RK[10];
         }
         break;

      case AES_256:
         for( i=0 ; i<7; ++i, RK+=8 )
         {
            RK[8]  = RK[0] ^ RCON[i] ^
               ( FSbox[ (uint8_t) ( RK[7] >> 16 ) ] << 24 ) ^
               ( FSbox[ (uint8_t) ( RK[7] >>  8 ) ] << 16 ) ^
               ( FSbox[ (uint8_t) ( RK[7]       ) ] <<  8 ) ^
               ( FSbox[ (uint8_t) ( RK[7] >> 24 ) ]       );

            RK[9]  = RK[1] ^ RK[8];
            RK[10] = RK[2] ^ RK[9];
            RK[11] = RK[3] ^ RK[10];

            RK[12] = RK[4] ^
               ( FSbox[ (uint8_t) ( RK[11] >> 24 ) ] << 24 ) ^
               ( FSbox[ (uint8_t) ( RK[11] >> 16 ) ] << 16 ) ^
               ( FSbox[ (uint8_t) ( RK[11] >>  8 ) ] <<  8 ) ^
               ( FSbox[ (uint8_t) ( RK[11]       ) ]       );

            RK[13] = RK[5] ^ RK[12];
            RK[14] = RK[6] ^ RK[13];
            RK[15] = RK[7] ^ RK[14];
         }
         break;
   }

   DK = ctx->drk;

   for (i=0 ; i<4 ; ++i)
      *DK++ = *RK++;

   for( i=1 ; i<ctx->nr ; ++i )
   {
      RK -= 8;

      *DK++ = RT0 [ FSbox[ (uint8_t) ( *RK >> 24 ) ]] ^
              RT1 [ FSbox[ (uint8_t) ( *RK >> 16 ) ]] ^
              RT2 [ FSbox[ (uint8_t) ( *RK >>  8 ) ]] ^
              RT3 [ FSbox[ (uint8_t) ( *RK       ) ]]; RK++;

      *DK++ = RT0 [ FSbox[ (uint8_t) ( *RK >> 24 ) ]] ^
              RT1 [ FSbox[ (uint8_t) ( *RK >> 16 ) ]] ^
              RT2 [ FSbox[ (uint8_t) ( *RK >>  8 ) ]] ^
              RT3 [ FSbox[ (uint8_t) ( *RK       ) ]]; RK++;

      *DK++ = RT0 [ FSbox[ (uint8_t) ( *RK >> 24 ) ]] ^
              RT1 [ FSbox[ (uint8_t) ( *RK >> 16 ) ]] ^
              RT2 [ FSbox[ (uint8_t) ( *RK >>  8 ) ]] ^
              RT3 [ FSbox[ (uint8_t) ( *RK       ) ]]; RK++;

      *DK++ = RT0 [ FSbox[ (uint8_t) ( *RK >> 24 ) ]] ^
              RT1 [ FSbox[ (uint8_t) ( *RK >> 16 ) ]] ^
              RT2 [ FSbox[ (uint8_t) ( *RK >>  8 ) ]] ^
              RT3 [ FSbox[ (uint8_t) ( *RK       ) ]]; RK++;
   }
   RK -= 8;

   for (i=0 ; i<4 ; ++i)
      *DK++ = *RK++;

   return 0;
}

/*!
 *  AES 128-bit block encryption routine
 */
#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)        \
{                                                  \
    RK += 4;                                       \
    X0 = RK[0] ^ FT0[ (uint8_t) ( Y0 >> 24 ) ] ^   \
                 FT1[ (uint8_t) ( Y1 >> 16 ) ] ^   \
                 FT2[ (uint8_t) ( Y2 >>  8 ) ] ^   \
                 FT3[ (uint8_t) ( Y3       ) ];    \
                                                   \
    X1 = RK[1] ^ FT0[ (uint8_t) ( Y1 >> 24 ) ] ^   \
                 FT1[ (uint8_t) ( Y2 >> 16 ) ] ^   \
                 FT2[ (uint8_t) ( Y3 >>  8 ) ] ^   \
                 FT3[ (uint8_t) ( Y0       ) ];    \
                                                   \
    X2 = RK[2] ^ FT0[ (uint8_t) ( Y2 >> 24 ) ] ^   \
                 FT1[ (uint8_t) ( Y3 >> 16 ) ] ^   \
                 FT2[ (uint8_t) ( Y0 >>  8 ) ] ^   \
                 FT3[ (uint8_t) ( Y1       ) ];    \
                                                   \
    X3 = RK[3] ^ FT0[ (uint8_t) ( Y3 >> 24 ) ] ^   \
                 FT1[ (uint8_t) ( Y0 >> 16 ) ] ^   \
                 FT2[ (uint8_t) ( Y1 >>  8 ) ] ^   \
                 FT3[ (uint8_t) ( Y2       ) ];    \
}

/*!
 * \brief
 *    Encrypt a buffer.
 * \note
 *    The input and output buffer can be the same.
 *
 * \param input   buffer holding the input data
 * \param output  buffer holding the output data
 * \return        none
 */
void aes_encrypt (aes_t *ctx, uint8_t in[16], uint8_t out[16])
{
   uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

   RK = ctx->erk;

   GET_UINT32 (X0, in,  0); X0 ^= RK[0];
   GET_UINT32 (X1, in,  4); X1 ^= RK[1];
   GET_UINT32 (X2, in,  8); X2 ^= RK[2];
   GET_UINT32 (X3, in, 12); X3 ^= RK[3];

   AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 1
   AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 2
   AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 3
   AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 4
   AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 5
   AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 6
   AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 7
   AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 8
   AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 9
   if (ctx->nr > 10)
   {
      AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);   // round 10
      AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);   // round 11
   }
   if (ctx->nr > 12)
   {
      AES_FROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);   // round 12
      AES_FROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);   // round 13
   }

   // last round
   RK += 4;
   X0 = RK[0] ^ ( FSbox[ (uint8_t) ( Y0 >> 24 ) ] << 24 ) ^
                ( FSbox[ (uint8_t) ( Y1 >> 16 ) ] << 16 ) ^
                ( FSbox[ (uint8_t) ( Y2 >>  8 ) ] <<  8 ) ^
                ( FSbox[ (uint8_t) ( Y3       ) ]       );

   X1 = RK[1] ^ ( FSbox[ (uint8_t) ( Y1 >> 24 ) ] << 24 ) ^
                ( FSbox[ (uint8_t) ( Y2 >> 16 ) ] << 16 ) ^
                ( FSbox[ (uint8_t) ( Y3 >>  8 ) ] <<  8 ) ^
                ( FSbox[ (uint8_t) ( Y0       ) ]       );

   X2 = RK[2] ^ ( FSbox[ (uint8_t) ( Y2 >> 24 ) ] << 24 ) ^
                ( FSbox[ (uint8_t) ( Y3 >> 16 ) ] << 16 ) ^
                ( FSbox[ (uint8_t) ( Y0 >>  8 ) ] <<  8 ) ^
                ( FSbox[ (uint8_t) ( Y1       ) ]       );

   X3 = RK[3] ^ ( FSbox[ (uint8_t) ( Y3 >> 24 ) ] << 24 ) ^
                ( FSbox[ (uint8_t) ( Y0 >> 16 ) ] << 16 ) ^
                ( FSbox[ (uint8_t) ( Y1 >>  8 ) ] <<  8 ) ^
                ( FSbox[ (uint8_t) ( Y2       ) ]       );

   PUT_UINT32 (X0, out,  0);
   PUT_UINT32 (X1, out,  4);
   PUT_UINT32 (X2, out,  8);
   PUT_UINT32 (X3, out, 12);
}

/*!
 *  AES 128-bit block decryption routine
 */
#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)        \
{                                                  \
    RK += 4;                                       \
    X0 = RK[0] ^ RT0[ (uint8_t) ( Y0 >> 24 ) ] ^   \
                 RT1[ (uint8_t) ( Y3 >> 16 ) ] ^   \
                 RT2[ (uint8_t) ( Y2 >>  8 ) ] ^   \
                 RT3[ (uint8_t) ( Y1       ) ];    \
                                                   \
    X1 = RK[1] ^ RT0[ (uint8_t) ( Y1 >> 24 ) ] ^   \
                 RT1[ (uint8_t) ( Y0 >> 16 ) ] ^   \
                 RT2[ (uint8_t) ( Y3 >>  8 ) ] ^   \
                 RT3[ (uint8_t) ( Y2       ) ];    \
                                                   \
    X2 = RK[2] ^ RT0[ (uint8_t) ( Y2 >> 24 ) ] ^   \
                 RT1[ (uint8_t) ( Y1 >> 16 ) ] ^   \
                 RT2[ (uint8_t) ( Y0 >>  8 ) ] ^   \
                 RT3[ (uint8_t) ( Y3       ) ];    \
                                                   \
    X3 = RK[3] ^ RT0[ (uint8_t) ( Y3 >> 24 ) ] ^   \
                 RT1[ (uint8_t) ( Y2 >> 16 ) ] ^   \
                 RT2[ (uint8_t) ( Y1 >>  8 ) ] ^   \
                 RT3[ (uint8_t) ( Y0       ) ];    \
}

/*!
 * \brief
 *    Decrypt a buffer.
 * \note
 *    The input and output buffer can be the same.
 *
 * \param input   buffer holding the input data
 * \param output  buffer holding the output data
 * \return        none
 */
void aes_decrypt (aes_t *ctx, uint8_t in[16], uint8_t out[16])
{
   uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

   RK = ctx->drk;

   GET_UINT32 (X0, in,  0); X0 ^= RK[0];
   GET_UINT32 (X1, in,  4); X1 ^= RK[1];
   GET_UINT32 (X2, in,  8); X2 ^= RK[2];
   GET_UINT32 (X3, in, 12); X3 ^= RK[3];

   AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 1
   AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 2
   AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 3
   AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 4
   AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 5
   AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 6
   AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 7
   AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);       // round 8
   AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);       // round 9
   if (ctx->nr > 10)
   {
      AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);   // round 10
      AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);   // round 11
   }
   if (ctx->nr > 12)
   {
      AES_RROUND (X0, X1, X2, X3, Y0, Y1, Y2, Y3);   // round 12
      AES_RROUND (Y0, Y1, Y2, Y3, X0, X1, X2, X3);   // round 13
   }

   // Last round
   RK += 4;
   X0 = RK[0] ^ ( RSbox[ (uint8_t) ( Y0 >> 24 ) ] << 24 ) ^
                ( RSbox[ (uint8_t) ( Y3 >> 16 ) ] << 16 ) ^
                ( RSbox[ (uint8_t) ( Y2 >>  8 ) ] <<  8 ) ^
                ( RSbox[ (uint8_t) ( Y1       ) ]       );

   X1 = RK[1] ^ ( RSbox[ (uint8_t) ( Y1 >> 24 ) ] << 24 ) ^
                ( RSbox[ (uint8_t) ( Y0 >> 16 ) ] << 16 ) ^
                ( RSbox[ (uint8_t) ( Y3 >>  8 ) ] <<  8 ) ^
                ( RSbox[ (uint8_t) ( Y2       ) ]       );

   X2 = RK[2] ^ ( RSbox[ (uint8_t) ( Y2 >> 24 ) ] << 24 ) ^
                ( RSbox[ (uint8_t) ( Y1 >> 16 ) ] << 16 ) ^
                ( RSbox[ (uint8_t) ( Y0 >>  8 ) ] <<  8 ) ^
                ( RSbox[ (uint8_t) ( Y3       ) ]       );

   X3 = RK[3] ^ ( RSbox[ (uint8_t) ( Y3 >> 24 ) ] << 24 ) ^
                ( RSbox[ (uint8_t) ( Y2 >> 16 ) ] << 16 ) ^
                ( RSbox[ (uint8_t) ( Y1 >>  8 ) ] <<  8 ) ^
                ( RSbox[ (uint8_t) ( Y0       ) ]       );

   PUT_UINT32 (X0, out,  0);
   PUT_UINT32 (X1, out,  4);
   PUT_UINT32 (X2, out,  8);
   PUT_UINT32 (X3, out, 12);
}

