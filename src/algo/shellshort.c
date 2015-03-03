/*
 * \file shellshort.c
 * \brief
 *    Shell short implementation using Marcin Ciura's gap sequence
 *
 * This file is part of toolbox
 *
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
#include <algo/shellshort.h>

/*!
 * Marcin Ciura's gap sequence
 */
static const int _gaps[] = {701, 301, 132, 57, 23, 10, 4, 1, 0};

void sshort (int *a, int n)
{
   int i,j,temp;
   uint8_t g;

   // Start with the largest gap and work down to a gap of 1
   for (g=0 ; _gaps[g] ; ++g) {
      // Do a gapped insertion sort for this gap size.
      for (i=_gaps[g] ; i<n ; ++i) {
         // add a[i] to the elements that have been gap sorted
         // save a[i] in temp and make a hole at position i
         temp = a[i];
         // shift earlier gap-sorted elements up until the correct location for a[i] is found
         for (j=i; j>=_gaps[g] && a[j-_gaps[g]]>temp ; j-= _gaps[g])
            a[j] = a[j-_gaps[g]];
         // put temp (the original a[i]) in its correct location
         a[j] = temp;
      }
   }
}
