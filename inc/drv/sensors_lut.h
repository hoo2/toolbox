 /*

	Sensors.h - Is the sensors input resistance to temperature, lux etx translator
      API header file, for various sensor types.
   includes:
      PT100, PT1000, NTC10k (B25/85: 3977K), PerkinElmer A9060 11 Photocell

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
   Date:       8/2011
   Version:    0.1
 */

#ifndef __sensors_lut_h__
#define __sensors_lut_h__

#include <math.h>
#include <limits.h>
#include <stdint.h>


/* ================        General         ======================*/
#define  TEMP_ERROR    (-274.0)        // Bellow Absolute zero

// Local type Definition
typedef float  res_t;
typedef float  temp_t;
typedef float  lut_t;

/*
 * ================== Exported API to Lib ==================
 */
float sen_nclut (float v, const lut_t *F, const float *T);
float sen_pclut (float v, const lut_t *F, const float *T);

/*!
 * \note
 *    Dont need to use these functions directly. The sen_pt100(), sen_pt1000(),
 *    sen_kty8x_121() etc... functions are beter ;)
 */

#endif //#ifndef __sensors_lut_h__

