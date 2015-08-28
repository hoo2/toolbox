/*
 * \file math.c
 * \brief
 *    Small and quick math functions, that are not general cases
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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
#include <math/math.h>

/*!
 * \brief
 *    Calculate the Log2N
 * \note    N has to be power of 2
 */
uint32_t _log2 (int32_t n) {
   uint32_t r=1;

   while (n>1) {
      n >>= 1;
      ++r;
   }
   return r-1;
}

/*!
 * \brief
 *    Calculate the 2^e power for integer e
 */
uint32_t _pow2 (uint32_t e) {
   uint32_t r = 1;

   if (e == 0)
      return 1;
   else {
      for ( ; e>0 ; --e)
         r <<= 1;
      return r;
   }
}
