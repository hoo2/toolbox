/*!
 * \file serializer.c
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
#include <com/serializer.h>

//! serialize a byte in big endian from \p from to \p to and return 1
static size_t _ser_bigen8 (void *to, void *from) {
   *(uint8_t*)to = *(uint8_t*)from;
   return 1;
}

//! serialize 16bit data in big endian from \p from to \p to and return 2
static size_t _ser_bigen16 (void *to, void *from) {
   uint16_t f = *(uint16_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f >> 8);
   *(uint8_t*)to   = (uint8_t) (f);
   return 2;
}

//! serialize 32bit data in big endian from \p from to \p to and return 4
static size_t _ser_bigen32 (void *to, void *from) {
   uint32_t f = *(uint32_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f >> 24);
   *(uint8_t*)to++ = (uint8_t) (f >> 16);
   *(uint8_t*)to++ = (uint8_t) (f >> 8);
   *(uint8_t*)to   = (uint8_t) (f);
   return 4;
}

//! serialize 64bit data in big endian from \p from to \p to and return 8
static size_t _ser_bigen64 (void *to, void *from) {
   uint64_t f = *(uint64_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f >> 56);
   *(uint8_t*)to++ = (uint8_t) (f >> 48);
   *(uint8_t*)to++ = (uint8_t) (f >> 40);
   *(uint8_t*)to++ = (uint8_t) (f >> 32);
   *(uint8_t*)to++ = (uint8_t) (f >> 24);
   *(uint8_t*)to++ = (uint8_t) (f >> 16);
   *(uint8_t*)to++ = (uint8_t) (f >> 8);
   *(uint8_t*)to   = (uint8_t) (f);
   return 8;
}

//! serialize a byte in little endian from \p from to \p to and return 1
static size_t _ser_liten8 (void *to, void *from) {
   *(uint8_t*)to = *(uint8_t*)from;
   return 1;
}

//! serialize 16bit data in little endian from \p from to \p to and return 2
static size_t _ser_liten16 (void *to, void *from) {
   uint16_t f = *(uint16_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f);
   *(uint8_t*)to   = (uint8_t) (f >> 8);
   return 2;
}

//! serialize 32bit data in little endian from \p from to \p to and return 4
static size_t _ser_liten32 (void *to, void *from) {
   uint32_t f = *(uint32_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f);
   *(uint8_t*)to++ = (uint8_t) (f >> 8);
   *(uint8_t*)to++ = (uint8_t) (f >> 16);
   *(uint8_t*)to   = (uint8_t) (f >> 24);
   return 4;
}

//! serialize 64bit data in little endian from \p from to \p to and return 8
static size_t _ser_liten64 (void *to, void *from) {
   uint64_t f = *(uint64_t*)from;
   *(uint8_t*)to++ = (uint8_t) (f);
   *(uint8_t*)to++ = (uint8_t) (f >> 8);
   *(uint8_t*)to++ = (uint8_t) (f >> 16);
   *(uint8_t*)to++ = (uint8_t) (f >> 24);
   *(uint8_t*)to++ = (uint8_t) (f >> 32);
   *(uint8_t*)to++ = (uint8_t) (f >> 40);
   *(uint8_t*)to++ = (uint8_t) (f >> 48);
   *(uint8_t*)to   = (uint8_t) (f >> 56);
   return 8;
}

//! deserialize a byte in big endian from \p from to \p to and return 1
static size_t _deser_bigen8 (void *to, void *from) {
   *(uint8_t*)to = *(uint8_t*)from;
   return 1;
}

//! deserialize 16bit data in big endian from \p from to \p to and return 2
static size_t _deser_bigen16 (void *to, void *from) {
   uint16_t f = *(uint16_t*)from;
   *(uint16_t*)to  = (uint8_t) (f);       *(uint16_t*)to <<= 8;
   *(uint16_t*)to |= (uint8_t) (f >> 8);
   return 2;
}

//! deserialize 32bit data in big endian from \p from to \p to and return 4
static size_t _deser_bigen32 (void *to, void *from) {
   uint32_t f = *(uint32_t*)from;
   *(uint32_t*)to  = (uint8_t) (f);       *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f >> 8);  *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f >> 16); *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f >> 24);
   return 4;
}

//! deserialize 64bit data in big endian from \p from to \p to and return 8
static size_t _deser_bigen64 (void *to, void *from) {
   uint64_t f = *(uint64_t*)from;
   *(uint64_t*)to  = (uint8_t) (f);       *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 8);  *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 16); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 24); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 32); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 40); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 48); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 56);
   return 8;
}

//! deserialize a byte in little endian from \p from to \p to and return 1
static size_t _deser_liten8 (void *to, void *from) {
   *(uint8_t*)to = *(uint8_t*)from;
   return 1;
}

//! deserialize 16bit data in little endian from \p from to \p to and return 2
static size_t _deser_liten16 (void *to, void *from) {
   uint16_t f = *(uint16_t*)from;
   *(uint16_t*)to  = (uint8_t) (f >> 8);  *(uint16_t*)to <<= 8;
   *(uint16_t*)to |= (uint8_t) (f);
   return 2;
}

//! deserialize 32bit data in little endian from \p from to \p to and return 4
static size_t _deser_liten32 (void *to, void *from) {
   uint32_t f = *(uint32_t*)from;
   *(uint32_t*)to  = (uint8_t) (f >> 24); *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f >> 16); *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f >> 8);  *(uint32_t*)to <<= 8;
   *(uint32_t*)to |= (uint8_t) (f);
   return 4;
}

//! deserialize 64bit data in little endian from \p from to \p to and return 8
static size_t _deser_liten64 (void *to, void *from) {
   uint64_t f = *(uint64_t*)from;
   *(uint64_t*)to  = (uint8_t) (f >> 56); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 48); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 40); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 32); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 24); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 16); *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f >> 8);  *(uint64_t*)to <<= 8;
   *(uint64_t*)to |= (uint8_t) (f);
   return 8;
}


/*
 * Public API
 */

