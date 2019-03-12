/*
 * \file det.c
 * \brief
 *    A simple algorithm to calculate determinant
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2018 Choutouridis Christos
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
#include <math/det.h>

static float temp[_DET_MINORS_SIZE]; // Memory for minors

#define  _access_i32(_A, _n, _i, _j)      ( ((int32_t*)((_A)+((_i)*(_n))))[(_j)] )
#define  _access_ui32(_A, _n, _i, _j)     ( ((uint32_t*)((_A)+((_i)*(_n))))[(_j)] )
#define  _access_f(_A, _n, _i, _j)        ( ((float*)((_A)+((_i)*(_n))))[(_j)] )
#define  _access_d(_A, _n, _i, _j)        ( ((double*)((_A)+((_i)*(_n))))[(_j)] )

static void* _al (void* pool, int size) {
   static uint8_t *p =0;
   uint8_t *ret =0;

   if (p == 0) {
      // first time initialization
      p = (uint8_t*) pool;
   }
   ret = p;             // Return the entry value of pool
   p += size;           // allocate
   return (void*)ret;
}


static void _minor (float *M, float *A, int Asize, int Ai, int Aj) {
   for (int i =0, Mi =0 ; i<Asize ; ++i) {
      if (Ai != i) {
         for (int j =0, Mj =0 ; j<Asize ; ++j) {
            if (Aj != j) {
               _access_f (M, Asize-1, Mi, Mj) = _access_f (A, Asize, i, j);
               ++Mj;
            }
         }
         ++Mi;
      }
   }
}


float detf (float *A, int n)
{
   //static int i =0, j =-1;    // recursive call counter for cache pointer
   float *m =0;               // pointer to current minor
   float d =0;            // return value
   int s =1;

   //++j;  // Count recursive calls
   // Filter the recursion
   if (n < 0)
      d = NAN;                   // Error
   else if (n == 1)
      d = _access_f(A, n, 0, 0); // Size one
   else if (n == 2) {
      // Base case scenario
      d = _access_f(A, 2, 0, 0)*_access_f(A, 2, 1, 1)
        - _access_f(A, 2, 1, 0)*_access_f(A, 2, 0, 1);
      // cache the result
      //_access_f(_cache, DET_MAX_SIZE, i, j-1) = d;
   }
   else {
      // Allocate memory for [n-1 x n-1] minors
      m = (float*)_al (temp, (n-1)*(n-1)*sizeof(float));
      for (int ii =0 ; ii<n ; ++ii, s *=-1) {
//         if (n==3 && !isnan (dd = _cache[j]) )
//            d += dd;
//         else {
            _minor (m, A, n, ii, 0);                      // Extract minor
            d += s*_access_f(A, n, ii, 0)*detf (m, n-1);  // Recursive calls
         //}
      }
      _al (temp, -(n-1)*(n-1)*sizeof(float));   // free memory
   }
   //--j;  // Count recursive calls
   return d;
}
