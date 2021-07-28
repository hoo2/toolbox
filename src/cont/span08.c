/*!
 * \file span08.c
 * \brief
 *    This file provides span capability for 8bit objects
 *
 * Copyright (C) 2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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
#include <cont/span08.h>




span08_t span08_init(byte_t* data) {
   return (span08_t){0,data};
}

span08_t span08_mk(byte_t* data, size_t num, ...) {
   span08_t s = {0, data};

   va_list args;
   va_start(args, num);
   for ( ; s.size < num ; ++s.size) {
      s.data[s.size] = (byte_t)va_arg(args, int32_t);
   }
   va_end(args);
   return s;   // copy return
}

span08_t span08_add (span08_t* span, byte_t it) {
   span->data[span->size++] = it;
   return *span;
}

span08_t span08_set (span08_t* span, size_t num, ...) {
   va_list args;
   va_start(args, num);
   for (span->size =0 ; span->size < num ; ++span->size) {
      span->data[span->size] = (byte_t)va_arg(args, int32_t);
   }
   va_end(args);
   return *span;
}

span08_t span08_cpy (span08_t* span, byte_t* data, size_t num) {
   memcpy((void*)data, (const void*)span->data, num);
   return *span;
}

span08_t span08_cat (span08_t* span, span08_t src) {
   for (size_t i=0 ; i<src.size ; ++i)
      span08_add(span, src.data[i]);
   return *span;
}

byte_t* span08_get (span08_t* span) { return span->data; }
