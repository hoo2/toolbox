/*!
 * \file serializer.h
 * \ingroup Communication
 * \brief
 *    A target independent Serializer/Deserializer
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015-2019 Choutouridis Christos (http://www.houtouridis.net)
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

#ifndef __serializer_h__
#define __serializer_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>

#define _member_offset(str, mem)    ((size_t)((uint8_t*)&str.mem - (uint8_t*)&str))
#define _member_pad(p)              (p)

/*!
 * Serializer schema data structure. Arrays of this acts as
 * instruction to serializer/deserializer
 * \example
 * \code
 * struct SerTest {
 *    int32_t  a;
 *    uint32_t b[5];
 *    float    f;
 *    struct S {
 *       int8_t   u8;
 *       uint32_t c;
 *    }s;
 *    char     str[20];
 * };
 * struct SerTest serTest = {
 *    .a = 0x11223344,
 *    .b = {0, 1, 2, 3, 4},
 *    .f = 2.0,
 *    .s = { .u8 = 0x10, .c  = 0x10000000 },
 *    .str = {'h', 'e', 'l', 'l', 'o'},
 * };
 *
 * const ser_schema_t sch[] = {
 *    {s_i32, 1, _member_offset(serTest, a)},
 *    {s_u32, 5, _member_offset(serTest, b)},
 *    {s_f32, 1, _member_offset(serTest, f)},
 *    {s_i8,  1, _member_offset(serTest, s.u8)},
 *    {s_u32, 1, _member_offset(serTest, s.c)},
 *    {s_i8, 20, _member_offset(serTest, str)},
 *    {0,0,0}
 * };
 *
 * int main () {
 *    uint8_t buffer[32];
 *    serialize ((void*)buffer, (void*)&serTest, sch, SER_BIG_ENDIAN);
 *    deserialize((void*)&serTest, (void*)buffer, sch, SER_BIG_ENDIAN);
 *    return 0;
 * }
 * \endcode
 */
typedef struct {
   enum {
      s_none=0, s_pad, s_u8, s_u16, s_u32, s_u64, s_i8, s_i16, s_i32, s_i64, s_f32, s_f64,
   }           type;    //!< Type of data to transfer
   size_t      times;   //!< How many times to repeat the current, while advancing offset
                        //!  This is used for arrays
   size_t      offset;  //!< Pointer offset of the current struct member
}ser_schema_t;

/*!
 * Endian selector enumerator
 */
typedef enum {
   SER_BIG_ENDIAN =0,   //!< Select BIG_ENDIAN
   SER_LITTLE_ENDIAN    //!< Select LITTLE_ENDIAN
}ser_endian_en;

size_t serialize_size (const ser_schema_t* schema);
size_t serialize (void* to, void* from, const ser_schema_t* schema, ser_endian_en endian);
size_t deserialize (void* to, void* from, const ser_schema_t* schema, ser_endian_en endian);


#ifdef __cplusplus
extern }
#endif

#endif /* __serializer_h__ */