/*!
 * Tool to get the serialized data size without serialization
 * @param schema  The scheme to use
 * @return        The size
 */
size_t serialize_size (const ser_schema_t* schema) {
   size_t s =0;
   for (int i =0 ; schema[i].type != s_none; ++i) {
      for (size_t t =0 ; t<schema[i].times ; ++t) {
         switch (schema[i].type) {
            case s_none:
            default:    break;
            case s_pad:
            case s_u8:
            case s_i8:  s += 1;  break;
            case s_u16:
            case s_i16: s += 2;  break;
            case s_u32:
            case s_i32:
            case s_f32: s += 4;  break;
            case s_u64:
            case s_i64:
            case s_f64: s += 8;  break;
         }
      }
   }
   return s;
}

/*!
 * Serialize data from a struct to a data stream. This serialize all the instruction
 * defined in \p schema parameter.
 * @note
 *    This algorithm is one pass
 * @param to      Pointer to data stream for output
 * @param from    Pointer to struct for input
 * @param schema  The scheme array to use
 * @param endian  The endian to use
 * @return        The number of serialized bytes
 */
size_t serialize (void* to, void* from, const ser_schema_t* schema, ser_endian_en endian) {
   // select endian machinery
   size_t  (*ser8) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _ser_bigen8 : _ser_liten8;
   size_t (*ser16) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _ser_bigen16: _ser_liten16;
   size_t (*ser32) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _ser_bigen32: _ser_liten32;
   size_t (*ser64) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _ser_bigen64: _ser_liten64;
   size_t s =0;
   for (int i =0 ; schema[i].type != s_none; ++i) {
      for (size_t t =0 ; t<schema[i].times ; ++t) {
         size_t adv =0;
         switch (schema[i].type) {
            case s_none:
            default:    break;
            case s_pad: adv =  ser8 (to+s, (void*)&schema[i].offset);      break;
            case s_u8:
            case s_i8:  adv =  ser8 (to+s, from + schema[i].offset + 1*t); break;
            case s_u16:
            case s_i16: adv = ser16 (to+s, from + schema[i].offset + 2*t); break;
            case s_u32:
            case s_i32:
            case s_f32: adv = ser32 (to+s, from + schema[i].offset + 4*t); break;
            case s_u64:
            case s_i64:
            case s_f64: adv = ser64 (to+s, from + schema[i].offset + 8*t); break;
         }
         s += adv;
      }
   }
   return s;
}

/*!
 * De-serialize data back to struct. This deserialize all the instruction
 * defined in \p schema parameter.
 * @note
 *    This algorithm is one pass
 * @param to      Pointer to struct for output
 * @param from    Pointer to serialized data stream
 * @param schema  Scheme array to use
 * @param endian  Endian to use
 * @return        The number of read bytes
 */
size_t deserialize (void* to, void* from, const ser_schema_t* schema, ser_endian_en endian) {
   // select endian machinery
   size_t  (*deser8) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _deser_bigen8 : _deser_liten8;
   size_t (*deser16) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _deser_bigen16: _deser_liten16;
   size_t (*deser32) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _deser_bigen32: _deser_liten32;
   size_t (*deser64) (void*, void*) = (endian == SER_BIG_ENDIAN) ? _deser_bigen64: _deser_liten64;
   size_t s =0;
   for (int i =0 ; schema[i].type != s_none; ++i) {
      for (size_t t =0 ; t<schema[i].times ; ++t) {
         size_t adv =0;
         switch (schema[i].type) {
            case s_none:
            default:    break;
            case s_pad: adv = 1; break;
            case s_u8:
            case s_i8:  adv = deser8 (to + schema[i].offset + 1*t, from+s);  break;
            case s_u16:
            case s_i16: adv = deser16 (to + schema[i].offset + 2*t, from+s); break;
            case s_u32:
            case s_i32:
            case s_f32: adv = deser32 (to + schema[i].offset + 4*t, from+s); break;
            case s_u64:
            case s_i64:
            case s_f64: adv = deser64 (to + schema[i].offset + 8*t, from+s); break;
         }
         s += adv;
      }
   }
   return s;
}
