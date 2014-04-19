/*

	Filters.h - Is the filters API header.
   includes:
      Averege Low pass filter

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       7/2012
   Version:    0.1
 */

#ifndef __Filters_h__
#define __Filters_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* =================== Data types ===================== */
// Data type for Buffer filtering
typedef volatile struct
{
   float       *pb;  // pointer to buffer
   uint8_t     size;
   uint8_t     c;    // cursor
}MovingAv_t;

typedef volatile struct
{
   float       out;
   float       lambda;
}LeakyInt_t;


/* =================== Exported Functions ===================== */

int MovingAv_DeInit (MovingAv_t* buffer);
int MovingAv_Init (MovingAv_t* buffer, uint8_t points);    // Init the Moving average points
float MovingAv (MovingAv_t* buffer, float value);   // return the filtered value

void LeakyInt_DeInit (LeakyInt_t* li);
void LeakyInt_Init (LeakyInt_t* li, float l);
float LeakyInt (LeakyInt_t* li, float value);

#ifdef __cplusplus
}
#endif

#endif

